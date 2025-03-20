#ifndef FUNC_DISAMB
#define FUNC_DISAMB

extern "C" {

int InitDisambText (char *nameHMM);
void CloseHMM();


void initialize_glob_variables ();
void update_glob_variables ();
void viterbi_next_word_from_string (char* string, int next_p);
int put_word (char* word,short type_word);
void end_put_word();
void this_is_the_end();
void free_word_struct ();
void alloc_word_struct ();
}

#endif

