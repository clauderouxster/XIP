/*
* Xerox Research Centre Europe - Grenoble Laboratory
*
* Copyright (C) 2003 - 2010 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
* This file can only be used with the KiF library,
* it should not and cannot be used otherwise.
*/
/* --- CONTENTS ---
Project    : KiF
Version    : 1.00
filename   : kiflibsvm.cxx
Date       : 09/09/2010
Purpose    : Implementation of the KiF language: PATTERN FILE
Programmer : Claude ROUX
Reviewer   :
*/

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include "svm.h"

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"
#include "kifmaps.h"
#include "kiftreemaps.h"

#include <float.h>

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

#ifndef max
#define max(x,y) (((x)>(y))?(x):(y))
#define min(x,y) (((x)<(y))?(x):(y))
#endif

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#define INF HUGE_VAL

short KifNewId(string n);
string KifIdString(short n);

//Replace Libsvm by your own definition
//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType KifLibsvm_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class KifLibsvm;
//This typedef defines a type "LibsvmMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (KifLibsvm::*LibsvmMethod)(KifElement*, KifDomain*, KifCallFunction*);

void print_null(const char *s) {}

class KifLibsvm : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, LibsvmMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	struct svm_parameter param;		// set by parse_command_line
	struct svm_problem prob;		// set by read_problem
	struct svm_model *model;
	struct svm_node *x_space;
	int cross_validation;
	int nr_fold;
	int max_line_len;

	string input_file_name;
	string model_file_name;

	struct svm_node *x;
	int max_nr_attr;

	int predict_probability;
	bool infos, loaded;

	double lower, upper, y_lower, y_upper;
	int y_scaling;
	double *feature_max;
	double *feature_min;
	double y_max;
	double y_min;
	int max_index;
	int min_index;
	long int num_nonzeros;
	long int new_num_nonzeros;
	char *line;


	//---------------------------------------------------------------------------------------------------------------------
	void RAZ() {
		max_line_len = 1024;
		line = (char *)malloc(max_line_len*sizeof(char));
		loaded = false;
		lower = -1.0;
		upper = 1.0;
		y_scaling = 0;
		feature_max = NULL;
		feature_min = NULL;
		y_max = -DBL_MAX;
		y_min = DBL_MAX;
		num_nonzeros = 0;
		new_num_nonzeros = 0;

		infos = false;
		input_file_name = "";
		model_file_name = "";
		model = NULL;
		x_space = NULL;
		void(*print_func)(const char*) = NULL;	// default printing to stdout

		// default values
		max_nr_attr = 64;
		predict_probability = 0;

		param.svm_type = C_SVC;
		param.kernel_type = RBF;
		param.degree = 3;
		param.gamma = 0;	// 1/num_features
		param.coef0 = 0;
		param.nu = 0.5;
		param.cache_size = 100;
		param.C = 1;
		param.eps = 1e-3;
		param.p = 0.1;
		param.shrinking = 1;
		param.probability = 0;
		param.nr_weight = 0;
		param.weight_label = NULL;
		param.weight = NULL;
		cross_validation = 0;
	}

	void CLEAN() {
		if (model != NULL) {
			svm_free_and_destroy_model(&model);
			svm_destroy_param(&param);
			free(prob.y);
			free(prob.x);
			free(x_space);
		}
	}

	KifLibsvm(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, KifLibsvm_type) {
		RAZ();
	}

	~KifLibsvm() {
		CLEAN();
		free(line);
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return KifLibsvm_type;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}


	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifLibsvm* kperso = new KifLibsvm(kifcode, kp);
		return kperso;
	}

	//---------------------------------------------------------------------------------------------------------------------

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
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestring(infomethods[s]);
	}

	//---------------------------------------------------------------------------------------------------------------------
	bool initialconstructor() {
		return true;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* domain, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;

		//Your first parameter is at position 0 etc...		
		KifElement* kmfn = params->Instruction(0)->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
		model_file_name = kmfn->String();
		kmfn->Release();

		if ((model = svm_load_model(STR(model_file_name))) == 0)
			return kifcode->Returnerror("SVM(012): can't open model file");

		loaded = true;
		return this;
	}

	//----------------------------------------SCALING------------------------------------------------------------------
	void output_target(double value)
	{
		if (y_scaling)
		{
			if (value == y_min)
				value = y_lower;
			else if (value == y_max)
				value = y_upper;
			else value = y_lower + (y_upper - y_lower) *
				(value - y_min) / (y_max - y_min);
		}
		printf("%g ", value);
	}

	void output(int index, double value)
	{
		/* skip single-valued attribute */
		if (feature_max[index] == feature_min[index])
			return;

		if (value == feature_min[index])
			value = lower;
		else if (value == feature_max[index])
			value = upper;
		else
			value = lower + (upper - lower) *
			(value - feature_min[index]) /
			(feature_max[index] - feature_min[index]);

		if (value != 0)
		{
			printf("%d:%g ", index, value);
			new_num_nonzeros++;
		}
	}

	KifElement* clean_up(FILE *fp_restore, FILE *fp, const char* msg)
	{
		fprintf(stderr, "%s", msg);
		free(feature_max);
		free(feature_min);
		fclose(fp);
		if (fp_restore)
			fclose(fp_restore);
		return kifFALSE;
	}

	KifElement* MethodScale(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		int i, index;
		FILE *fp, *fp_restore = NULL;

		string message;
		string input_file = callfunc->Evaluate(0, domain)->String();
		KifElement* koptions = callfunc->Evaluate(1, domain);
		if (!koptions->aMapContainer())
			return kifcode->Returnerror("SVM(013): Expecting a map as second parameter");

		string save_filename;
		string restore_filename;

		KifIteration* itr = (KifIteration*)koptions->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			string key = itr->IteratorKeyString();
			string val = itr->IteratorValueString();
			switch (key[0]) {
			case 'l': lower = atof(STR(val)); break;
			case 'u': upper = atof(STR(val)); break;
			case 'y':
				y_lower = atof(STR(val));
				break;
			case 'Y':
				y_upper = atof(STR(val));
				y_scaling = 1;
				break;
			case 'r':
				restore_filename = val;
				break;
			case 's':
				save_filename = val;
				break;
			default:
				return kifcode->Returnerror("SVM(009): Unknown option");
			}
		}
		itr->Release();

		if (!(upper > lower) || (y_scaling && !(y_upper > y_lower)))
		{
			return kifcode->Returnerror("SVM(011): inconsistent lower/upper specification");
		}

		if (restore_filename != "" && save_filename != "")
		{
			return kifcode->Returnerror("SVM(012): cannot use -r and -s simultaneously");
		}

		fp = fopen(STR(input_file), "r");

		if (fp == NULL)
			return kifcode->Returnerror("SVM(014): can't open input file");

		//line = (char *) malloc(max_line_len*sizeof(char));

#define SKIP_TARGET\
		while (isspace(*p)) ++p; \
		while (!isspace(*p)) ++p;

#define SKIP_ELEMENT\
		while (*p != ':') ++p; \
		++p; \
		while (isspace(*p)) ++p; \
		while (*p && !isspace(*p)) ++p;

		/* assumption: min index of attributes is 1 */
		/* pass 1: find out max index of attributes */
		max_index = 0;
		min_index = 1;

		if (restore_filename != "")
		{
			int idx, c;

			fp_restore = fopen(STR(restore_filename), "r");
			if (fp_restore == NULL)
			{
				return kifcode->Returnerror("SVM(015): can't open restore file");
			}

			c = fgetc(fp_restore);
			if (c == 'y')
			{
				readline(fp_restore);
				readline(fp_restore);
				readline(fp_restore);
			}
			readline(fp_restore);
			readline(fp_restore);

			while (fscanf(fp_restore, "%d %*f %*f\n", &idx) == 1)
				max_index = max(idx, max_index);
			rewind(fp_restore);
		}

		while (readline(fp) != NULL)
		{
			char *p = line;

			SKIP_TARGET

			while (sscanf(p, "%d:%*f", &index) == 1)
			{
				max_index = max(max_index, index);
				min_index = min(min_index, index);
				SKIP_ELEMENT
					num_nonzeros++;
			}
		}

		if (min_index < 1)
			fprintf(stderr,
			"WARNING: minimal feature index is %d, but indices should start from 1\n", min_index);

		rewind(fp);

		feature_max = (double *)malloc((max_index + 1)* sizeof(double));
		feature_min = (double *)malloc((max_index + 1)* sizeof(double));

		if (feature_max == NULL || feature_min == NULL)
		{
			fprintf(stderr, "can't allocate enough memory\n");
			exit(1);
		}

		for (i = 0; i <= max_index; i++)
		{
			feature_max[i] = -DBL_MAX;
			feature_min[i] = DBL_MAX;
		}

		/* pass 2: find out min/max value */
		while (readline(fp) != NULL)
		{
			char *p = line;
			int next_index = 1;
			double target;
			double value;

			if (sscanf(p, "%lf", &target) != 1)
				return clean_up(fp_restore, fp, "ERROR: failed to read labels\n");
			y_max = max(y_max, target);
			y_min = min(y_min, target);

			SKIP_TARGET

			while (sscanf(p, "%d:%lf", &index, &value) == 2)
			{
				for (i = next_index; i < index; i++)
				{
					feature_max[i] = max(feature_max[i], 0);
					feature_min[i] = min(feature_min[i], 0);
				}

				feature_max[index] = max(feature_max[index], value);
				feature_min[index] = min(feature_min[index], value);

				SKIP_ELEMENT
					next_index = index + 1;
			}

			for (i = next_index; i <= max_index; i++)
			{
				feature_max[i] = max(feature_max[i], 0);
				feature_min[i] = min(feature_min[i], 0);
			}
		}

		rewind(fp);

		/* pass 2.5: save/restore feature_min/feature_max */

		if (restore_filename != "")
		{
			/* fp_restore rewinded in finding max_index */
			int idx, c;
			double fmin, fmax;
			int next_index = 1;

			if ((c = fgetc(fp_restore)) == 'y')
			{
				if (fscanf(fp_restore, "%lf %lf\n", &y_lower, &y_upper) != 2 ||
					fscanf(fp_restore, "%lf %lf\n", &y_min, &y_max) != 2)
					return clean_up(fp_restore, fp, "ERROR: failed to read scaling parameters\n");
				y_scaling = 1;
			}
			else
				ungetc(c, fp_restore);

			if (fgetc(fp_restore) == 'x')
			{
				if (fscanf(fp_restore, "%lf %lf\n", &lower, &upper) != 2)
					return clean_up(fp_restore, fp, "ERROR: failed to read scaling parameters\n");
				while (fscanf(fp_restore, "%d %lf %lf\n", &idx, &fmin, &fmax) == 3)
				{
					for (i = next_index; i < idx; i++)
					if (feature_min[i] != feature_max[i])
						fprintf(stderr,
						"WARNING: feature index %d appeared in file %s was not seen in the scaling factor file %s.\n",
						i, STR(input_file), STR(restore_filename));

					feature_min[idx] = fmin;
					feature_max[idx] = fmax;

					next_index = idx + 1;
				}

				for (i = next_index; i <= max_index; i++)
				if (feature_min[i] != feature_max[i])
					fprintf(stderr,
					"WARNING: feature index %d appeared in file %s was not seen in the scaling factor file %s.\n",
					i, STR(input_file), STR(restore_filename));
			}
			fclose(fp_restore);
		}

		if (save_filename != "")
		{
			FILE *fp_save = fopen(STR(save_filename), "w");
			if (fp_save == NULL)
			{
				return kifcode->Returnerror("SVM(016): can't open save file");
			}
			if (y_scaling)
			{
				fprintf(fp_save, "y\n");
				fprintf(fp_save, "%.16g %.16g\n", y_lower, y_upper);
				fprintf(fp_save, "%.16g %.16g\n", y_min, y_max);
			}
			fprintf(fp_save, "x\n");
			fprintf(fp_save, "%.16g %.16g\n", lower, upper);
			for (i = 1; i <= max_index; i++)
			{
				if (feature_min[i] != feature_max[i])
					fprintf(fp_save, "%d %.16g %.16g\n", i, feature_min[i], feature_max[i]);
			}

			if (min_index < 1)
				fprintf(stderr,
				"WARNING: scaling factors with indices smaller than 1 are not stored to the file %s.\n", STR(save_filename));

			fclose(fp_save);
		}

		/* pass 3: scale */
		while (readline(fp) != NULL)
		{
			char *p = line;
			int next_index = 1;
			double target;
			double value;

			if (sscanf(p, "%lf", &target) != 1)
				return clean_up(NULL, fp, "ERROR: failed to read labels\n");
			output_target(target);

			SKIP_TARGET

			while (sscanf(p, "%d:%lf", &index, &value) == 2)
			{
				for (i = next_index; i < index; i++)
					output(i, 0);

				output(index, value);

				SKIP_ELEMENT
					next_index = index + 1;
			}

			for (i = next_index; i <= max_index; i++)
				output(i, 0);

			printf("\n");
		}

		if (new_num_nonzeros > num_nonzeros)
			fprintf(stderr,
			"WARNING: original #nonzeros %ld\n"
			"         new      #nonzeros %ld\n"
			"Use -l 0 if many original feature values are zeros\n",
			num_nonzeros, new_num_nonzeros);

		free(feature_max);
		free(feature_min);
		fclose(fp);
		return kifTRUE;
	}

	//----------------------------------------PREDICTION------------------------------------------------------------------
	KifElement* MethodDoPredictFromFile(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		if (!loaded)
			return kifcode->Returnerror("LLN(018): Load your model first");

		int correct = 0;
		int total = 0;
		double error = 0;
		double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

		int svm_type = svm_get_svm_type(model);
		int nr_class = svm_get_nr_class(model);
		double *prob_estimates = NULL;
		int j;

		string inputfile = callfunc->Evaluate(0, domain)->String();
		FILE* input;
		input = fopen(STR(inputfile), "r");
		if (input == NULL)
			return kifcode->Returnerror("LLN(014): Can't open input file");

		predict_probability = callfunc->Evaluate(1, domain)->Integer();
		x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
		infos = callfunc->Evaluate(1, domain)->Boolean();

		KifVector* output = Selectvector(callfunc, contextualpattern);
		do_predict_with_file(input, output);
		fclose(input);
		free(x);
		x = NULL;
		return output;
	}

	KifElement* do_predict_with_file(FILE *input, KifVector* output) {
		KifTreeMap* kinfo = NULL;
		if (infos) {
			kinfo = new KifTreeMap(kifcode, NULL);
			kinfo->Setreference(output->reference + 1);
			output->values.push_back(kinfo);
		}

		int correct = 0;
		int total = 0;
		double error = 0;
		double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

		int svm_type = svm_get_svm_type(model);
		int nr_class = svm_get_nr_class(model);
		double *prob_estimates = NULL;
		int j;

		char buff[1000];

		if (predict_probability) {
			if (svm_type == NU_SVR || svm_type == EPSILON_SVR) {
				sprintf_s(buff, 10000, "Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma=%g\n", svm_get_svr_probability(model));
				output->Push(kifcode->Providestringraw(buff));
			}
			else
			{
				int *labels = (int *)malloc(nr_class*sizeof(int));
				svm_get_labels(model, labels);
				prob_estimates = (double *)malloc(nr_class*sizeof(double));

				if (infos) {
					KifVectorInteger* kvint = new KifVectorInteger(kifcode, NULL);
					kvint->Setreference(kinfo->reference + 1);
					kinfo->values["labels"] = kvint;
					for (j = 0; j < nr_class; j++)
						kvint->values.push_back(labels[j]);
				}
				free(labels);
			}
		}

		//line = (char *)malloc(max_line_len*sizeof(char));
		while (readline(input) != NULL)
		{
			int i = 0;
			double target_label, predict_label;
			char *idx, *val, *label, *endptr;
			int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0

			label = strtok(line, " \t\n");
			if (label == NULL) // empty line
				return exit_input_error(total + 1);

			target_label = strtod(label, &endptr);
			if (endptr == label || *endptr != '\0')
				return exit_input_error(total + 1);

			while (1)
			{
				if (i >= max_nr_attr - 1)	// need one more for index = -1
				{
					max_nr_attr *= 2;
					x = (struct svm_node *) realloc(x, max_nr_attr*sizeof(struct svm_node));
				}

				idx = strtok(NULL, ":");
				val = strtok(NULL, " \t");

				if (val == NULL)
					break;
				errno = 0;
				x[i].index = (int)strtol(idx, &endptr, 10);
				if (endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
					return exit_input_error(total + 1);
				else
					inst_max_index = x[i].index;

				errno = 0;
				x[i].value = strtod(val, &endptr);
				if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
					return exit_input_error(total + 1);

				++i;
			}
			x[i].index = -1;

			if (predict_probability && (svm_type == C_SVC || svm_type == NU_SVC)) {
				KifTreeMap* mp = new KifTreeMap(kifcode, NULL);
				mp->Setreference(output->reference + 1);
				output->values.push_back(mp);
				predict_label = svm_predict_probability(model, x, prob_estimates);
				sprintf_s(buff, 20, "%g", predict_label);
				KifVectorFloat* kvfloat = new KifVectorFloat(kifcode, NULL);
				kvfloat->Setreference(mp->reference + 1);
				mp->values[buff] = kvfloat;
				for (j = 0; j < nr_class; j++)
					kvfloat->values.push_back(prob_estimates[j]);
			}
			else
			{
				predict_label = svm_predict(model, x);
				output->Push(kifcode->Providefloat(predict_label));
			}

			if (predict_label == target_label)
				++correct;

			error += (predict_label - target_label)*(predict_label - target_label);
			sump += predict_label;
			sumt += target_label;
			sumpp += predict_label*predict_label;
			sumtt += target_label*target_label;
			sumpt += predict_label*target_label;
			++total;
		}

		if (infos) {
			if (svm_type == NU_SVR || svm_type == EPSILON_SVR)
			{
				kinfo->Push("Mean squared error (regression)", kifcode->Providefloat(error / total));
				kinfo->Push("Squared correlation coefficient (regression)",
					kifcode->Providefloat(((total*sumpt - sump*sumt)*(total*sumpt - sump*sumt)) / ((total*sumpp - sump*sump)*(total*sumtt - sumt*sumt))));
			}
			else {
				kinfo->Push("Accuracy", kifcode->Providefloat((double)correct / total * 100));
				kinfo->Push("Accuracy (correct)", kifcode->Providefloat(correct));
				kinfo->Push("Accuracy (total)", kifcode->Providefloat(total));
			}
		}

		if (predict_probability)
			free(prob_estimates);
		return output;
	}

	KifElement* do_predict(KifVectorFloat* klabels, KifVector* input, KifVector* output) {
		KifTreeMap* kinfo = NULL;
		if (infos) {
			kinfo = new KifTreeMap(kifcode, NULL);
			kinfo->Setreference(output->reference + 1);
			output->values.push_back(kinfo);
		}

		int correct = 0;
		int total = 0;
		double error = 0;
		double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

		int svm_type = svm_get_svm_type(model);
		int nr_class = svm_get_nr_class(model);
		double *prob_estimates = NULL;
		int j;

		char buff[1000];

		if (predict_probability) {
			if (svm_type == NU_SVR || svm_type == EPSILON_SVR) {
				sprintf_s(buff, 10000, "Prob. model for test data: target value = predicted value + z,\nz: Laplace distribution e^(-|z|/sigma)/(2sigma),sigma=%g\n", svm_get_svr_probability(model));
				output->Push(kifcode->Providestringraw(buff));
			}
			else
			{
				int *labels = (int *)malloc(nr_class*sizeof(int));
				svm_get_labels(model, labels);
				prob_estimates = (double *)malloc(nr_class*sizeof(double));

				if (infos) {
					KifVectorInteger* kvint = new KifVectorInteger(kifcode, NULL);
					kvint->Setreference(kinfo->reference + 1);
					kinfo->values["labels"] = kvint;
					for (j = 0; j < nr_class; j++)
						kvint->values.push_back(labels[j]);
				}
				free(labels);
			}
		}

		max_line_len = 1024;
		map<long, double>::iterator it;
		KifTreeMapIntegerFloat mapbase(NULL, NULL);
		KifTreeMapIntegerFloat* ktmif;
		double target_label = 0, predict_label;

		for (int ix = 0; ix < input->values.size(); ix++) {
			int i = 0;
			int inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
			if (klabels != NULL)
				target_label = klabels->values[ix];
			if (input->values[ix]->type != kifTreeMapIntegerFloat) {
				if (!input->values[ix]->aContainer())
					return kifcode->Returnerror("LLN(011): Wrong map type");
				mapbase.values.clear();
				KifIteration* itr = (KifIteration*)input->values[ix]->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
					mapbase.values[itr->IteratorKeyInteger()] = itr->IteratorValueFloat();
				itr->Release();
				ktmif = &mapbase;
			}
			else
				ktmif = (KifTreeMapIntegerFloat*)input->values[ix];

			map<long, double>::iterator it;
			for (it = ktmif->values.begin(); it != ktmif->values.end(); it++) {

				if (i >= max_nr_attr - 1)	// need one more for index = -1
				{
					max_nr_attr *= 2;
					x = (struct svm_node *) realloc(x, max_nr_attr*sizeof(struct svm_node));
				}

				errno = 0;
				x[i].index = (int)it->first;
				if (x[i].index <= inst_max_index)
					return exit_input_error(total + 1);
				else
					inst_max_index = x[i].index;

				errno = 0;
				x[i].value = it->second;
				++i;
			}
			x[i].index = -1;

			if (predict_probability && (svm_type == C_SVC || svm_type == NU_SVC)) {
				KifTreeMap* mp = new KifTreeMap(kifcode, NULL);
				mp->Setreference(output->reference + 1);
				output->values.push_back(mp);
				predict_label = svm_predict_probability(model, x, prob_estimates);
				sprintf_s(buff, 20, "%g", predict_label);
				KifVectorFloat* kvfloat = new KifVectorFloat(kifcode, NULL);
				kvfloat->Setreference(mp->reference + 1);
				mp->values[buff] = kvfloat;
				for (j = 0; j < nr_class; j++)
					kvfloat->values.push_back(prob_estimates[j]);
			}
			else
			{
				predict_label = svm_predict(model, x);
				output->Push(kifcode->Providefloat(predict_label));
			}

			if (predict_label == target_label)
				++correct;

			error += (predict_label - target_label)*(predict_label - target_label);
			sump += predict_label;
			sumt += target_label;
			sumpp += predict_label*predict_label;
			sumtt += target_label*target_label;
			sumpt += predict_label*target_label;
			++total;
		}

		if (infos) {
			if (svm_type == NU_SVR || svm_type == EPSILON_SVR)
			{
				kinfo->Push("Mean squared error (regression)", kifcode->Providefloat(error / total));
				kinfo->Push("Squared correlation coefficient (regression)",
					kifcode->Providefloat(((total*sumpt - sump*sumt)*(total*sumpt - sump*sumt)) / ((total*sumpp - sump*sump)*(total*sumtt - sumt*sumt))));
			}
			else {
				kinfo->Push("Accuracy", kifcode->Providefloat((double)correct / total * 100));
				kinfo->Push("Accuracy (correct)", kifcode->Providefloat(correct));
				kinfo->Push("Accuracy (total)", kifcode->Providefloat(total));
			}
		}

		if (predict_probability)
			free(prob_estimates);
		return output;
	}

	KifElement* MethodDoPredict(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		if (!loaded)
			return kifcode->Returnerror("LLN(018): Load your model first");

		KifElement* klabels = NULL;
		KifElement* kdata = NULL;

		if (callfunc->Size() == 4) {
			klabels = callfunc->Evaluate(0, domain);
			kdata = callfunc->Evaluate(1, domain);
			predict_probability = callfunc->Evaluate(2, domain)->Integer();
			infos = callfunc->Evaluate(3, domain)->Boolean();
			if (klabels->type != kifVectorFloat || kdata->type != kifVector)
				return kifcode->Returnerror("LLN(007): Wrong type for labels or data");
			if (klabels->Size() != kdata->Size())
				return kifcode->Returnerror("LLN(016): The two containers should have the same size");
		}
		else {
			kdata = callfunc->Evaluate(0, domain);
			predict_probability = callfunc->Evaluate(1, domain)->Integer();
			infos = callfunc->Evaluate(2, domain)->Boolean();
			if (kdata->type != kifVector)
				return kifcode->Returnerror("LLN(007): Wrong type for labels or data");
		}

		KifVector* output = Selectvector(callfunc, contextualpattern);

		x = (struct svm_node *) malloc(max_nr_attr*sizeof(struct svm_node));
		do_predict((KifVectorFloat*)klabels, (KifVector*)kdata, output);
		free(x);
		x = NULL;
		return output;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This section implements the libshortext specifics.


	KifElement* exit_input_error(int line_num) {
		char msg[100];
		sprintf_s(msg, 100, "SVM(002): Wrong input format at line %d", line_num);
		return kifcode->Returnerror(msg);
	}

	char* readline(FILE *input)
	{
		int len;

		if (fgets(line, max_line_len, input) == NULL)
			return NULL;

		while (strrchr(line, '\n') == NULL)
		{
			max_line_len *= 2;
			line = (char *)realloc(line, max_line_len);
			len = (int)strlen(line);
			if (fgets(line + len, max_line_len - len, input) == NULL)
				break;
		}
		return line;
	}

	KifElement* read_problem(const char *filename) {
		int elements, max_index, inst_max_index, i, j;
		FILE *fp = fopen(filename, "r");
		char *endptr;
		char *idx, *val, *label;
		string message;

		if (fp == NULL)
		{
			message = "SVM(001): Can't open input file ";
			message += filename;
			return kifcode->Returnerror(message);
		}

		prob.l = 0;
		elements = 0;

		// max_line_len = 1024;
		//line = Malloc(char,max_line_len);
		while (readline(fp) != NULL)
		{
			char *p = strtok(line, " \t"); // label

			// features
			while (1)
			{
				p = strtok(NULL, " \t");
				if (p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
					break;
				++elements;
			}
			++elements;
			++prob.l;
		}
		rewind(fp);

		prob.y = Malloc(double, prob.l);
		prob.x = Malloc(struct svm_node *, prob.l);
		x_space = Malloc(struct svm_node, elements);

		max_index = 0;
		j = 0;
		for (i = 0; i < prob.l; i++)
		{
			inst_max_index = -1; // strtol gives 0 if wrong format, and precomputed kernel has <index> start from 0
			readline(fp);
			prob.x[i] = &x_space[j];
			label = strtok(line, " \t\n");
			if (label == NULL) // empty line
				return exit_input_error(i + 1);

			prob.y[i] = strtod(label, &endptr);
			if (endptr == label || *endptr != '\0')
				return exit_input_error(i + 1);

			while (1)
			{
				idx = strtok(NULL, ":");
				val = strtok(NULL, " \t");

				if (val == NULL)
					break;

				errno = 0;
				x_space[j].index = (int)strtol(idx, &endptr, 10);
				if (endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
					return exit_input_error(i + 1);
				else
					inst_max_index = x_space[j].index;

				errno = 0;
				x_space[j].value = strtod(val, &endptr);
				if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
					return exit_input_error(i + 1);

				++j;
			}

			if (inst_max_index > max_index)
				max_index = inst_max_index;
			x_space[j++].index = -1;
		}

		if (param.gamma == 0 && max_index > 0)
			param.gamma = 1.0 / max_index;

		if (param.kernel_type == PRECOMPUTED)
		for (i = 0; i<prob.l; i++)
		{
			if (prob.x[i][0].index != 0)
			{
				message = "Wrong input format: first column must be 0:sample_serial_number";
				fclose(fp);
				return kifcode->Returnerror(message);
			}
			if ((int)prob.x[i][0].value <= 0 || (int)prob.x[i][0].value > max_index)
			{
				message = "Wrong input format: sample_serial_number out of range";
				fclose(fp);
				return kifcode->Returnerror(message);
			}
		}

		fclose(fp);
		return kifTRUE;
	}

	KifElement* do_cross_validation() {
		int i;
		int total_correct = 0;
		double total_error = 0;
		double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
		double *target = Malloc(double, prob.l);

		KifMapStringFloat* kres = new KifMapStringFloat(kifcode, NULL);

		svm_cross_validation(&prob, &param, nr_fold, target);
		if (param.svm_type == EPSILON_SVR ||
			param.svm_type == NU_SVR)
		{
			for (i = 0; i < prob.l; i++)
			{
				double y = prob.y[i];
				double v = target[i];
				total_error += (v - y)*(v - y);
				sumv += v;
				sumy += y;
				sumvv += v*v;
				sumyy += y*y;
				sumvy += v*y;
			}
			kres->values["Cross Validation Mean squared error"] = total_error / prob.l;
			kres->values["Cross Validation Squared correlation coefficient"] =
				((prob.l*sumvy - sumv*sumy)*(prob.l*sumvy - sumv*sumy)) /
				((prob.l*sumvv - sumv*sumv)*(prob.l*sumyy - sumy*sumy));
		}
		else
		{
			for (i = 0; i < prob.l; i++)
			if (target[i] == prob.y[i])
				++total_correct;
			kres->values["Cross Validation Accuracy"] = 100.0*total_correct / prob.l;
		}
		free(target);
		return kres;
	}

	KifElement* parse_options(KifElement* kmap) {
		RAZ();

		string key;
		string val;

		string msg;

		KifIteration* itr = (KifIteration*)kmap->Newiterator(false);
		for (itr->Begin(); itr->End() == kifFALSE; itr->Next()) {
			key = itr->IteratorKeyString();
			val = itr->IteratorValueString();
			switch (key[0])
			{
			case 's':
				param.svm_type = atoi(STR(val));
				break;
			case 't':
				param.kernel_type = atoi(STR(val));
				break;
			case 'd':
				param.degree = atoi(STR(val));
				break;
			case 'g':
				param.gamma = atof(STR(val));
				break;
			case 'r':
				param.coef0 = atof(STR(val));
				break;
			case 'n':
				param.nu = atof(STR(val));
				break;
			case 'm':
				param.cache_size = atof(STR(val));
				break;
			case 'c':
				param.C = atof(STR(val));
				break;
			case 'e':
				param.eps = atof(STR(val));
				break;
			case 'p':
				param.p = atof(STR(val));
				break;
			case 'h':
				param.shrinking = atoi(STR(val));
				break;
			case 'b':
				param.probability = atoi(STR(val));
				break;
			case 'v':
				cross_validation = 1;
				nr_fold = atoi(STR(val));
				if (nr_fold < 2)
				{
					val = "SVM(003): n-fold cross validation: n must >= 2";
					return kifcode->Returnerror(val);
				}
				break;
			case 'w':
				++param.nr_weight;
				param.weight_label = (int *)realloc(param.weight_label, sizeof(int)*param.nr_weight);
				param.weight = (double *)realloc(param.weight, sizeof(double)*param.nr_weight);
				msg = key.substr(1, key.size() - 1);
				param.weight_label[param.nr_weight - 1] = atoi(STR(msg));
				param.weight[param.nr_weight - 1] = atof(STR(val));
				break;
			default:
				val = "SVM(003): ";
				val += key;
				return kifcode->Returnerror(val);
			}
		}
		itr->Release();
		return kifTRUE;
	}

	KifElement* MethodLoad(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		input_file_name = callfunc->Evaluate(0, domain)->String();
		KifElement* koptions = callfunc->Evaluate(1, domain);
		if (koptions->aContainer() == false)
			return kifcode->Returnerror("SVM(090): Options should be a map");

		model_file_name = "";
		if (callfunc->Size() == 3)
			model_file_name = callfunc->Evaluate(2, domain)->String();

		if (parse_options(koptions) == kifRAISEERROR)
			return kifRAISEERROR;

		//Loading the data
		if (read_problem(STR(input_file_name)) == kifRAISEERROR)
			return kifRAISEERROR;

		model = svm_train(&prob, &param);

		KifElement* retour = kifTRUE;
		model = svm_train(&prob, &param);
		if (model_file_name != "") {
			if (svm_save_model(STR(model_file_name), model)) {
				string msgerr = "SVM(094): can't save model to file ";
				msgerr += model_file_name;
				return kifcode->Returnerror(msgerr);
			}
		}

		loaded = true;
		return kifTRUE;
	}

	KifElement* MethodSave(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		if (model == NULL || !loaded)
			return callfunc->kifcode->Returnerror("LLN(021): No model to save");

		model_file_name = callfunc->Evaluate(0, domain)->String();
		if (svm_save_model(STR(model_file_name), model)) {
			string msgerr = "SVM(094): can't save model to file ";
			msgerr += model_file_name;
			return kifcode->Returnerror(msgerr);
		}

		return kifTRUE;
	}

	KifElement* MethodCrossvalidation(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		if (!loaded)
			return kifcode->Returnerror("LLN(008): Load your training data first");
		return do_cross_validation();
	}

	KifElement* MethodClean(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		if (!loaded)
			return kifFALSE;
		CLEAN();
		RAZ();
		return kifTRUE;
	}

	KifElement* MethodOptions(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		KifElement* koptions = callfunc->Evaluate(0, domain);
		if (koptions->aContainer() == false)
			return kifcode->Returnerror("LLN(003): Options should be a map");
		if (parse_options(koptions) == kifRAISEERROR)
			return kifRAISEERROR;
		return kifTRUE;
	}

	KifElement* MethodLoadModel(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		if (loaded)
			return kifcode->Returnerror("LLN(011): Clean your data first");
		model_file_name = callfunc->Evaluate(0, domain)->String();

		if ((model = svm_load_model(STR(model_file_name))) == 0)
			return kifcode->Returnerror("LLN(012): can't open model file");
		loaded = true;
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecuteMethod(short name, KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with "name"
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)
			return (this->*linkedmethods[name])(contextualpattern, domain, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	string String() {
		return model_file_name;
	}

	KifElement* plus(KifElement* a, KifElement* b, KifElement* context, bool autoself) {
		string sa = a->String();
		string sb = b->String();
		sa += sb;
		return kifcode->Providestring(sa);
	}

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> KifLibsvm::kifexportedmethods;
map<short, LibsvmMethod>  KifLibsvm::linkedmethods;
map<string, string> KifLibsvm::infomethods;
hmap<short, bool> KifLibsvm::methodassigned;

static KifLibsvm* local;

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifLibsvmElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	local = new KifLibsvm(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, LibsvmMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	KifLibsvm::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	KifLibsvm::linkedmethods[idname] = func;
	KifLibsvm::methodassigned[idname] = true;
	KifLibsvm::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
//kiflibsvm MUST BE THE NAME of your new library... This name is used to find
//the proper entry point within your code.
extern "C" {
	Exporting bool Init_kiflibsvm(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}

		kifcode->KifInitLib();
		///The new type will be: Libsvm. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifLibsvmElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifLibsvm)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifLibsvmElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("libsvm", CreatekifLibsvmElement);

		KifLibsvm_type = res;

		return true;
	}

	Exporting bool kiflibsvm_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kiflibsvm(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("load", &KifLibsvm::MethodLoad, P_TWO | P_THREE, "load(string inputdata,smap options,string outputfilename): load and your training data with option. outputfilename is optional.");
		MethodInitialization("train", &KifLibsvm::MethodLoad, P_TWO | P_THREE, "train(string inputdata,smap options,string outputfilename): load and train your training data with option. outputfilename is optional.");
		MethodInitialization("savemodel", &KifLibsvm::MethodSave, P_ONE, "savemodel(string outputfilename): save your model in a file");
		MethodInitialization("crossvalidation", &KifLibsvm::MethodCrossvalidation, P_NONE, "crossvalidation(): Relaunch the cross validation with new parameters");
		MethodInitialization("cleandata", &KifLibsvm::MethodClean, P_NONE, "cleandata(): clean internal data");
		MethodInitialization("options", &KifLibsvm::MethodOptions, P_ONE, "options(smap actions): Set the training options");

		MethodInitialization("loadmodel", &KifLibsvm::MethodLoadModel, P_ONE, "loadmodel(string filename): Load your model");
		MethodInitialization("predictfromfile", &KifLibsvm::MethodDoPredictFromFile, P_THREE, "predictfromfile(string input,bool flag_predict_probability,bool infos): Predict from a file");
		MethodInitialization("predict", &KifLibsvm::MethodDoPredict, P_THREE | P_FOUR, "predict(fvector labels,vector data,bool flag_predict_probability,bool infos): Predict from a vector of iftreemap. labels is optional.");

		MethodInitialization("scale", &KifLibsvm::MethodScale, P_TWO, "scale(string inputfile,map options): Scale files along options: 'l': lower_value, 'u':upper_value, 'y':y_lower_value, 'Y':y_upper_value, 'r': restore_file_name,'s':save_file_name.");

		kifGlobals->kifAllFunctions[KifLibsvm_type] = KifLibsvm::methodassigned;
		kifGlobals->kifAllInformation[KifLibsvm_type] = KifLibsvm::infomethods;
		return true;
	}

	Exporting bool kiflibsvm_Terminate(KifCode* kifcode) {
		KifLibsvm::kifexportedmethods.clear();
		KifLibsvm::linkedmethods.clear();
		KifLibsvm::methodassigned.clear();
		KifLibsvm::infomethods.clear();
		return true;
	}
}





