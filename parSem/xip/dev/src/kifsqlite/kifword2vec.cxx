/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the XIP library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : XIP
Version    : 10.00
filename   : kiftemplate.cxx
Date       : 09/09/2010
Purpose    : A KiF library template
Programmer : Claude ROUX
Reviewer   :
*/

//Use KIF_DLL only in Windows, if you link to the KiF DLL
//#define KIF_DLL
#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"
#include "kifmaps.h"
#include "kiftreemaps.h"
#include <math.h>
#ifdef WIN32
#include <process.h>
#define testfinite _finite
#else
#include <pthread.h>
#ifdef C11
#define testfinite std::isfinite
#else
#ifdef ARCH64
#define testfinite isfinite
#else
#define testfinite std::isfinite
#endif
#endif
#endif

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

//Replace kifword2vec by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType kifword2vec_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitializationWord2Vec for an example
//of how to declare a new method.
class Kifkifword2vec;
//This typedef defines a type "kifword2vecMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifkifword2vec::*kifword2vecMethod)(KifElement*, KifDomain*, KifCallFunction*, int idthread);

static KifType word2vecitem_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitializationWord2Vec for an example
//of how to declare a new method.
class Kifword2vecitem;
//This typedef defines a type "word2vecitemMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifword2vecitem::*word2vecitemMethod)(KifElement*, KifDomain*, KifCallFunction*, int idthread);

#define W2VEC_MAX_STRING 512
#define W2VEC_EXP_W2VEC_table_size 1000
#define W2VEC_MAX_EXP 6
#define W2VEC_MAX_SENTENCE_LENGTH 1000
#define W2VEC_MAX_CODE_LENGTH 40

const long long max_size = 10000;         // max length of strings
const long long analogy_N = 40;                  // number of closest words that will be shown
const long long distance_N = 40;                  // number of closest words that will be shown
const long long accuracy_N = 1;                   // number of closest a_words
const long long max_w = 50;              // max length of vocabulary entries

const int W2VEC_vocab_hash_size = 30000000;  // Maximum 30 * 0.7 = 21M words in the vocabulary

#ifdef W2VSMALL
static const unsigned long long learningconstant = 2521490391;
#else
static const unsigned long long learningconstant = 25214903917;
#endif

typedef float real;                    // Precision of float numbers

struct W2VEC_vocab_word {
	long long cn;
	int *point;
	char *word, *code, codelen;
};


const int W2VEC_table_size = 1e8;

// Used later for sorting by word counts
int W2VVocabCompare(const void *a, const void *b) {
	return ((struct W2VEC_vocab_word *)b)->cn - ((struct W2VEC_vocab_word *)a)->cn;
}

class kifword2vec;
static kifword2vec* current = NULL;

#ifdef WIN32
static vector<bool> tobejoined;
unsigned __stdcall TrainModelThread(void* arg);
unsigned __stdcall TrainModelThreadVector(void* arg);
#else
void *TrainModelThread(void *id);
void *TrainModelThreadVector(void *id);
#endif



class kifword2vec {
public:

	KifCode* kifcode;
	char train_file[W2VEC_MAX_STRING], output_file[W2VEC_MAX_STRING], save_context_file[W2VEC_MAX_STRING], read_context_file[W2VEC_MAX_STRING];
	char save_vocab_file[W2VEC_MAX_STRING], read_vocab_file[W2VEC_MAX_STRING];
	struct W2VEC_vocab_word *vocab;
	int binary, cbow, debug_mode, window, min_count, num_threads, min_reduce;
	int *vocab_hash;
	long long vocab_max_size, vocab_size, layer1_size;
	long long train_words, word_count_actual, iter, file_size, classes;
	real alpha, starting_alpha, sample;
	real *syn0, *syn1, *syn1neg, *expTable;
	clock_t start;
	int hs, negative;
	int *table;
	float *M;
	char *a_vocab;
	long long a_words, a_size;
	KifTreeMapStringInteger d_vocab;
	KifElement* inputwords;

	kifword2vec(KifCode* klc) : d_vocab(NULL, NULL) {
		output_file[0] = 0;
		save_context_file[0] = 0;
		read_context_file[0] = 0;
		save_vocab_file[0] = 0;
		read_vocab_file[0] = 0;

		kifcode = klc;
		d_vocab.kifcode = kifcode;
		d_vocab.reference = 30000;
		vocab = NULL;
		binary = 0; cbow = 1; debug_mode = 2; window = 5; min_count = 5; num_threads = 12; min_reduce = 1;
		vocab_hash = NULL;
		vocab_max_size = 1000; vocab_size = 0; layer1_size = 100;
		train_words = 0; word_count_actual = 0; iter = 5; file_size = 0; classes = 0;
		alpha = 0.025; sample = 1e-3;
		hs = 0; negative = 5;
		vocab = NULL;
		M = NULL;
		a_vocab = NULL;
		inputwords = NULL;
		syn0 = NULL;
		syn1 = NULL;
		syn1neg = NULL;
		expTable = NULL;
	}

	~kifword2vec() {
		if (vocab != NULL){
			free(vocab);
			free(vocab_hash);
			free(expTable);
		}
		clearvocab();
#ifdef WIN32
		if (syn0 != NULL)
			_aligned_free(syn0);
		if (syn1 != NULL)
			_aligned_free(syn1);
		if (syn1neg != NULL)
			_aligned_free(syn1neg);
#else
		if (syn0 != NULL)
			free(syn0);
		if (syn1 != NULL)
			free(syn1);
		if (syn1neg != NULL)
			free(syn1neg);
#endif
	}

	void clearvocab() {
		if (M != NULL)
			free(M);
		M = NULL;
		if (a_vocab != NULL)
			free(a_vocab);
		a_vocab = NULL;
	}

	KifElement* init(KifElement* themap) {

		output_file[0] = 0;
		save_context_file[0] = 0;
		read_context_file[0] = 0;
		save_vocab_file[0] = 0;
		read_vocab_file[0] = 0;

		KifIteration* itr = (KifIteration*)themap->Newiterator(false);
		string key;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			key = itr->IteratorKeyString();

			if (key == "size") {
				layer1_size = itr->IteratorValueInteger();
				continue;
			}
			if (key == "train") {
				strcpy(train_file, STR(itr->IteratorValueString()));
				continue;
			}

			if (key == "save-context") {
				strcpy(save_context_file, STR(itr->IteratorValueString()));
				continue;
			}

			if (key == "read-context") {
				strcpy(read_context_file, STR(itr->IteratorValueString()));
				continue;
			}

			if (key == "save-vocab") {
				strcpy(save_vocab_file, STR(itr->IteratorValueString()));
				continue;
			}
			if (key == "read-vocab") {
				strcpy(read_vocab_file, STR(itr->IteratorValueString()));
				continue;
			}

			if (key == "debug") {
				debug_mode = itr->IteratorValueInteger();
				continue;
			}
			if (key == "binary") {
				binary = itr->IteratorValueInteger();
				continue;
			}
			if (key == "cbow") {
				cbow = itr->IteratorValueInteger();
				if (cbow) alpha = 0.05;
				continue;
			}

			if (key == "alpha") {
				alpha = itr->IteratorValueFloat();
				continue;
			}

			if (key == "output"){
				strcpy(output_file, STR(itr->IteratorValueString()));
				continue;
			}

			if (key == "window") {
				window = itr->IteratorValueInteger();
				continue;
			}

			if (key == "sample") {
				sample = itr->IteratorValueFloat();
				continue;
			}

			if (key == "hs") {
				hs = itr->IteratorValueInteger();
				continue;
			}
			if (key == "negative") {
				negative = itr->IteratorValueInteger();
				continue;
			}

			if (key == "threads") {
				num_threads = itr->IteratorValueInteger();
				continue;
			}

			if (key == "iter") {
				iter = itr->IteratorValueInteger();
				continue;
			}

			if (key == "min-count") {
				min_count = itr->IteratorValueInteger();
				continue;
			}

			if (key == "classes") {
				classes = itr->IteratorValueInteger();
				continue;
			}

			string errmsg("W2V(333): Unkown option: ");
			errmsg += key;

			return kifcode->Returnerror(errmsg);
		}

		itr->Release();

		if (read_context_file[0] && read_vocab_file[0] == 0)
			return kifcode->Returnerror("W2V(032): Missing vocabulary file");

		if (vocab != NULL){
			free(vocab);
			free(vocab_hash);
			free(expTable);
		}

		vocab = (struct W2VEC_vocab_word *)calloc(vocab_max_size, sizeof(struct W2VEC_vocab_word));
		vocab_hash = (int *)calloc(W2VEC_vocab_hash_size, sizeof(int));
		expTable = (real *)malloc((W2VEC_EXP_W2VEC_table_size + 1) * sizeof(real));
		for (int i = 0; i < W2VEC_EXP_W2VEC_table_size; i++) {
			expTable[i] = exp((i / (real)W2VEC_EXP_W2VEC_table_size * 2 - 1) * W2VEC_MAX_EXP); // Precompute the exp() table
			expTable[i] = expTable[i] / (expTable[i] + 1);                   // Precompute f(x) = x / (x + 1)
		}
		return kifTRUE;
	}


	void InitUnigramTable() {
		int a, i;
		long long train_words_pow = 0;
		real d1, power = 0.75;
		table = (int *)malloc(W2VEC_table_size * sizeof(int));
		for (a = 0; a < vocab_size; a++) train_words_pow += pow(vocab[a].cn, power);
		i = 0;
		d1 = pow(vocab[i].cn, power) / (real)train_words_pow;
		for (a = 0; a < W2VEC_table_size; a++) {
			table[a] = i;
			if (a / (real)W2VEC_table_size > d1) {
				i++;
				d1 += pow(vocab[i].cn, power) / (real)train_words_pow;
			}
			if (i >= vocab_size) i = vocab_size - 1;
		}
	}

	// Reads a single word from a file, assuming space + tab + EOL to be word boundaries
	void ReadWord(char *word, FILE *fin) {
		int a = 0, ch;
		while (!feof(fin)) {
			ch = fgetc(fin);
			if (ch == 13) continue;
			if ((ch == ' ') || (ch == '\t') || (ch == '\n')) {
				if (a > 0) {
					if (ch == '\n') ungetc(ch, fin);
					break;
				}
				if (ch == '\n') {
					strcpy(word, (char *)"</s>");
					return;
				}
				else continue;
			}
			word[a] = ch;
			a++;
			if (a >= W2VEC_MAX_STRING - 1) a--;   // Truncate too long words
		}
		word[a] = 0;
	}

	// Returns hash value of a word
	int GetWordHash(char *word) {
		unsigned long long a, hash = 0;
		for (a = 0; a < strlen(word); a++) hash = hash * 257 + word[a];
		hash = hash % W2VEC_vocab_hash_size;
		return hash;
	}

	// Returns position of a word in the vocabulary; if the word is not found, returns -1
	int SearchVocab(char *word) {
		unsigned int hash = GetWordHash(word);
		while (1) {
			if (vocab_hash[hash] == -1) return -1;
			if (!strcmp(word, vocab[vocab_hash[hash]].word)) return vocab_hash[hash];
			hash = (hash + 1) % W2VEC_vocab_hash_size;
		}
		return -1;
	}

	// Reads a word and returns its index in the vocabulary
	int ReadWordIndex(FILE *fin) {
		char word[W2VEC_MAX_STRING];
		ReadWord(word, fin);
		if (feof(fin)) return -1;
		return SearchVocab(word);
	}

	// Adds a word to the vocabulary
	int AddWordToVocab(char *word) {
		unsigned int hash, length = strlen(word) + 1;
		if (length > W2VEC_MAX_STRING) length = W2VEC_MAX_STRING;
		vocab[vocab_size].word = (char *)calloc(length, sizeof(char));
		strcpy(vocab[vocab_size].word, word);
		vocab[vocab_size].cn = 0;
		vocab_size++;
		// Reallocate memory if needed
		if (vocab_size + 2 >= vocab_max_size) {
			vocab_max_size += 1000;
			vocab = (struct W2VEC_vocab_word *)realloc(vocab, vocab_max_size * sizeof(struct W2VEC_vocab_word));
		}
		hash = GetWordHash(word);
		while (vocab_hash[hash] != -1) hash = (hash + 1) % W2VEC_vocab_hash_size;
		vocab_hash[hash] = vocab_size - 1;
		return vocab_size - 1;
	}

	// Sorts the vocabulary by frequency using word counts
	void SortVocab() {
		int a, size;
		unsigned int hash;
		// Sort the vocabulary and keep </s> at the first position
		qsort(&vocab[1], vocab_size - 1, sizeof(struct W2VEC_vocab_word), W2VVocabCompare);
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;
		size = vocab_size;
		train_words = 0;
		for (a = 0; a < size; a++) {
			// Words occuring less than min_count times will be discarded from the vocab
			if ((vocab[a].cn < min_count) && (a != 0)) {
				vocab_size--;
				free(vocab[a].word);
			}
			else {
				// Hash will be re-computed, as after the sorting it is not actual
				hash = GetWordHash(vocab[a].word);
				while (vocab_hash[hash] != -1) hash = (hash + 1) % W2VEC_vocab_hash_size;
				vocab_hash[hash] = a;
				train_words += vocab[a].cn;
			}
		}
		vocab = (struct W2VEC_vocab_word *)realloc(vocab, (vocab_size + 1) * sizeof(struct W2VEC_vocab_word));
		// Allocate memory for the binary tree construction
		for (a = 0; a < vocab_size; a++) {
			vocab[a].code = (char *)calloc(W2VEC_MAX_CODE_LENGTH, sizeof(char));
			vocab[a].point = (int *)calloc(W2VEC_MAX_CODE_LENGTH, sizeof(int));
		}
	}

	// Reduces the vocabulary by removing infrequent tokens
	void ReduceVocab() {
		int a, b = 0;
		unsigned int hash;
		for (a = 0; a < vocab_size; a++) if (vocab[a].cn > min_reduce) {
			vocab[b].cn = vocab[a].cn;
			vocab[b].word = vocab[a].word;
			b++;
		}
		else free(vocab[a].word);
		vocab_size = b;
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;
		for (a = 0; a < vocab_size; a++) {
			// Hash will be re-computed, as it is not actual
			hash = GetWordHash(vocab[a].word);
			while (vocab_hash[hash] != -1) hash = (hash + 1) % W2VEC_vocab_hash_size;
			vocab_hash[hash] = a;
		}
		fflush(stdout);
		min_reduce++;
	}

	// Create binary Huffman tree using the word counts
	// Frequent words will have short uniqe binary codes
	void CreateBinaryTree() {
		long long a, b, i, min1i, min2i, pos1, pos2, point[W2VEC_MAX_CODE_LENGTH];
		char code[W2VEC_MAX_CODE_LENGTH];
		long long *count = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
		long long *binary = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
		long long *parent_node = (long long *)calloc(vocab_size * 2 + 1, sizeof(long long));
		for (a = 0; a < vocab_size; a++) count[a] = vocab[a].cn;
		for (a = vocab_size; a < vocab_size * 2; a++) count[a] = 1e15;
		pos1 = vocab_size - 1;
		pos2 = vocab_size;
		// Following algorithm constructs the Huffman tree by adding one node at a time
		for (a = 0; a < vocab_size - 1; a++) {
			// First, find two smallest nodes 'min1, min2'
			if (pos1 >= 0) {
				if (count[pos1] < count[pos2]) {
					min1i = pos1;
					pos1--;
				}
				else {
					min1i = pos2;
					pos2++;
				}
			}
			else {
				min1i = pos2;
				pos2++;
			}
			if (pos1 >= 0) {
				if (count[pos1] < count[pos2]) {
					min2i = pos1;
					pos1--;
				}
				else {
					min2i = pos2;
					pos2++;
				}
			}
			else {
				min2i = pos2;
				pos2++;
			}
			count[vocab_size + a] = count[min1i] + count[min2i];
			parent_node[min1i] = vocab_size + a;
			parent_node[min2i] = vocab_size + a;
			binary[min2i] = 1;
		}
		// Now assign binary code to each vocabulary word
		for (a = 0; a < vocab_size; a++) {
			b = a;
			i = 0;
			while (1) {
				code[i] = binary[b];
				point[i] = b;
				i++;
				b = parent_node[b];
				if (b == vocab_size * 2 - 2) break;
			}
			vocab[a].codelen = i;
			vocab[a].point[0] = vocab_size - 2;
			for (b = 0; b < i; b++) {
				vocab[a].code[i - b - 1] = code[b];
				vocab[a].point[i - b] = point[b] - vocab_size;
			}
		}
		free(count);
		free(binary);
		free(parent_node);
	}

	int LearnVocabInContext() {
		char word[W2VEC_MAX_STRING];
		FILE *fin;
		long long a, i;
		fin = fopen(train_file, "rb");
		if (fin == NULL) {
			kifcode->Returnerror("W2V(001):ERROR: training data file not found!");
			return -1;
		}
		while (1) {
			ReadWord(word, fin);
			if (feof(fin)) break;
			train_words++;
			if ((debug_mode > 1) && (train_words % 100000 == 0)) {
				printf("%lldK%c", train_words / 1000, 13);
				fflush(stdout);
			}
			i = SearchVocab(word);
			if (i == -1) {
				a = AddWordToVocab(word);
				vocab[a].cn = 1;
			}
			else vocab[i].cn++;
			if (vocab_size > W2VEC_vocab_hash_size * 0.7) ReduceVocab();
		}
		SortVocab();
		if (debug_mode > 0) {
			printf("Vocab size: %lld\n", vocab_size);
			printf("Words in train file: %lld\n", train_words);
		}
		file_size = ftell(fin);
		fclose(fin);
		return(0);
	}

	int LearnVocabFromTrainFile() {
		char word[W2VEC_MAX_STRING];
		FILE *fin;
		long long a, i;
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;
		fin = fopen(train_file, "rb");
		if (fin == NULL) {
			kifcode->Returnerror("W2V(001):ERROR: training data file not found!");
			return -1;
		}
		vocab_size = 0;
		AddWordToVocab((char *)"</s>");
		while (1) {
			ReadWord(word, fin);
			if (feof(fin)) break;
			train_words++;
			if ((debug_mode > 1) && (train_words % 100000 == 0)) {
				printf("%lldK%c", train_words / 1000, 13);
				fflush(stdout);
			}
			i = SearchVocab(word);
			if (i == -1) {
				a = AddWordToVocab(word);
				vocab[a].cn = 1;
			}
			else vocab[i].cn++;
			if (vocab_size > W2VEC_vocab_hash_size * 0.7) ReduceVocab();
		}
		SortVocab();
		if (debug_mode > 0) {
			printf("Vocab size: %lld\n", vocab_size);
			printf("Words in train file: %lld\n", train_words);
		}
		file_size = ftell(fin);
		fclose(fin);
		return 0;
	}


	void FeedVocabFromVector(KifElement* words) {
		inputwords = words;
		long long sz = inputwords->Size();

		long long a, i, w;
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;

		vocab_size = 0;
		AddWordToVocab((char *)"</s>");
		string aword;
		char baseword[W2VEC_MAX_STRING + 1];
		for (w = 0; w<sz; w++) {
			aword = inputwords->getstring(w);
			if (aword.size() >= W2VEC_MAX_STRING)
				aword = aword.substr(0, W2VEC_MAX_STRING - 1);
			memcpy(baseword, STR(aword), aword.size());
			baseword[aword.size()] = 0;
			train_words++;
			if ((debug_mode > 1) && (train_words % 100000 == 0)) {
				printf("%lldK%c", train_words / 1000, 13);
				fflush(stdout);
			}
			i = SearchVocab(baseword);
			if (i == -1) {
				a = AddWordToVocab(baseword);
				vocab[a].cn = 1;
			}
			else vocab[i].cn++;
			if (vocab_size > W2VEC_vocab_hash_size * 0.7) ReduceVocab();
		}
		SortVocab();
	}

	void SaveVocab() {
		long long i;
		FILE *fo = fopen(save_vocab_file, "wb");
		for (i = 0; i < vocab_size; i++) fprintf(fo, "%s %lld\n", vocab[i].word, vocab[i].cn);
		fclose(fo);
	}

	int ReadVocab() {
		long long a, i = 0;
		char c;
		char word[W2VEC_MAX_STRING];
		FILE *fin = fopen(read_vocab_file, "rb");
		if (fin == NULL) {
			kifcode->Returnerror("W2V(002): Vocabulary file not found");
			return -1;
		}
		for (a = 0; a < W2VEC_vocab_hash_size; a++) vocab_hash[a] = -1;
		vocab_size = 0;
		while (1) {
			ReadWord(word, fin);
			if (feof(fin)) break;
			a = AddWordToVocab(word);
			fscanf(fin, "%lld%c", &vocab[a].cn, &c);
			i++;
		}

		//If we do not have a context file, then we do not process the train file as all its vocabulary has been extracted into the vocab file
		if (!read_context_file[0]) {
			SortVocab();
			fin = fopen(train_file, "rb");
			if (fin == NULL) {
				kifcode->Returnerror("W2V(003): ERROR: training data file not found!");
				return -1;
			}

			fseek(fin, 0, SEEK_END);
			file_size = ftell(fin);
			fclose(fin);
			if (debug_mode > 0) {
				printf("Vocab size: %lld\n", vocab_size);
				printf("Words in train file: %lld\n", train_words);
			}
		}
		else {
			if (debug_mode > 0) {
				printf("Vocab size: %lld\n", vocab_size);
			}
		}
		return 0;
	}

	void InitNet() {
		long long a, b;
		unsigned long long next_random = 1;
#ifdef WIN32
		syn0 = (real*)_aligned_malloc((long long)vocab_size * layer1_size * sizeof(real), 128);
#else
		a = posix_memalign((void **)&syn0, 128, (long long)vocab_size * layer1_size * sizeof(real));
#endif
		if (syn0 == NULL) { printf("Memory allocation failed\n"); exit(1); }
		if (hs) {
#ifdef WIN32
			syn1 = (real*)_aligned_malloc((long long)vocab_size * layer1_size * sizeof(real), 128);
#else
			a = posix_memalign((void **)&syn1, 128, (long long)vocab_size * layer1_size * sizeof(real));
#endif
			if (syn1 == NULL) { printf("Memory allocation failed\n"); exit(1); }
			for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++)
				syn1[a * layer1_size + b] = 0;
		}
		if (negative>0) {
#ifdef WIN32
			syn1neg = (real*)_aligned_malloc((long long)vocab_size * layer1_size * sizeof(real), 128);
#else
			a = posix_memalign((void **)&syn1neg, 128, (long long)vocab_size * layer1_size * sizeof(real));
#endif
			if (syn1neg == NULL) { printf("Memory allocation failed\n"); exit(1); }
			for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++)
				syn1neg[a * layer1_size + b] = 0;
		}
		for (a = 0; a < vocab_size; a++) for (b = 0; b < layer1_size; b++) {
			next_random = next_random * learningconstant + 11;
			syn0[a * layer1_size + b] = (((next_random & 0xFFFF) / (real)65536) - 0.5) / layer1_size;
		}
		CreateBinaryTree();
	}


	int loadcontext(string vectorfile) {
		FILE *f;

		f = fopen(STR(vectorfile), "rb");
		if (f == NULL) {
			kifcode->Returnerror("W2V(005): Input file not found");
			return -1;
		}

		long long vsz, lsz;
		fscanf(f, "%lld", &vsz);
		fscanf(f, "%lld", &lsz);
		if (lsz != layer1_size) {
			kifcode->Returnerror("W2V(045): Mismatch between context and vocabulary file");
			return -1;
		}
		InitNet();

		long long a, b, c;
		for (a = 0; a < vsz; a++) {
			for (b = 0; b < layer1_size; b++) {
				c = a * layer1_size + b;
				fread(&syn0[c], sizeof(real), 1, f);
				if (syn1 != NULL)
					fread(&syn1[c], sizeof(real), 1, f);
				if (syn1neg != NULL)
					fread(&syn1neg[c], sizeof(real), 1, f);
			}
		}

		fclose(f);
		return(0);
	}


	int TrainModel() {
		inputwords = NULL;
		//printf("Starting training using file %s\n", train_file);
		starting_alpha = alpha;
		if (read_vocab_file[0] != 0)  {
			if (ReadVocab() != 0)
				return -1;

			if (read_context_file[0] != 0) {
				if (LearnVocabInContext() != 0)
					return -1;
			}
		}
		else {
			if (LearnVocabFromTrainFile() != 0)
				return -1;
		}
		if (save_vocab_file[0] != 0) SaveVocab();
		if (output_file[0] == 0) return-1;
		LaunchTrainModel();
		return 0;
	}

	int TrainModelFromVector(KifElement* v) {
		if (output_file[0] == 0) return-1;
		starting_alpha = alpha;
		FeedVocabFromVector(v);
		if (save_vocab_file[0] != 0) SaveVocab();
		LaunchTrainModel();
		return 0;
	}

	void LaunchTrainModel() {
#ifndef WIN32
		pthread_t *pt = (pthread_t *)malloc(num_threads * sizeof(pthread_t));
#endif
		FILE *fo;
		long a, b, c, d;
		if (read_context_file[0] != 0) {
			if (loadcontext(read_context_file))
				return;
		}
		else
			InitNet();

		if (negative > 0) InitUnigramTable();
		start = clock();
		current = this;
#ifdef WIN32
		DWORD tid;
		tobejoined.clear();
		for (a = 0; a < num_threads; a++)
			tobejoined.push_back(true);

		if (inputwords == NULL) {
			for (a = 0; a < num_threads; a++) {
				_beginthreadex(NULL, 0, TrainModelThread, (void*)a, 0, (UINT*)&tid);
			}
		}
		else {
			for (a = 0; a < num_threads; a++) {
				_beginthreadex(NULL, 0, TrainModelThreadVector, (void*)a, 0, (UINT*)&tid);
			}
		}

		for (a = 0; a < num_threads; a++) {
			ThreadLock _lock(kifInstructionWHILE);
			if (tobejoined[a] == true)
				a--;
		}
#else
		if (inputwords == NULL)
		for (a = 0; a < num_threads; a++) pthread_create(&pt[a], NULL, TrainModelThread, (void *)a);
		else
		for (a = 0; a < num_threads; a++) pthread_create(&pt[a], NULL, TrainModelThreadVector, (void *)a);

		for (a = 0; a < num_threads; a++) pthread_join(pt[a], NULL);
#endif

		if (save_context_file[0]) {
			fo = fopen(save_context_file, "wb");
			// Save the word vectors
			fprintf(fo, "%lld %lld\n", vocab_size, layer1_size);
			long c;
			for (a = 0; a < vocab_size; a++) {
				for (b = 0; b < layer1_size; b++) {
					c = a * layer1_size + b;
					fwrite(&syn0[c], sizeof(real), 1, fo);
					if (syn1 != NULL)
						fwrite(&syn1[c], sizeof(real), 1, fo);
					if (syn1neg != NULL)
						fwrite(&syn1neg[c], sizeof(real), 1, fo);
				}
			}
			fclose(fo);
		}

		fo = fopen(output_file, "wb");
		if (classes == 0) {
			// Save the word vectors
			fprintf(fo, "%lld %lld\n", vocab_size, layer1_size);
			for (a = 0; a < vocab_size; a++) {
				fprintf(fo, "%s ", vocab[a].word);
				if (binary) for (b = 0; b < layer1_size; b++) fwrite(&syn0[a * layer1_size + b], sizeof(real), 1, fo);
				else for (b = 0; b < layer1_size; b++) fprintf(fo, "%lf ", syn0[a * layer1_size + b]);
				fprintf(fo, "\n");
			}
		}
		else {
			// Run K-means on the word vectors
			int clcn = classes, iter = 10, closeid;
			int *centcn = (int *)malloc(classes * sizeof(int));
			int *cl = (int *)calloc(vocab_size, sizeof(int));
			real closev, x;
			real *cent = (real *)calloc(classes * layer1_size, sizeof(real));
			for (a = 0; a < vocab_size; a++) cl[a] = a % clcn;
			for (a = 0; a < iter; a++) {
				for (b = 0; b < clcn * layer1_size; b++) cent[b] = 0;
				for (b = 0; b < clcn; b++) centcn[b] = 1;
				for (c = 0; c < vocab_size; c++) {
					for (d = 0; d < layer1_size; d++) cent[layer1_size * cl[c] + d] += syn0[c * layer1_size + d];
					centcn[cl[c]]++;
				}
				for (b = 0; b < clcn; b++) {
					closev = 0;
					for (c = 0; c < layer1_size; c++) {
						cent[layer1_size * b + c] /= centcn[b];
						closev += cent[layer1_size * b + c] * cent[layer1_size * b + c];
					}
					closev = sqrt(closev);
					for (c = 0; c < layer1_size; c++) cent[layer1_size * b + c] /= closev;
				}
				for (c = 0; c < vocab_size; c++) {
					closev = -10;
					closeid = 0;
					for (d = 0; d < clcn; d++) {
						x = 0;
						for (b = 0; b < layer1_size; b++) x += cent[layer1_size * d + b] * syn0[c * layer1_size + b];
						if (x > closev) {
							closev = x;
							closeid = d;
						}
					}
					cl[c] = closeid;
				}
			}
			// Save the K-means classes
			for (a = 0; a < vocab_size; a++) fprintf(fo, "%s %d\n", vocab[a].word, cl[a]);
			free(centcn);
			free(cent);
			free(cl);
		}
		fclose(fo);
	}

	int loadmodel(string vectorfile, bool normalize) {
		if (M != NULL)
			clearvocab();

		FILE *f;

		f = fopen(STR(vectorfile), "rb");
		if (f == NULL) {
			kifcode->Returnerror("W2V(005): Input file not found");
			return -1;
		}

		fscanf(f, "%lld", &a_words);
		fscanf(f, "%lld", &a_size);
		a_vocab = (char *)malloc((long long)a_words * max_w * sizeof(char));
		M = (float *)malloc((long long)a_words * (long long)a_size * sizeof(float));
		if (M == NULL) {
			printf("Cannot allocate memory: %lld MB    %lld  %lld\n", (long long)a_words * a_size * sizeof(float) / 1048576, a_words, a_size);
			exit(-1);
		}

		long long a, b;
		float len;
		for (b = 0; b < a_words; b++) {
			if ((b % 10000) == 0)
				cerr << ".";
			a = 0;
			while (1) {
				a_vocab[b * max_w + a] = fgetc(f);
				if (feof(f) || (a_vocab[b * max_w + a] == ' ')) break;
				if ((a < max_w) && (a_vocab[b * max_w + a] != '\n')) a++;
			}
			a_vocab[b * max_w + a] = 0;

			if (normalize) {
				for (a = 0; a < max_w; a++) a_vocab[b * max_w + a] = toupper(a_vocab[b * max_w + a]);
			}

			for (a = 0; a < a_size; a++) fread(&M[a + b * a_size], sizeof(float), 1, f);
			len = 0;
			for (a = 0; a < a_size; a++) len += M[a + b * a_size] * M[a + b * a_size];
			len = sqrt(len);
			for (a = 0; a < a_size; a++) M[a + b * a_size] /= len;
		}
		fclose(f);
		cerr << endl;
		d_vocab.values.clear();
		for (b = 0; b < a_words; b++)
			d_vocab.values[&a_vocab[b * max_w]] = b;

		return 0;
	}


	int analogy(KifElement* vwords, KifMapStringFloat* resmap) {
		if (M == NULL) {
			kifcode->Returnerror("W2V(011): Load a model first");
			return -1;
		}

		if (vwords->Size() < 3) {
			kifcode->Returnerror("W2V(006): Requires at least two words for analogy");
			return -1;
		}

		char bestw[analogy_N][max_size];
		char st[100][max_size];
		float dist, len, bestd[analogy_N], vec[max_size];
		long long a, b, c, d, cn, bi[100];

		for (a = 0; a < analogy_N; a++) bestd[a] = 0;
		for (a = 0; a < analogy_N; a++) bestw[a][0] = 0;

		cn = 0;
		int mx = vwords->Size();
		string aword;
		for (cn = 0; cn < mx; cn++) {
			aword = vwords->getstring(cn);
			if (aword.size() >= W2VEC_MAX_STRING)
				aword = aword.substr(0, W2VEC_MAX_STRING - 1);
			memcpy(st[cn], STR(aword), aword.size());
			st[cn][aword.size()] = 0;
		}

		map<string, long>::iterator it;
		for (a = 0; a < mx; a++) {
			it = d_vocab.values.find(st[a]);
			if (it == d_vocab.values.end()) {
				kifcode->Returnerror("W2V(008): Out of dictionary word!");
				b = 0;
				break;
			}
			b = it->second;
			bi[a] = b;
		}

		if (b == 0)
			return(-1);

		for (a = 0; a < a_size; a++) vec[a] = M[a + bi[1] * a_size] - M[a + bi[0] * a_size] + M[a + bi[2] * a_size];
		len = 0;
		for (a = 0; a < a_size; a++) len += vec[a] * vec[a];
		len = sqrt(len);
		for (a = 0; a < a_size; a++) vec[a] /= len;
		for (a = 0; a < analogy_N; a++) bestd[a] = 0;
		for (a = 0; a < analogy_N; a++) bestw[a][0] = 0;
		for (c = 0; c < a_words; c++) {
			if (c == bi[0]) continue;
			if (c == bi[1]) continue;
			if (c == bi[2]) continue;
			a = 0;
			for (b = 0; b < mx; b++) if (bi[b] == c) a = 1;
			if (a == 1) continue;
			dist = 0;
			for (a = 0; a < a_size; a++) dist += vec[a] * M[a + c * a_size];
			for (a = 0; a < analogy_N; a++) {
				if (dist > bestd[a]) {
					for (d = analogy_N - 1; d > a; d--) {
						bestd[d] = bestd[d - 1];
						strcpy(bestw[d], bestw[d - 1]);
					}
					bestd[a] = dist;
					strcpy(bestw[a], &a_vocab[c * max_w]);
					break;
				}
			}
		}
		for (a = 0; a < analogy_N; a++)
			resmap->values[bestw[a]] = bestd[a];

		return 0;
	}

	int accuracy(long long threshold, KifElement* vwords, KifMapStringFloat* resmap) {
		if (M == NULL) {
			kifcode->Returnerror("W2V(011): Load a model first");
			return -1;
		}

		char bestw[accuracy_N][max_size];
		float dist, bestd[accuracy_N], vec[max_size];
		long long a, b, c, d, b1, b2, b3;
		int TCN, CCN = 0, TACN = 0, CACN = 0, SECN = 0, SYCN = 0, SEAC = 0, SYAC = 0, QID = 0, TQ = 0, TQS = 0;
		TCN = 0;
		KifIteration* itr = (KifIteration*)vwords->Newiterator(false);
		KifElement* subvect;
		agnostring key;

		map<string, long>::iterator it;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			subvect = itr->IteratorValue();
			if (subvect->Size() != 4) {
				kifcode->Returnerror("W2V(030): Expecting sub-vectors of 4 elements");
				return -1;
			}
			for (a = 0; a < accuracy_N; a++) bestd[a] = 0;
			for (a = 0; a < accuracy_N; a++) bestw[a][0] = 0;

			key = subvect->getstring(0);
			key = key.toupper();
			it = d_vocab.values.find(key);
			if (it == d_vocab.values.end())
				continue;
			b1 = it->second;

			key = subvect->getstring(1);
			key = key.toupper();
			it = d_vocab.values.find(key);
			if (it == d_vocab.values.end())
				continue;
			b2 = it->second;

			key = subvect->getstring(2);
			key = key.toupper();
			it = d_vocab.values.find(key);
			if (it == d_vocab.values.end())
				continue;
			b3 = it->second;

			key = subvect->getstring(3);
			key = key.toupper();
			it = d_vocab.values.find(key);
			if (it == d_vocab.values.end())
				continue;
			b = it->second;

			for (a = 0; a < accuracy_N; a++) bestd[a] = 0;
			for (a = 0; a < accuracy_N; a++) bestw[a][0] = 0;
			TQ++;

			for (a = 0; a < a_size; a++) vec[a] = (M[a + b2 * a_size] - M[a + b1 * a_size]) + M[a + b3 * a_size];
			TQS++;
			for (c = 0; c < a_words; c++) {
				if (c == b1) continue;
				if (c == b2) continue;
				if (c == b3) continue;
				dist = 0;
				for (a = 0; a < a_size; a++) dist += vec[a] * M[a + c * a_size];
				for (a = 0; a < accuracy_N; a++) {
					if (dist > bestd[a]) {
						for (d = accuracy_N - 1; d > a; d--) {
							bestd[d] = bestd[d - 1];
							strcpy(bestw[d], bestw[d - 1]);
						}
						bestd[a] = dist;
						strcpy(bestw[a], &a_vocab[c * max_w]);
						break;
					}
				}
			}

			if (key == bestw[0]) {
				CCN++;
				CACN++;
			}
			TCN++;
			TACN++;
		}

		itr->Release();
		if (TCN == 0) TCN = 1;
		resmap->values["accuracy top1"] = CCN / (float)TCN * 100;
		resmap->values["CCN"] = CCN;
		resmap->values["TCN"] = TCN;
		resmap->values["total accuracy"] = CACN / (float)TACN * 100;
		if (!SECN)
			resmap->values["semantic accuracy"] = 0;
		else
			resmap->values["semantic accuracy"] = SEAC / (float)SECN * 100;

		QID++;
		resmap->values["questions"] = TQS;
		resmap->values["total questions"] = TQ;
		resmap->values["ratio questions"] = TQS / (float)TQ * 100;
		return 0;
	}

	float* avector(long wrdidx) {
		float* vec = new float[max_size];
		long long a;
		for (a = 0; a < a_size; a++)
			vec[a] = M[a + wrdidx * a_size];
		return vec;
	}

	float* normalize(float* vec, float& norm) {
		long long a;
		norm = 0;
		for (a = 0; a < a_size; a++) norm += vec[a] * vec[a];
		float* vecret = new float[max_size];
		norm = sqrt(norm);
		for (a = 0; a < a_size; a++) vecret[a] = vec[a] / norm;

		return vecret;
	}

	long long wordindex(string& wrd) {
		if (d_vocab.values.find(wrd) == d_vocab.values.end())
			return -1;
		return d_vocab.values[wrd];
	}

	float dot(float* v1, float* v2) {
		float d = 0;
		long long a;
		for (a = 0; a < a_size; a++)
			d += v1[a] * v2[a];
		return fabs(d);
	}


	float dot(float* v1, string& wrd, float& norm) {
		if (d_vocab.values.find(wrd) == d_vocab.values.end())
			return 0;

		long long a;
		float* v2 = avector(d_vocab.values[wrd]);
		norm = 0;
		float d = 0;
		for (a = 0; a < a_size; a++) {
			d += v1[a] * v2[a];
			norm += v2[a] * v2[a];
		}
		norm = sqrt(norm);
		delete[] v2;
		return fabs(d);
	}

	float dot(float* v1, vector<long>& v2, float& norm) {
		if (v2.size() != a_size)
			return 0;

		float d = 0;
		long long a;
		norm = 0;
		for (a = 0; a < a_size; a++) {
			d += v1[a] * v2[a];
			norm += v2[a] * v2[a];
		}
		norm = sqrt(norm);
		return fabs(d);
	}

	float dot(float* v1, vector<double>& v2, float& norm) {
		if (v2.size() != a_size)
			return 0;

		float d = 0;
		long long a;
		norm = 0;
		for (a = 0; a < a_size; a++) {
			d += v1[a] * v2[a];
			norm += v2[a] * v2[a];
		}
		norm = sqrt(norm);
		return fabs(d);
	}

	float dot(float* v1, vector<KifElement*>& v2, float& norm) {
		if (v2.size() != a_size)
			return 0;

		float d = 0;
		long long a;
		double v;
		norm = 0;
		for (a = 0; a < a_size; a++) {
			v = v2[a]->Float();
			d += v1[a] * v;
			norm += v*v;
		}
		norm = sqrt(norm);
		return fabs(d);
	}

	float distance(float* vecnorm, long long wrdidx) {
		float dist = 0;
		if (wrdidx != -1) {
			for (long long a = 0; a < a_size; a++)
				dist += vecnorm[a] * M[a + wrdidx * a_size];
		}
		return fabs(dist);
	}

	int distance(string wrd, KifMapStringFloat* resmap) {
		char bestw[distance_N][max_size];
		char st[100][max_size];
		float dist, len, bestd[distance_N], vec[max_size];
		long long a, b, c, d, cn, bi[100];

		for (a = 0; a < distance_N; a++) bestd[a] = 0;
		for (a = 0; a < distance_N; a++) bestw[a][0] = 0;

		if (d_vocab.values.find(wrd) == d_vocab.values.end())
			return 0;

		long long idx = d_vocab.values[wrd];

		strcpy(st[0], STR(wrd));
		bi[0] = idx;
		cn = 1;

		len = 0;
		for (a = 0; a < a_size; a++)  {
			vec[a] = M[a + idx * a_size];
			len += vec[a] * vec[a];
		}
		len = sqrt(len);
		for (a = 0; a < a_size; a++) vec[a] /= len;

		for (a = 0; a < distance_N; a++) bestd[a] = -1;
		for (a = 0; a < distance_N; a++) bestw[a][0] = 0;

		for (c = 0; c < a_words; c++) {
			if (c == idx)
				continue;

			dist = 0;
			for (a = 0; a < a_size; a++) dist += vec[a] * M[a + c * a_size];
			for (a = 0; a < distance_N; a++) {
				if (dist > bestd[a]) {
					for (d = distance_N - 1; d > a; d--) {
						bestd[d] = bestd[d - 1];
						strcpy(bestw[d], bestw[d - 1]);
					}
					bestd[a] = dist;
					strcpy(bestw[a], &a_vocab[c * max_w]);
					break;
				}
			}
		}

		for (a = 0; a < distance_N; a++)  {
			if (bestw[a][0] != 0)
				resmap->values[bestw[a]] = bestd[a];
		}

		return 0;
	}

	int distance(KifElement* vwords, KifMapStringFloat* resmap) {
		char bestw[distance_N][max_size];
		char st[100][max_size];
		float dist, len, bestd[distance_N], vec[max_size];
		long long a, b, c, d, cn, bi[100];

		for (a = 0; a < distance_N; a++) bestd[a] = 0;
		for (a = 0; a < distance_N; a++) bestw[a][0] = 0;


		KifIteration* itr = (KifIteration*)vwords->Newiterator(false);
		a = 0;
		c = 0;
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			strcpy(st[a++], STR(itr->IteratorValueString()));
			c++;
		}
		itr->Release();

		map<string, long>::iterator it;
		cn = 0;
		for (a = 0; a < c; a++) {
			it = d_vocab.values.find(st[a]);
			if (it == d_vocab.values.end())
				continue;
			b = it->second;
			bi[cn++] = b;
		}

		for (a = 0; a < a_size; a++) vec[a] = 0;
		for (b = 0; b < cn; b++) {
			if (bi[b] == -1)
				continue;
			for (a = 0; a < a_size; a++)
				vec[a] += M[a + bi[b] * a_size];
		}

		len = 0;
		for (a = 0; a < a_size; a++) len += vec[a] * vec[a];
		len = sqrt(len);
		for (a = 0; a < a_size; a++) vec[a] /= len;
		for (a = 0; a < distance_N; a++) bestd[a] = -1;
		for (a = 0; a < distance_N; a++) bestw[a][0] = 0;
		for (c = 0; c < a_words; c++) {
			a = 0;
			for (b = 0; b < cn; b++) {
				if (bi[b] == c) {
					a = 1;
					break;
				}
			}

			if (a == 1) continue;
			dist = 0;
			for (a = 0; a < a_size; a++) dist += vec[a] * M[a + c * a_size];
			for (a = 0; a < distance_N; a++) {
				if (dist > bestd[a]) {
					for (d = distance_N - 1; d > a; d--) {
						bestd[d] = bestd[d - 1];
						strcpy(bestw[d], bestw[d - 1]);
					}
					bestd[a] = dist;
					strcpy(bestw[a], &a_vocab[c * max_w]);
					break;
				}
			}
		}

		for (a = 0; a < distance_N; a++)  {
			if (bestw[a][0] != 0)
				resmap->values[bestw[a]] = bestd[a];
		}

		return 0;
	}
};

#ifdef WIN32
unsigned __stdcall TrainModelThread(void* id) {
#else
void *TrainModelThread(void *id) {
#endif
	long long a, b, d, cw, word, last_word, sentence_length = 0, sentence_position = 0;
	long long word_count = 0, last_word_count = 0, sen[W2VEC_MAX_SENTENCE_LENGTH + 1];
	long long l1, l2, c, target, label, local_iter = current->iter;
	unsigned long long next_random = (long long)id;
	real f, g;
	clock_t now;
	real *neu1 = (real *)calloc(current->layer1_size, sizeof(real));
	real *neu1e = (real *)calloc(current->layer1_size, sizeof(real));
	FILE *fi = fopen(current->train_file, "rb");
	fseek(fi, current->file_size / (long long)current->num_threads * (long long)id, SEEK_SET);
	while (1) {
		if (word_count - last_word_count > 10000) {
			current->word_count_actual += word_count - last_word_count;
			last_word_count = word_count;
			if ((current->debug_mode > 1)) {
				now = clock();
				printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, current->alpha,
					current->word_count_actual / (real)(current->iter * current->train_words + 1) * 100,
					current->word_count_actual / ((real)(now - current->start + 1) / (real)CLOCKS_PER_SEC * 1000));
				fflush(stdout);
			}
			current->alpha = current->starting_alpha * (1 - current->word_count_actual / (real)(current->iter * current->train_words + 1));
			if (current->alpha < current->starting_alpha * 0.0001) current->alpha = current->starting_alpha * 0.0001;
		}
		if (sentence_length == 0) {
			while (1) {
				word = current->ReadWordIndex(fi);
				if (feof(fi)) break;
				if (word == -1) continue;
				word_count++;
				if (word == 0) break;
				// The subsampling randomly discards frequent words while keeping the ranking same
				if (current->sample > 0) {
					real ran = (sqrt(current->vocab[word].cn / (current->sample * current->train_words)) + 1) * (current->sample * current->train_words) / current->vocab[word].cn;
					next_random = next_random * learningconstant + 11;
					if (ran < (next_random & 0xFFFF) / (real)65536) continue;
				}
				sen[sentence_length] = word;
				sentence_length++;
				if (sentence_length >= W2VEC_MAX_SENTENCE_LENGTH) break;
			}
			sentence_position = 0;
		}
		if (feof(fi) || (word_count > current->train_words / current->num_threads)) {
			current->word_count_actual += word_count - last_word_count;
			local_iter--;
			if (local_iter == 0) break;
			word_count = 0;
			last_word_count = 0;
			sentence_length = 0;
			fseek(fi, current->file_size / (long long)current->num_threads * (long long)id, SEEK_SET);
			continue;
		}
		word = sen[sentence_position];
		if (word == -1) continue;
		for (c = 0; c < current->layer1_size; c++) neu1[c] = 0;
		for (c = 0; c < current->layer1_size; c++) neu1e[c] = 0;
		next_random = next_random * learningconstant + 11;
		b = next_random % current->window;
		if (current->cbow) {  //train the cbow architecture
			// in -> hidden
			cw = 0;
			for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
				c = sentence_position - current->window + a;
				if (c < 0) continue;
				if (c >= sentence_length) continue;
				last_word = sen[c];
				if (last_word == -1) continue;
				for (c = 0; c < current->layer1_size; c++) neu1[c] += current->syn0[c + last_word * current->layer1_size];
				cw++;
			}
			if (cw) {
				for (c = 0; c < current->layer1_size; c++) neu1[c] /= cw;
				if (current->hs) for (d = 0; d < current->vocab[word].codelen; d++) {
					f = 0;
					l2 = current->vocab[word].point[d] * current->layer1_size;
					// Propagate hidden -> output
					for (c = 0; c < current->layer1_size; c++) f += neu1[c] * current->syn1[c + l2];
					if (f <= -W2VEC_MAX_EXP) continue;
					else if (f >= W2VEC_MAX_EXP) continue;
					else f = current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))];
					// 'g' is the gradient multiplied by the learning rate
					g = (1 - current->vocab[word].code[d] - f) * current->alpha;
					// Propagate errors output -> hidden
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1[c + l2];
					// Learn weights hidden -> output
					for (c = 0; c < current->layer1_size; c++) current->syn1[c + l2] += g * neu1[c];
				}
				// NEGATIVE SAMPLING
				if (current->negative > 0) for (d = 0; d < current->negative + 1; d++) {
					if (d == 0) {
						target = word;
						label = 1;
					}
					else {
						next_random = next_random * learningconstant + 11;
						target = current->table[(next_random >> 16) % W2VEC_table_size];
						if (target == 0) target = next_random % (current->vocab_size - 1) + 1;
						if (target == word) continue;
						label = 0;
					}
					l2 = target * current->layer1_size;
					f = 0;
					for (c = 0; c < current->layer1_size; c++) f += neu1[c] * current->syn1neg[c + l2];
					if (!testfinite(f)) g = (label - 0) * current->alpha;
					else
					if (f > W2VEC_MAX_EXP) g = (label - 1) * current->alpha;
					else if (f < -W2VEC_MAX_EXP) g = (label - 0) * current->alpha;
					else g = (label - current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))]) * current->alpha;
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1neg[c + l2];
					for (c = 0; c < current->layer1_size; c++) current->syn1neg[c + l2] += g * neu1[c];
				}
				// hidden -> in
				for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
					c = sentence_position - current->window + a;
					if (c < 0) continue;
					if (c >= sentence_length) continue;
					last_word = sen[c];
					if (last_word == -1) continue;
					for (c = 0; c < current->layer1_size; c++) current->syn0[c + last_word * current->layer1_size] += neu1e[c];
				}
			}
		}
		else {  //train skip-gram
			for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
				c = sentence_position - current->window + a;
				if (c < 0) continue;
				if (c >= sentence_length) continue;
				last_word = sen[c];
				if (last_word == -1) continue;
				l1 = last_word * current->layer1_size;
				for (c = 0; c < current->layer1_size; c++) neu1e[c] = 0;
				// HIERARCHICAL SOFTMAX
				if (current->hs) for (d = 0; d < current->vocab[word].codelen; d++) {
					f = 0;
					l2 = current->vocab[word].point[d] * current->layer1_size;
					// Propagate hidden -> output
					for (c = 0; c < current->layer1_size; c++) f += current->syn0[c + l1] * current->syn1[c + l2];
					if (f <= -W2VEC_MAX_EXP) continue;
					else if (f >= W2VEC_MAX_EXP) continue;
					else f = current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))];
					// 'g' is the gradient multiplied by the learning rate
					g = (1 - current->vocab[word].code[d] - f) * current->alpha;
					// Propagate errors output -> hidden
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1[c + l2];
					// Learn weights hidden -> output
					for (c = 0; c < current->layer1_size; c++) current->syn1[c + l2] += g * current->syn0[c + l1];
				}
				// NEGATIVE SAMPLING
				if (current->negative > 0) for (d = 0; d < current->negative + 1; d++) {
					if (d == 0) {
						target = word;
						label = 1;
					}
					else {
						next_random = next_random * learningconstant + 11;
						target = current->table[(next_random >> 16) % W2VEC_table_size];
						if (target == 0) target = next_random % (current->vocab_size - 1) + 1;
						if (target == word) continue;
						label = 0;
					}
					l2 = target * current->layer1_size;
					f = 0;
					for (c = 0; c < current->layer1_size; c++) f += current->syn0[c + l1] * current->syn1neg[c + l2];
					if (!testfinite(f)) g = (label - 0) * current->alpha;
					else
					if (f > W2VEC_MAX_EXP) g = (label - 1) * current->alpha;
					else if (f < -W2VEC_MAX_EXP) g = (label - 0) * current->alpha;
					else g = (label - current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))]) * current->alpha;
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1neg[c + l2];
					for (c = 0; c < current->layer1_size; c++) current->syn1neg[c + l2] += g * current->syn0[c + l1];
				}
				// Learn weights input -> hidden
				for (c = 0; c < current->layer1_size; c++) current->syn0[c + l1] += neu1e[c];
			}
		}
		sentence_position++;
		if (sentence_position >= sentence_length) {
			sentence_length = 0;
			continue;
		}
	}
	fclose(fi);
	free(neu1);
	free(neu1e);
#ifdef WIN32
	ThreadLock _lock(kifInstructionWHILE);
	tobejoined[(int)id] = false;
	return(0);
#else
	pthread_exit(NULL);
#endif
}

#ifdef WIN32
unsigned __stdcall TrainModelThreadVector(void* id) {
#else
void *TrainModelThreadVector(void *id) {
#endif
	long long a, b, d, cw, word, last_word, sentence_length = 0, sentence_position = 0;
	long long word_count = 0, last_word_count = 0, sen[W2VEC_MAX_SENTENCE_LENGTH + 1];
	long long l1, l2, c, target, label, local_iter = current->iter;
	unsigned long long next_random = (long long)id;
	real f, g;
	clock_t now;
	real *neu1 = (real *)calloc(current->layer1_size, sizeof(real));
	real *neu1e = (real *)calloc(current->layer1_size, sizeof(real));

	//We need to select the part of the vector which are going to analyze...
	KifElement* inputwords = current->inputwords;

	int baseidx = inputwords->Size() / (long long)current->num_threads * (long long)id;
	int idx = baseidx;

	while (1) {
		if (word_count - last_word_count > 10000) {
			current->word_count_actual += word_count - last_word_count;
			last_word_count = word_count;
			if ((current->debug_mode > 1)) {
				now = clock();
				printf("%cAlpha: %f  Progress: %.2f%%  Words/thread/sec: %.2fk  ", 13, current->alpha,
					current->word_count_actual / (real)(current->iter * current->train_words + 1) * 100,
					current->word_count_actual / ((real)(now - current->start + 1) / (real)CLOCKS_PER_SEC * 1000));
				fflush(stdout);
			}
			current->alpha = current->starting_alpha * (1 - current->word_count_actual / (real)(current->iter * current->train_words + 1));
			if (current->alpha < current->starting_alpha * 0.0001) current->alpha = current->starting_alpha * 0.0001;
		}
		if (sentence_length == 0) {
			string aword;
			char bword[W2VEC_MAX_STRING + 1];
			while (1) {
				aword = inputwords->getstring(idx++);
				if (aword.size() >= W2VEC_MAX_STRING)
					aword = aword.substr(0, W2VEC_MAX_STRING - 1);
				memcpy(bword, STR(aword), aword.size());
				bword[aword.size()] = 0;
				word = current->SearchVocab(bword);

				if (idx >= inputwords->Size()) break;

				if (word == -1) continue;
				word_count++;
				if (word == 0) break;
				// The subsampling randomly discards frequent words while keeping the ranking same
				if (current->sample > 0) {
					real ran = (sqrt(current->vocab[word].cn / (current->sample * current->train_words)) + 1) * (current->sample * current->train_words) / current->vocab[word].cn;
					next_random = next_random * learningconstant + 11;
					if (ran < (next_random & 0xFFFF) / (real)65536) continue;
				}
				sen[sentence_length] = word;
				sentence_length++;
				if (sentence_length >= W2VEC_MAX_SENTENCE_LENGTH) break;
			}
			sentence_position = 0;
		}
		if (idx >= inputwords->Size() || (word_count > current->train_words / current->num_threads)) {
			current->word_count_actual += word_count - last_word_count;
			local_iter--;
			if (local_iter == 0) break;
			word_count = 0;
			last_word_count = 0;
			sentence_length = 0;
			idx = baseidx;
			continue;
		}
		word = sen[sentence_position];
		if (word == -1) continue;
		for (c = 0; c < current->layer1_size; c++) neu1[c] = 0;
		for (c = 0; c < current->layer1_size; c++) neu1e[c] = 0;
		next_random = next_random * learningconstant + 11;
		b = next_random % current->window;
		if (current->cbow) {  //train the cbow architecture
			// in -> hidden
			cw = 0;
			for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
				c = sentence_position - current->window + a;
				if (c < 0) continue;
				if (c >= sentence_length) continue;
				last_word = sen[c];
				if (last_word == -1) continue;
				for (c = 0; c < current->layer1_size; c++) neu1[c] += current->syn0[c + last_word * current->layer1_size];
				cw++;
			}
			if (cw) {
				for (c = 0; c < current->layer1_size; c++) neu1[c] /= cw;
				if (current->hs) for (d = 0; d < current->vocab[word].codelen; d++) {
					f = 0;
					l2 = current->vocab[word].point[d] * current->layer1_size;
					// Propagate hidden -> output
					for (c = 0; c < current->layer1_size; c++) f += neu1[c] * current->syn1[c + l2];
					if (f <= -W2VEC_MAX_EXP) continue;
					else if (f >= W2VEC_MAX_EXP) continue;
					else f = current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))];
					// 'g' is the gradient multiplied by the learning rate
					g = (1 - current->vocab[word].code[d] - f) * current->alpha;
					// Propagate errors output -> hidden
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1[c + l2];
					// Learn weights hidden -> output
					for (c = 0; c < current->layer1_size; c++) current->syn1[c + l2] += g * neu1[c];
				}
				// NEGATIVE SAMPLING
				if (current->negative > 0) for (d = 0; d < current->negative + 1; d++) {
					if (d == 0) {
						target = word;
						label = 1;
					}
					else {
						next_random = next_random * learningconstant + 11;
						target = current->table[(next_random >> 16) % W2VEC_table_size];
						if (target == 0) target = next_random % (current->vocab_size - 1) + 1;
						if (target == word) continue;
						label = 0;
					}
					l2 = target * current->layer1_size;
					f = 0;
					for (c = 0; c < current->layer1_size; c++) f += neu1[c] * current->syn1neg[c + l2];
					if (f > W2VEC_MAX_EXP) g = (label - 1) * current->alpha;
					else if (f < -W2VEC_MAX_EXP) g = (label - 0) * current->alpha;
					else g = (label - current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))]) * current->alpha;
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1neg[c + l2];
					for (c = 0; c < current->layer1_size; c++) current->syn1neg[c + l2] += g * neu1[c];
				}
				// hidden -> in
				for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
					c = sentence_position - current->window + a;
					if (c < 0) continue;
					if (c >= sentence_length) continue;
					last_word = sen[c];
					if (last_word == -1) continue;
					for (c = 0; c < current->layer1_size; c++) current->syn0[c + last_word * current->layer1_size] += neu1e[c];
				}
			}
		}
		else {  //train skip-gram
			for (a = b; a < current->window * 2 + 1 - b; a++) if (a != current->window) {
				c = sentence_position - current->window + a;
				if (c < 0) continue;
				if (c >= sentence_length) continue;
				last_word = sen[c];
				if (last_word == -1) continue;
				l1 = last_word * current->layer1_size;
				for (c = 0; c < current->layer1_size; c++) neu1e[c] = 0;
				// HIERARCHICAL SOFTMAX
				if (current->hs) for (d = 0; d < current->vocab[word].codelen; d++) {
					f = 0;
					l2 = current->vocab[word].point[d] * current->layer1_size;
					// Propagate hidden -> output
					for (c = 0; c < current->layer1_size; c++) f += current->syn0[c + l1] * current->syn1[c + l2];
					if (f <= -W2VEC_MAX_EXP) continue;
					else if (f >= W2VEC_MAX_EXP) continue;
					else f = current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))];
					// 'g' is the gradient multiplied by the learning rate
					g = (1 - current->vocab[word].code[d] - f) * current->alpha;
					// Propagate errors output -> hidden
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1[c + l2];
					// Learn weights hidden -> output
					for (c = 0; c < current->layer1_size; c++) current->syn1[c + l2] += g * current->syn0[c + l1];
				}
				// NEGATIVE SAMPLING
				if (current->negative > 0) for (d = 0; d < current->negative + 1; d++) {
					if (d == 0) {
						target = word;
						label = 1;
					}
					else {
						next_random = next_random * learningconstant + 11;
						target = current->table[(next_random >> 16) % W2VEC_table_size];
						if (target == 0) target = next_random % (current->vocab_size - 1) + 1;
						if (target == word) continue;
						label = 0;
					}
					l2 = target * current->layer1_size;
					f = 0;
					for (c = 0; c < current->layer1_size; c++) f += current->syn0[c + l1] * current->syn1neg[c + l2];
					if (f > W2VEC_MAX_EXP) g = (label - 1) * current->alpha;
					else if (f < -W2VEC_MAX_EXP) g = (label - 0) * current->alpha;
					else g = (label - current->expTable[(int)((f + W2VEC_MAX_EXP) * (W2VEC_EXP_W2VEC_table_size / W2VEC_MAX_EXP / 2))]) * current->alpha;
					for (c = 0; c < current->layer1_size; c++) neu1e[c] += g * current->syn1neg[c + l2];
					for (c = 0; c < current->layer1_size; c++) current->syn1neg[c + l2] += g * current->syn0[c + l1];
				}
				// Learn weights input -> hidden
				for (c = 0; c < current->layer1_size; c++) current->syn0[c + l1] += neu1e[c];
			}
		}
		sentence_position++;
		if (sentence_position >= sentence_length) {
			sentence_length = 0;
			continue;
		}
	}
	free(neu1);
	free(neu1e);
#ifdef WIN32
	ThreadLock _lock(kifInstructionWHILE);
	tobejoined[(int)id] = false;
	return(0);
#else
	pthread_exit(NULL);
#endif
}





class Kifkifword2vec : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, kifword2vecMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;

	kifword2vec* w2vec;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	//---------------------------------------------------------------------------------------------------------------------
	Kifkifword2vec(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, kifword2vec_type) {
		//Do not forget your variable initialisation
		w2vec = new kifword2vec(kifcode);
	}

	~Kifkifword2vec() {
		delete w2vec;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return kifword2vec_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc);

	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifkifword2vec* kperso = new Kifkifword2vec(KifCurrent(), NULL);
		//Your initialisations here
		return kperso;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	bool initialconstructor() {
		return false;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;

		//Your first parameter is at position 0 etc...
		KifElement* kfirst = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc); //would match i as in the example above: mynewtype toto(i,j);
		KifElement* ksecond = params->Instruction(1)->Exec(kifNULL, dom, kifNULL, idthread, callfunc); //would match j as in the example above: mynewtype toto(i,j);
		//etc...
		return this;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitializationWord2Vec below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->Push(kifcode->Providestringraw(KifIdString(it->first)));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestringraw(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.	
	KifElement* MethodInit(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* kmap = callfunc->Evaluate(0, domain);
		return w2vec->init(kmap);
	}

	KifElement* MethodTrainModel(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (callfunc->Size() == 0) {
			if (w2vec->TrainModel() == -1)
				return kifFALSE;
			return kifTRUE;
		}

		KifElement* kvect = callfunc->Evaluate(0, domain);
		if (kvect->aVectorContainer() == false)
			return kifcode->Returnerror("W2V(029): Expecting a vector as input");

		w2vec->TrainModelFromVector(kvect);
		return kifTRUE;
	}

	KifElement* MethodLoadModel(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		bool normalize = false;
		if (callfunc->Size() == 2)
			normalize = callfunc->Evaluate(1, domain)->Boolean();

		if (w2vec->loadmodel(callfunc->Evaluate(0, domain)->String(), normalize) == -1)
			return kifFALSE;
		return kifTRUE;
	}

	KifElement* MethodAnalogy(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* kvect = callfunc->Evaluate(0, domain);
		if (kvect->aVectorContainer() == false)
			return kifcode->Returnerror("W2V(021): Need a vector as input to analogy");

		KifMapStringFloat* resmap;
		if (contextualpattern->type == kifMapStringFloat) {
			resmap = (KifMapStringFloat*)contextualpattern;
			resmap->Clear();
		}
		else
			resmap = new KifMapStringFloat(kifcode, NULL);

		if (w2vec->analogy(kvect, resmap) == -1)
			return kifNULL;

		return resmap;
	}

	KifElement* MethodAccuracy(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* kvect = callfunc->Evaluate(0, domain);
		if (kvect->aVectorContainer() == false)
			return kifcode->Returnerror("W2V(022): Need a vector as input to accuracy");

		KifMapStringFloat* resmap;
		if (contextualpattern->type == kifMapStringFloat) {
			resmap = (KifMapStringFloat*)contextualpattern;
			resmap->Clear();
		}
		else
			resmap = new KifMapStringFloat(kifcode, NULL);

		long long threshold = 30000;
		if (callfunc->Size() == 2)
			threshold = callfunc->Evaluate(1, domain)->Integer();

		if (w2vec->accuracy(threshold, kvect, resmap) == -1)
			return kifNULL;

		return resmap;
	}

	KifElement* MethodFeatures(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifMap* kmap;
		if (contextualpattern->type == kifMap) {
			kmap = (KifMap*)contextualpattern;
			kmap->Clear();
		}
		else
			kmap = new KifMap(kifcode, NULL);

		size_t a, b;
		string key;
		long long wrds = w2vec->a_words;
		long long sz = w2vec->a_size;
		KifVectorFloat* kvect;

		for (b = 0; b < wrds; b++) {
			key = &w2vec->a_vocab[b * max_w];
			kvect = new KifVectorFloat(kifcode, NULL);
			kmap->Push(key, kvect);
			kvect->values.reserve(sz);
			for (a = 0; a < sz; a++)
				kvect->values.push_back(w2vec->M[a + b * sz]);
		}

		return kmap;
	}

	KifElement* MethodVocabulary(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		return &w2vec->d_vocab;
	}

	KifElement* MethodDistance(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* kvect = callfunc->Evaluate(0, domain);


		KifMapStringFloat* resmap;
		if (contextualpattern->type == kifMapStringFloat) {
			resmap = (KifMapStringFloat*)contextualpattern;
			resmap->Clear();
		}
		else
			resmap = new KifMapStringFloat(kifcode, NULL);

		if (kvect->aVectorContainer() == false) {
			if (w2vec->distance(kvect->String(), resmap) == -1) {
				resmap->Release();
				return kifNULL;
			}
			return resmap;
		}

		if (w2vec->distance(kvect, resmap) == -1) {
			resmap->Release();
			return kifNULL;
		}

		return resmap;
	}


	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)

			return (this->*linkedmethods[name])(contextualpattern, domain, callfunc, idthread);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return ""; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

	//You can overcome the following function behaviour
	KifElement* in(KifElement* a, KifElement* b, KifDomain* kfunc, KifElement* contextualpattern, bool idx) {
		string as = a->String();
		if (w2vec->d_vocab.values.find(as) == w2vec->d_vocab.values.end())
			return kifFALSE;
		return kifTRUE;
	}
};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifkifword2vec::kifexportedmethods;
map<short, kifword2vecMethod>  Kifkifword2vec::linkedmethods;
map<string, string> Kifkifword2vec::infomethods;
hmap<short, bool> Kifkifword2vec::methodassigned;


class Kifword2vecitem : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, word2vecitemMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	string value;
	Kifkifword2vec* reference;
	double threshold;
	long long wordindex;
	float* vect;
	float* vectnorm;
	float norm;

	//---------------------------------------------------------------------------------------------------------------------
	Kifword2vecitem(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, word2vecitem_type) {
		//Do not forget your variable initialisation
		reference = NULL;
		vect = NULL;
		vectnorm = NULL;
		threshold = 0.5;
		wordindex = -1;
		norm = 0;
	}


	~Kifword2vecitem() {
		if (vect != NULL)
			delete[] vect;
		if (vectnorm != NULL)
			delete[] vectnorm;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* e, KifElement* kindex, KifElement* domain) {
		if (e->type != word2vecitem_type) {
			kifcode->Returnerror("W2V(109) : Cannot initialize with this value");
			return true;
		}

		Kifword2vecitem* item = (Kifword2vecitem*)e;
		if (item->vect != NULL) {
			value = item->value;
			reference = item->reference;
			threshold = item->threshold;
			wordindex = item->wordindex;
			norm = item->norm;
			long sz = item->reference->w2vec->a_size;
			vect = new float[max_size];
			vectnorm = new float[max_size];
			for (long long a = 0; a < reference->w2vec->a_size; a++) {
				vect[a] = item->vect[a];
				vectnorm[a] = item->vectnorm[a];
			}
		}

		return true;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return word2vecitem_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Atom(bool forced = false) {
		Kifword2vecitem* item = new Kifword2vecitem(kifcode, NULL);
		if (vect != NULL) {
			item->value = value;
			item->reference = reference;
			item->threshold = threshold;
			item->wordindex = wordindex;
			item->norm = norm;
			long sz = reference->w2vec->a_size;
			item->vect = new float[max_size];
			item->vectnorm = new float[max_size];
			for (long long a = 0; a < reference->w2vec->a_size; a++) {
				item->vect[a] = vect[a];
				item->vectnorm[a] = vectnorm[a];
			}
		}
		return item;
	}

	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		Kifword2vecitem* item = new Kifword2vecitem(kifcode, NULL);
		if (vect != NULL) {
			item->value = value;
			item->reference = reference;
			item->threshold = threshold;
			item->wordindex = wordindex;
			item->norm = norm;
			long sz = reference->w2vec->a_size;
			item->vect = new float[max_size];
			item->vectnorm = new float[max_size];
			for (long long a = 0; a < reference->w2vec->a_size; a++) {
				item->vect[a] = vect[a];
				item->vectnorm[a] = vectnorm[a];
			}
		}
		return item;
	}

	//We create our iterator element. This method is called when an iterator is initialised with our local object
	//This description is facultative
	//iterator it=mydb;
	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	bool initialconstructor() {
		return false;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* dom, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;

		//Your first parameter is at position 0 etc...
		KifElement* e = params->Instruction(0)->Exec(kifNULL, dom, kifNULL, idthread, callfunc); //would match i as in the example above: mynewtype toto(i,j);
		if (e->type != word2vecitem_type)
			return kifcode->Returnerror("W2V(109) : Cannot initialize with this value");

		Kifword2vecitem* item = (Kifword2vecitem*)e;
		value = item->value;
		reference = item->reference;
		threshold = item->threshold;
		wordindex = item->wordindex;
		norm = item->norm;

		if (e->reference == 0) {
			//It has been created just now...			
			vect = item->vect;
			vectnorm = item->vectnorm;
			item->vect = NULL;
			item->vectnorm = NULL;
			return e;
		}

		if (item->vect != NULL) {
			vect = new float[max_size];
			vectnorm = new float[max_size];
			for (long long a = 0; a < reference->w2vec->a_size; a++) {
				vect[a] = item->vect[a];
				vectnorm[a] = item->vectnorm[a];
			}
		}
		return e;
	}


	bool initialization(Kifkifword2vec* ref) {
		if (vect == NULL) {
			reference = ref;
			vect = new float[max_size];
			vectnorm = new float[max_size];
			for (long long a = 0; a < reference->w2vec->a_size; a++) {
				vect[a] = 0;
				vectnorm[a] = 0;
			}
			return true;
		}
		if (reference != ref)
			return false;
		return true;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->Push(kifcode->Providestringraw(KifIdString(it->first)));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestringraw(infomethods[s]);
	}
	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodCosine(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (vect == NULL || norm == 0)
			return kifZERO;

		KifElement* kfirst = callfunc->Evaluate(0, domain);
		float lastdistance = 0;
		if (kfirst->type == word2vecitem_type) {
			Kifword2vecitem* kwrd = (Kifword2vecitem*)kfirst;
			if (kwrd->vect == NULL || kwrd->norm == 0)
				return kifZERO;
			lastdistance = reference->w2vec->dot(vect, kwrd->vect);
			lastdistance /= norm*kwrd->norm;
			return kifcode->Providefloat(lastdistance);
		}

		float wnorm = 0;
		if (kfirst->type == kifString) {
			string wrd = kfirst->String();
			lastdistance = reference->w2vec->dot(vect, wrd, wnorm);
			if (wnorm == 0)
				return kifZERO;

			lastdistance /= norm*wnorm;
			return kifcode->Providefloat(lastdistance);
		}

		if (kfirst->type == kifVectorInteger) {
			lastdistance = reference->w2vec->dot(vect, ((KifVectorInteger*)kfirst)->values, wnorm);
			lastdistance /= norm*wnorm;
			return kifcode->Providefloat(lastdistance);
		}

		if (kfirst->type == kifVectorFloat) {
			lastdistance = reference->w2vec->dot(vect, ((KifVectorFloat*)kfirst)->values, wnorm);
			if (wnorm == 0)
				return kifZERO;
			lastdistance /= norm*wnorm;
			return kifcode->Providefloat(lastdistance);
		}

		if (kfirst->type == kifVector) {
			lastdistance = reference->w2vec->dot(vect, ((KifVector*)kfirst)->values, wnorm);
			if (wnorm == 0)
				return kifZERO;
			lastdistance /= norm*wnorm;
			return kifcode->Providefloat(lastdistance);
		}

		return kifZERO;
	}

	KifElement* MethodDistance(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		if (vect == NULL || norm == 0)
			return kifZERO;

		float lastdistance = 0;

		KifElement* kfirst = callfunc->Evaluate(0, domain);
		if (kfirst->type == word2vecitem_type) {
			Kifword2vecitem* kwrd = (Kifword2vecitem*)kfirst;
			if (kwrd->wordindex != -1 && wordindex != -1) {
				lastdistance = reference->w2vec->distance(vectnorm, kwrd->wordindex);
				return kifcode->Providefloat(lastdistance);
			}

			if (kwrd->vect == NULL || kwrd->norm == 0)
				return kifZERO;

			lastdistance = reference->w2vec->dot(vect, kwrd->vect);
			lastdistance /= norm*kwrd->norm;
			return kifcode->Providefloat(lastdistance);
		}

		if (kfirst->type == kifString) {
			string wrd = kfirst->String();
			long long idx = reference->w2vec->wordindex(wrd);
			if (idx == -1)
				return kifZERO;

			if (wordindex != -1) {
				lastdistance = reference->w2vec->distance(vectnorm, idx);
				return kifcode->Providefloat(lastdistance);
			}

			float wnorm = 0;
			lastdistance = reference->w2vec->dot(vect, wrd, wnorm);
			if (wnorm == 0)
				return kifZERO;

			lastdistance /= norm*wnorm;
			return kifcode->Providefloat(lastdistance);
		}

		return kifcode->Returnerror("W2V(111): Unknown item");
	}

	KifElement* MethodDot(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		KifElement* kfirst = callfunc->Evaluate(0, domain);
		float d;
		if (kfirst->type == word2vecitem_type) {
			Kifword2vecitem* kwrd = (Kifword2vecitem*)kfirst;
			d = reference->w2vec->dot(vect, kwrd->vect);
			return kifcode->Providefloat(d);
		}

		float wnorm = 0;
		if (kfirst->type == kifString) {
			string wrd = kfirst->String();
			d = reference->w2vec->dot(vectnorm, wrd, wnorm);
			return kifcode->Providefloat(d);
		}

		if (kfirst->type == kifVectorInteger) {
			d = reference->w2vec->dot(vect, ((KifVectorInteger*)kfirst)->values, wnorm);
			return kifcode->Providefloat(d);
		}

		if (kfirst->type == kifVectorFloat) {
			d = reference->w2vec->dot(vect, ((KifVectorFloat*)kfirst)->values, wnorm);
			return kifcode->Providefloat(d);
		}

		if (kfirst->type == kifVector) {
			d = reference->w2vec->dot(vect, ((KifVector*)kfirst)->values, wnorm);
			return kifcode->Providefloat(d);
		}

		return kifcode->Returnerror("W2V(111): Unknown item");
	}


	KifElement* MethodNorm(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		return kifcode->Providefloat(norm);
	}

	KifElement* MethodVectorNorm(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* kvect = Selectvectorfloat(callfunc, contextualpattern);
		if (vect == NULL)
			return kvect;

		kvect->Reserve(reference->w2vec->a_size);
		for (long long a = 0; a < reference->w2vec->a_size; a++)
			kvect->storevalue(vectnorm[a]);

		return kvect;
	}


	KifElement* MethodThreshold(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (callfunc->Size() == 0)
			return kifcode->Providefloat(threshold);
		threshold = callfunc->Evaluate(0, domain)->Float();
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)
			return (this->*linkedmethods[name])(contextualpattern, domain, callfunc, idthread);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//For Predicate use only...
	bool unifiedDeclaration(KifDomain* dom) {
		return false;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the "size" method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() {
		stringstream v;
		v << value << "_" << wordindex;
		return v.str();
	}
	long Integer() { return norm; }
	double Float() { return norm; }
	bool Boolean() { return true; }


	KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (b->type != word2vecitem_type || a->type != word2vecitem_type)
			return kifNULL;

		Kifword2vecitem* item = (Kifword2vecitem*)b;

		long long i;
		if (autoself) {
			if (!initialization(item->reference))
				return kifNULL;

			norm = 0;
			for (i = 0; i < reference->w2vec->a_size; i++) {
				vect[i] += item->vect[i];
				norm += vect[i] * vect[i];
			}
			if (norm != 0) {
				norm = sqrt(norm);
				for (i = 0; i < reference->w2vec->a_size; i++)
					vectnorm[i] = vect[i] / norm;
			}
			wordindex = -1;
			value += "&";
			value += item->value;
			return this;
		}

		Kifword2vecitem* base = (Kifword2vecitem*)a;
		if (base->reference != item->reference)
			return kifNULL;

		Kifword2vecitem* res = new Kifword2vecitem(kifcode, NULL);
		res->initialization(item->reference);
		for (i = 0; i < item->reference->w2vec->a_size; i++) {
			res->vect[i] = base->vect[i] + item->vect[i];
			res->norm += res->vect[i] * res->vect[i];
		}
		if (res->norm != 0) {
			res->norm = sqrt(res->norm);
			for (i = 0; i < item->reference->w2vec->a_size; i++)
				res->vectnorm[i] = res->vect[i] / res->norm;
		}
		res->value = base->value;
		res->value += "&";
		res->value += item->value;
		return res;
	}

	KifElement* minus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		if (b->type != word2vecitem_type || a->type != word2vecitem_type)
			return kifNULL;

		Kifword2vecitem* item = (Kifword2vecitem*)b;

		long long i;
		if (autoself) {
			if (!initialization(item->reference))
				return kifNULL;

			norm = 0;
			for (i = 0; i < reference->w2vec->a_size; i++) {
				vect[i] -= item->vect[i];
				norm += vect[i] * vect[i];
			}
			if (norm != 0) {
				norm = sqrt(norm);
				for (i = 0; i < reference->w2vec->a_size; i++)
					vectnorm[i] = vect[i] / norm;
			}
			wordindex = -1;
			value += "&";
			value += item->value;
			return this;
		}

		Kifword2vecitem* base = (Kifword2vecitem*)a;
		if (base->reference != item->reference)
			return kifNULL;

		Kifword2vecitem* res = new Kifword2vecitem(kifcode, NULL);
		res->initialization(item->reference);
		for (i = 0; i < item->reference->w2vec->a_size; i++) {
			res->vect[i] = base->vect[i] - item->vect[i];
			res->norm += res->vect[i] * res->vect[i];
		}
		if (res->norm != 0) {
			res->norm = sqrt(res->norm);
			for (i = 0; i < item->reference->w2vec->a_size; i++)
				res->vectnorm[i] = res->vect[i] / res->norm;
		}
		res->value = base->value;
		res->value += "<-";
		res->value += item->value;
		return res;
	}

	//Here, we use the distance to check if two elements are the same...
	KifElement* same(KifElement* a) {
		if (a == this)
			return kifTRUE;

		if (a == kifNULL || a == kifNOELEMENT) {
			if (reference == NULL || reference->w2vec == NULL)
				return kifTRUE;
			return kifFALSE;
		}

		float lastdistance = 0;
		if (a->type == word2vecitem_type) {
			Kifword2vecitem* w = (Kifword2vecitem*)a;
			if (w->reference != reference)
				return kifFALSE;

			if (wordindex != -1 && w->wordindex == wordindex)
				return kifTRUE;

			if (w->wordindex != -1) {
				lastdistance = reference->w2vec->distance(vectnorm, w->wordindex);
				if (lastdistance >= threshold)
					return kifTRUE;
				return kifFALSE;
			}

			if (w->vect == NULL || w->norm == 0 || norm == 0)
				return kifFALSE;

			lastdistance = reference->w2vec->dot(vect, w->vect);
			lastdistance /= norm*w->norm;
			if (lastdistance >= threshold)
				return kifTRUE;
			return kifFALSE;
		}

		string wrd = a->String();
		long long idx = reference->w2vec->wordindex(wrd);
		if (idx == -1)
			return kifFALSE;
		if (wordindex == idx)
			return kifTRUE;

		lastdistance = reference->w2vec->distance(vectnorm, idx);
		if (lastdistance >= threshold)
			return kifTRUE;
		return kifFALSE;
	}

	bool unified(KifDomain* dom, KifElement* a, float& w) {
		if (a == this) {
			w += 1;
			return true;
		}

		if (a == kifNULL || a == kifNOELEMENT) {
			if (reference == NULL || reference->w2vec == NULL) {
				w += 1;
				return true;
			}
			return false;
		}

		float lastdistance = 0;
		if (a->type == word2vecitem_type) {
			Kifword2vecitem* wrd = (Kifword2vecitem*)a;
			if (wrd->reference != reference)
				return false;

			if (wordindex != -1 && wrd->wordindex == wordindex) {
				w += 1;
				return true;
			}

			if (wrd->wordindex != -1) {
				lastdistance = reference->w2vec->distance(vectnorm, wrd->wordindex);
				if (lastdistance >= threshold) {
					w += lastdistance;
					return true;
				}
				return false;
			}

			if (wrd->vect == NULL || wrd->norm == 0 || norm == 0)
				return false;

			lastdistance = reference->w2vec->dot(vect, wrd->vect);
			lastdistance /= norm*wrd->norm;
			if (lastdistance >= threshold) {
				w += lastdistance;
				return true;
			}
			return false;
		}

		string wrd = a->String();
		long long idx = reference->w2vec->wordindex(wrd);
		if (idx == -1)
			return false;
		if (wordindex == idx) {
			w += 1;
			return true;
		}

		lastdistance = reference->w2vec->distance(vectnorm, idx);
		if (lastdistance >= threshold) {
			w += lastdistance;
			return true;
		}
		return false;
	}


	KifElement* Vector(KifElement* contextualpattern, KifVector* v) {
		KifElement* kvect = Selectvectorfloat(this, contextualpattern);

		kvect->Reserve(reference->w2vec->a_size);
		for (long long a = 0; a < reference->w2vec->a_size; a++)
			kvect->storevalue(vect[a]);

		return kvect;
	}
};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifword2vecitem::kifexportedmethods;
map<short, word2vecitemMethod>  Kifword2vecitem::linkedmethods;
map<string, string> Kifword2vecitem::infomethods;
hmap<short, bool> Kifword2vecitem::methodassigned;



//---------------------------------------------------------------------------------------------------------------------
KifElement* Kifkifword2vec::Exec(KifElement* contextualpattern, KifDomain* dom, KifElement* idx, int idthread, KifCallFunction* callfunc) {
	if (w2vec == NULL)
		return kifcode->Returnerror("W2V(110): Empty model");

	if (idx != NULL && !idx->isConst()) {
		string skey;
		KifElement* key;
		KifElement* kthreshold = NULL;
		double threshold = 0;
		if (idx->type == kifIndex) {
			KifIndex* kind = (KifIndex*)idx;
			key = kind->key->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
			if (kind->interval == true) {
				kthreshold = kind->right->Exec(kifNULL, dom, kifNULL, idthread, callfunc);
				threshold = kthreshold->Float();
				kthreshold->Release();
			}
		}
		else
			key = idx->Exec(contextualpattern, dom, kifNULL, idthread, callfunc);

		skey = key->String();
		key->Release();
		Kifword2vecitem* item = new Kifword2vecitem(kifcode, NULL);
		if (w2vec->d_vocab.values.find(skey) == w2vec->d_vocab.values.end()) {
			item->initialization(this);
			item->value = skey;
			return item;
		}

		item->reference = this;
		item->value = skey;
		item->threshold = threshold;
		item->wordindex = w2vec->d_vocab.values[skey];
		item->vect = w2vec->avector(item->wordindex);
		item->vectnorm = w2vec->normalize(item->vect, item->norm);
		return item;
	}

	return this;
}
//---------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* Createkifkifword2vecElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifkifword2vec* local = new Kifkifword2vec(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}


static KifElement* Createkifword2vecitemElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifword2vecitem* local = new Kifword2vecitem(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}


//MethodInitializationWord2Vec will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitializationWord2Vec(string name, kifword2vecMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifkifword2vec::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifkifword2vec::linkedmethods[idname] = func;
	Kifkifword2vec::methodassigned[idname] = true;
	Kifkifword2vec::infomethods[name] = infos;
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitializationWord2VecItem(string name, word2vecitemMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifword2vecitem::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifword2vecitem::linkedmethods[idname] = func;
	Kifword2vecitem::methodassigned[idname] = true;
	Kifword2vecitem::infomethods[name] = infos;
}


//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifword2vec(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}
		kifcode->KifInitLib();

		///The new type will be: kifword2vec. You will be able to declare any variable with that type, once this library has been loaded...
		//Createkifkifword2vecElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifkifword2vec)
		//Whenever the keyword testage will be found by KiF it will then call Createkifkifword2vecElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("word2vec", Createkifkifword2vecElement);
		if (res == kifVOID)
			return false;

		kifword2vec_type = res;
		
		res = KifAddNewType("w2vector", Createkifword2vecitemElement);
		if (res == kifVOID)
			return false;

		word2vecitem_type = res;
		return true;
	}

	Exporting bool kifword2vec_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifword2vec(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitializationWord2Vec, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitializationWord2Vec("initialization", &Kifkifword2vec::MethodInit, P_ONE, "initialization(map m): Initialization of a word2vec training set");
		MethodInitializationWord2Vec("trainmodel", &Kifkifword2vec::MethodTrainModel, P_NONE | P_ONE, "trainmodel(vector v): Launching the training. If v is not supplied, then the system utilizes the input file given in the initialisation options");
		MethodInitializationWord2Vec("loadmodel", &Kifkifword2vec::MethodLoadModel, P_ONE | P_TWO, "loadmodel(string filename,bool normalize): Loading a model");
		MethodInitializationWord2Vec("analogy", &Kifkifword2vec::MethodAnalogy, P_ONE, "analogy(svector words): Finding analogies for a group of words. Return a fmap");
		MethodInitializationWord2Vec("accuracy", &Kifkifword2vec::MethodAccuracy, P_ONE | P_TWO, "accuracy(vector words,int threshold): Finding accuracies for a vector of many times 4 words. Return a fmap. If threshold is not supplied then its value is 30000");
		MethodInitializationWord2Vec("distance", &Kifkifword2vec::MethodDistance, P_ONE, "distance(svector words): Finding the distance in a vector of words. Return a fmap.");
		MethodInitializationWord2Vec("vocabulary", &Kifkifword2vec::MethodVocabulary, P_NONE, "vocabulary(): Return a itreemap of the vocabulary covered by the training.");
		MethodInitializationWord2Vec("features", &Kifkifword2vec::MethodFeatures, P_NONE, "features(): Return a map of the vocabulary with their feature values.");

		MethodInitializationWord2VecItem("dot", &Kifword2vecitem::MethodDot, P_ONE, "dot(element): Return the dot product between two words. Element is either a string or a w2vector");
		MethodInitializationWord2VecItem("cosine", &Kifword2vecitem::MethodCosine, P_ONE, "cosine(element): Return the cosine distance between two words. Element is either a string or a w2vector.");
		MethodInitializationWord2VecItem("distance", &Kifword2vecitem::MethodDistance, P_ONE, "distance(element): Return the distance between two words. Element is either a string or a w2vector.");
		MethodInitializationWord2VecItem("threshold", &Kifword2vecitem::MethodThreshold, P_NONE | P_ONE, "threshold(element): Return or set the threshold.");
		MethodInitializationWord2VecItem("norm", &Kifword2vecitem::MethodNorm, P_NONE, "norm(element): Return the vector norm.");

		kifGlobals->kifAllFunctions[word2vecitem_type] = Kifword2vecitem::methodassigned;
		kifGlobals->kifAllInformation[word2vecitem_type] = Kifword2vecitem::infomethods;
		kifGlobals->kifAllFunctions[kifword2vec_type] = Kifkifword2vec::methodassigned;
		kifGlobals->kifAllInformation[kifword2vec_type] = Kifkifword2vec::infomethods;

		//If you want to create specific variables in the main frame		
		KifVariableDeclaration* var = kifcode->Createvariable("myvariable");
		//then we create its value
		Kifkifword2vec* local = new Kifkifword2vec(kifcode, var);
		return true;
	}

	Exporting bool kifword2vec_Terminate(KifCode* kifcode) {
		Kifkifword2vec::kifexportedmethods.clear();
		Kifkifword2vec::linkedmethods.clear();
		Kifkifword2vec::methodassigned.clear();
		Kifkifword2vec::infomethods.clear();
		Kifword2vecitem::kifexportedmethods.clear();
		Kifword2vecitem::linkedmethods.clear();
		Kifword2vecitem::infomethods.clear();
		Kifword2vecitem::methodassigned.clear();
		return true;
	}
}




