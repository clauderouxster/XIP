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
filename   : kifliblinear.cxx
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
#include "linear.h"

#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"
#include "kifmaps.h"
#include "kiftreemaps.h"

#ifdef WIN32
#define Exporting __declspec(dllexport)
#endif

#define Malloc(type,n) (type *)malloc((n)*sizeof(type))
#define INF HUGE_VAL

short KifNewId(string n);
string KifIdString(short n);

//Replace Liblinear by your own definition
//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType KifLiblinear_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class KifLiblinear;
//This typedef defines a type "LiblinearMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (KifLiblinear::*LiblinearMethod)(KifElement*, KifDomain*, KifCallFunction*, int);

void print_null(const char *s) {}


static map<string, int> solvers;

typedef struct {
	struct problem prob;
	struct feature_node* x_space;
	INT64 len_x_space;
} SVMProblem;

class KifLiblinear : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which is common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, LiblinearMethod> linkedmethods;
	static hmap<short, bool> methodassigned;	static map<string, string> infomethods;


	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	struct feature_node *x_space;
	struct parameter param;
	struct problem prob;
	struct model* model_;
	INT64 flag_cross_validation;
	INT64 nr_fold;
	double bias;
	char *line;
	INT64 max_line_len;
	string infilename;
	char errormessage[1024];
	string model_file_name;
	bool loaded;
	INT64 flag_predict_probability;
	struct feature_node *x;
	INT64 max_nr_attr;
	int infos;
	vector<SVMProblem> svmproblems;
	int num_srcs;
	//---------------------------------------------------------------------------------------------------------------------
	void RAZ() {
		num_srcs = 0;
		svmproblems.clear();
		infos = 1;
		line = NULL;
		x_space = NULL;
		model_ = NULL;
		loaded = false;
		param.solver_type = L2R_L2LOSS_SVC_DUAL;
		param.C = 1;
		param.eps = INF; // see setting below
		param.p = 0.1;
		param.nr_weight = 0;
		param.weight_label = NULL;
		param.weight = NULL;
		//param.ovo = 0;
		flag_cross_validation = 0;
		bias = -1;
		max_nr_attr = 64;
		flag_predict_probability = 0;
		x = NULL;
		prob.y = NULL;
		prob.x = NULL;
	}


	void CLEAN() {
		if (model_ != NULL)
			free_and_destroy_model(&model_);

		model_ = NULL;
		if (x_space != NULL) {
			destroy_param(&param);
			free(x_space);
		}

		if (prob.y != NULL)
			free(prob.y);
		if (prob.x != NULL)
			free(prob.x);

		prob.y = NULL;
		prob.x = NULL;
		x_space = NULL;
		if (line != NULL)
			free(line);

		line = NULL;

		if (x != NULL)
			free(x);
		x = NULL;
	}

	KifLiblinear(KifCode* kifcode, KifElement* base) : KifBasic(kifcode, base, KifLiblinear_type) {
		solvers["L2R_LR"] = 0;
		solvers["L2R_L2LOSS_SVC_DUAL"] = 1;
		solvers["L2R_L2LOSS_SVC"] = 2;
		solvers["L2R_L1LOSS_SVC_DUAL"] = 3;
		solvers["MCSVM_CS"] = 4;
		solvers["L1R_L2LOSS_SVC"] = 5;
		solvers["L1R_LR"] = 6;
		solvers["L2R_LR_DUAL"] = 7;
		solvers["L2R_L2LOSS_SVR = 11"] = 8;
		solvers["L2R_L2LOSS_SVR_DUAL"] = 9;
		solvers["L2R_L1LOSS_SVR_DUAL"] = 10;

		//Do not forget your variable initialisation
		RAZ();
	}

	~KifLiblinear() {
		CLEAN();
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return KifLiblinear_type;
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}


	KifElement* Copy(KifDomain* kp, KifElement* dom = kifNULL) {
		KifLiblinear* kperso = new KifLiblinear(kifcode, kp);
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
		KifElement* mfn = params->Instruction(0)->Exec(kifNULL, domain, kifNULL, idthread, callfunc);
		model_file_name = mfn->String();
		mfn->Release();

		flag_predict_probability = 0;
		if ((model_ = load_model(STR(model_file_name))) == 0)
			return kifcode->Returnerror("LLN(012): can't open model file");

		loaded = true;
		return this;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This section implements the libshortext specifics.

	void freeSVMProblem(SVMProblem svmprob) {
		struct problem *prob = &(svmprob.prob);
		if (prob->x != NULL) free(prob->x);
		if (prob->y != NULL) free(prob->y);
		if (svmprob.x_space != NULL) free(svmprob.x_space);
	}

	void compute_idf(struct problem *prob, vector<double>& idf_val)
	{
		INT64 i, j;
		//double* idf_val = Malloc(double, prob->n);
		//memset(idf_val, 0, sizeof(double) * prob->n);
		idf_val.reserve(prob->n + 1);
		for (i = 0; i < prob->l; ++i)
			idf_val.push_back(0);

		for (i = 0; i < prob->l; ++i)
		{
			struct feature_node* xi = prob->x[i];
			while (xi->index != -1)
			{
				++idf_val[xi->index - 1];
				++xi;
			}
		}

		for (j = 0; j < prob->n; ++j)
		{
			if (idf_val[j] > 0)
				idf_val[j] = log(prob->l / idf_val[j]);
			else
				idf_val[j] = 0;
		}
	}

	KifElement* MethodComputeidf(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (svmproblems.size() == 0 || num_srcs == 0)
			return kifcode->Returnerror("LLN(044): No data");

		KifElement* kvf = callfunc->Evaluate(0, domain);
		if (!kvf->aVectorContainer())
			return kifcode->Returnerror("LLN(043): Expecting a vector container");

		if (kvf->type == kifVectorFloat) {
			for (int i = 0; i < svmproblems.size(); i++)
				compute_idf(&svmproblems[i].prob, ((KifVectorFloat*)kvf)->values);
		}
		else {
			kvf->Clear();
			vector<double> v;
			for (int i = 0; i < svmproblems.size(); i++)
				compute_idf(&svmproblems[i].prob, v);
			for (int i = 0; i < v.size(); i++)
				kvf->Push(kifcode->Providefloat(v[i]));
		}
		return kifTRUE;
	}

	void normalize(struct problem *prob, int binary, int norm, int tf, int idf, vector<double>& idf_val)
	{
		INT64 i;

		for (i = 0; i < prob->l; ++i)
		{
			struct feature_node* xi;

			if (binary)
			{
				xi = prob->x[i];
				while (xi->index != -1)
				{
					xi->value = xi->value != 0;
					++xi;
				}
			}

			if (tf)
			{
				double norm = 0;
				xi = prob->x[i];
				while (xi->index != -1)
				{
					norm += xi->value;
					++xi;
				}

				xi = prob->x[i];
				if (norm != 0)
				while (xi->index != -1)
				{
					xi->value /= norm;
					++xi;
				}
			}

			if (idf)
			{
				xi = prob->x[i];
				while (xi->index != -1)
				{
					xi->value *= idf_val[xi->index - 1];
					++xi;
				}
			}

			if (norm)
			{
				double norm = 0;
				xi = prob->x[i];
				while (xi->index != -1)
				{
					norm += xi->value * xi->value;
					++xi;
				}

				norm = sqrt(norm);

				xi = prob->x[i];
				if (norm != 0)
				while (xi->index != -1)
				{
					xi->value /= norm;
					++xi;
				}
			}
		}
	}

	KifElement* MethodNormalize(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (svmproblems.size() || num_srcs == 0)
			return kifcode->Returnerror("LLN(044): No data");

		KifElement* kvf = callfunc->Evaluate(0, domain);
		if (!kvf->aVectorContainer())
			return kifcode->Returnerror("LLN(043): Expecting a vector container");

		vector<double> v;
		if (kvf->type == kifVectorFloat)
			v = ((KifVectorFloat*)kvf)->values;
		else {
			for (int i = 0; i < v.size(); i++)
				v.push_back(kvf->ValueOnIndex(i)->Float());
		}

		int binary = callfunc->Evaluate(1, domain)->Boolean();
		int norm = callfunc->Evaluate(2, domain)->Boolean();
		int tf = callfunc->Evaluate(3, domain)->Boolean();
		int idf = callfunc->Evaluate(4, domain)->Boolean();

		for (int i = 0; i < svmproblems.size(); i++)
			normalize(&svmproblems[i].prob, binary, norm, tf, idf, v);

		if (kvf->type == kifVectorFloat)
			((KifVectorFloat*)kvf)->values = v;
		else {
			kvf->Clear();
			for (int i = 0; i < v.size(); i++)
				kvf->Push(kifcode->Providefloat(v[i]));
		}

		return kifTRUE;
	}

	SVMProblem load_short_problem(KifVectorFloat* labels, KifVector* data, int& error_code) {
		INT64 max_index, inst_max_index, i;
		INT64 elements, j;
		struct problem xprob;
		SVMProblem svmprob;

		/**
		* error_code:
		* 0	no error
		* > 0	input format error. The error_code value
		* 	indicates the line number.
		* -1	can not open file
		* -2	memory exhausted
		*/
		error_code = 0;


		xprob.l = 0;
		elements = 0;
		for (j = 0; j < data->Size(); j++)
			elements += data->values[j]->Size();
		xprob.l = labels->Size();


		xprob.bias = bias;
		if (xprob.bias >= 0) elements += xprob.l;

		errno = 0;
		xprob.y = Malloc(double, xprob.l);
		xprob.x = Malloc(struct feature_node *, xprob.l);
		struct feature_node* x_space = Malloc(struct feature_node, elements + xprob.l);

		if (errno == ENOMEM)
		{
			error_code = -2;
			return svmprob;
		}

		KifTreeMapIntegerFloat* ktmif;
		KifTreeMapIntegerFloat mapbase(NULL, NULL);
		KifElement* mymap;

		max_index = 0;
		j = 0;
		for (i = 0; i < xprob.l; i++)
		{
			inst_max_index = 0; // strtol gives 0 if wrong format
			xprob.x[i] = &x_space[j];
			xprob.y[i] = labels->values[i];

			mymap = data->values[i];

			if (mymap->type != kifTreeMapIntegerFloat) {
				if (!mymap->aContainer()) {
					sprintf(errormessage, "LLN(011): Wrong map type");
					error_code = -100;
					return svmprob;
				}

				mapbase.values.clear();
				KifIteration* itr = (KifIteration*)mymap->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
					mapbase.values[itr->IteratorKeyInteger()] = itr->IteratorValueFloat();
				itr->Release();
				ktmif = &mapbase;
			}
			else
				ktmif = (KifTreeMapIntegerFloat*)mymap;

			map<long, double>::iterator it;
			for (it = ktmif->values.begin(); it != ktmif->values.end(); it++) {

				errno = 0;
				x_space[j].index = (int)it->first;
				if (x_space[j].index <= inst_max_index)
				{
					error_code = i + 1;
					return svmprob;
				}
				else
					inst_max_index = x_space[j].index;

				errno = 0;
				x_space[j].value = it->second;
				++j;
			}

			if (inst_max_index > max_index)
				max_index = inst_max_index;

			if (xprob.bias >= 0)
				x_space[j++].value = xprob.bias;

			x_space[j++].index = -1;
		}

		if (xprob.bias >= 0)
		{
			xprob.n = max_index + 1;
			for (i = 1; i < xprob.l; i++)
				(xprob.x[i] - 2)->index = xprob.n;
			x_space[j - 2].index = xprob.n;
		}
		else
			xprob.n = max_index;

		svmprob.prob = xprob;
		svmprob.x_space = x_space;
		svmprob.len_x_space = j;

		return svmprob;
	}

	// read in a problem (in libsvm format)
	SVMProblem read_short_problem(const char *filename, double bias, INT64 *error_code)
	{
		INT64 max_index, inst_max_index, i;
		INT64 elements, j;
		FILE *fp = fopen(filename, "r");
		char *endptr;
		char *idx, *val, *label;
		struct problem xprob;
		SVMProblem svmprob;

		/**
		* error_code:
		* 0	no error
		* > 0	input format error. The error_code value
		* 	indicates the line number.
		* -1	can not open file
		* -2	memory exhausted
		*/
		*error_code = 0;

		if (fp == NULL)
		{
			*error_code = -1;
			return svmprob;
		}

		xprob.l = 0;
		elements = 0;
		max_line_len = 1024;
		line = Malloc(char, max_line_len);
		while (readline(fp) != NULL)
		{
			char *p = strtok(line, " \t"); // label

			// features
			while (1)
			{
				p = strtok(NULL, " \t");
				if (p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
					break;
				elements++;
			}
			xprob.l++;
		}
		rewind(fp);

		xprob.bias = bias;
		if (xprob.bias >= 0) elements += xprob.l;

		errno = 0;
		xprob.y = Malloc(double, xprob.l);
		xprob.x = Malloc(struct feature_node *, xprob.l);
		struct feature_node* x_space = Malloc(struct feature_node, elements + xprob.l);

		if (errno == ENOMEM)
		{
			free(line);
			fclose(fp);
			*error_code = -2;
			return svmprob;
		}

		max_index = 0;
		j = 0;
		for (i = 0; i < xprob.l; i++)
		{
			inst_max_index = 0; // strtol gives 0 if wrong format
			readline(fp);
			xprob.x[i] = &x_space[j];
			label = strtok(line, " \t\n");
			if (label == NULL) // empty line
			{
				free(line);
				fclose(fp);
				*error_code = i + 1;
				return svmprob;
			}

			xprob.y[i] = strtod(label, &endptr);
			if (endptr == label || *endptr != '\0')
			{
				free(line);
				fclose(fp);
				*error_code = i + 1;
				return svmprob;
			}

			while (1)
			{
				idx = strtok(NULL, ":");
				val = strtok(NULL, " \t");

				if (val == NULL)
					break;

				errno = 0;
#ifdef WIN32
				x_space[j].index = (INT64)strtol(idx, &endptr, 10);
#else
				x_space[j].index = (INT64)strtoll(idx, &endptr, 10);
#endif
				if (endptr == idx || errno != 0 || *endptr != '\0' || x_space[j].index <= inst_max_index)
				{
					free(line);
					fclose(fp);
					*error_code = i + 1;
					return svmprob;
				}
				else
					inst_max_index = x_space[j].index;

				errno = 0;
				x_space[j].value = strtod(val, &endptr);
				//if(binary) x_space[j].value = x_space[j].value != 0;
				if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
				{
					free(line);
					fclose(fp);
					*error_code = i + 1;
					return svmprob;
				}

				++j;
			}

			if (inst_max_index > max_index)
				max_index = inst_max_index;

			if (xprob.bias >= 0)
				x_space[j++].value = xprob.bias;

			x_space[j++].index = -1;
		}

		if (xprob.bias >= 0)
		{
			xprob.n = max_index + 1;
			for (i = 1; i < xprob.l; i++)
				(xprob.x[i] - 2)->index = xprob.n;
			x_space[j - 2].index = xprob.n;
		}
		else
			xprob.n = max_index;

		fclose(fp);
		free(line);

		svmprob.prob = xprob;
		svmprob.x_space = x_space;
		svmprob.len_x_space = j;

		return svmprob;
	}

	int load_vector_problems(KifVectorFloat* labels, KifVector* data) {
		int error_code = 0;
		SVMProblem svmp = load_short_problem(labels, data, error_code);

		if (error_code != 0) {
			switch (error_code) {
			case -2:
				sprintf(errormessage, "ERROR: Memory exhausted when reading");
				break;
			default: /* error_code  > 0 input format error*/
				sprintf(errormessage, "ERROR: input format error at line %ld", (long)error_code);
				break;
			}
			freeSVMProblem(svmp);
			return -1;
		}

		num_srcs++;
		svmproblems.push_back(svmp);
		// Make sure # of instances are all equal.
		for (int i = 1; i < svmproblems.size(); i++)
		{
			if (svmproblems[i].prob.l != svmproblems[i - 1].prob.l)
			{
				sprintf(errormessage, "ERROR: #insts = %ld, but #insts in previous = %ld\n",
					(long)svmproblems[i].prob.l, (long)svmproblems[i - 1].prob.l);
				return 3;
			}
		}

		return 0;
	}

	KifElement* MethodAddproblemvectors(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* klabels = callfunc->Evaluate(0, domain);
		KifElement* kdata = callfunc->Evaluate(1, domain);
		if (klabels->type != kifVectorFloat || kdata->type != kifVector)
			return kifcode->Returnerror("LLN(007): Wrong type for labels or data");

		if (klabels->Size() != kdata->Size())
			return kifcode->Returnerror("LLN(016): The two containers should have the same size");
		int err = load_vector_problems((KifVectorFloat*)klabels, (KifVector*)kdata);
		if (err) {
			string message(errormessage);
			return kifcode->Returnerror(message);
		}
		return kifTRUE;
	}

	int load_merge_problems(vector<string>& sources) {
		int i;
		const double bias = -1;
		num_srcs = sources.size();
		INT64 error_code = 0;

		/**
		* error_code:
		* 0	no error
		* > 0	input format error. The error_code value
		* 	indicates the line number.
		* -1	can not open file
		* -2	memory exhausted
		* -3	input files contain different numbsers of instances
		* -4   no file given
		*/

		if (num_srcs <= 0)
			return -4;

		svmproblems.clear();

		for (i = 0; i < num_srcs; i++)
		{
			char* srcs = STR(sources[i]);
			svmproblems.push_back(read_short_problem(srcs, bias, &error_code));
			if (error_code != 0) {
				switch (error_code) {
				case -1:
					sprintf(errormessage, "ERROR: Cannot open input file: %s\n", srcs);
					break;
				case -2:
					sprintf(errormessage, "ERROR: Memory exhausted when reading %s\n", srcs);
					break;
				default: /* error_code  > 0 input format error*/
					sprintf(errormessage, "ERROR: input format error at line %ld in %s\n", (long)error_code, srcs);
					break;
				}

				for (int j = 0; j < i; j++)
					freeSVMProblem(svmproblems[j]);

				svmproblems.clear();
				num_srcs = 0;
				return error_code;
			}
		}

		// Make sure # of instances are all equal.
		for (i = 1; i < num_srcs; i++)
		{
			if (svmproblems[i].prob.l != svmproblems[i - 1].prob.l)
			{
				sprintf(errormessage, "ERROR: #insts in %s = %ld, but #insts in %s = %ld\n",
					STR(sources[i]), (long)svmproblems[i].prob.l, STR(sources[i - 1]), (long)svmproblems[i - 1].prob.l);
				return 3;
			}
		}

		return error_code;
	}

	KifElement* MethodLoadproblems(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* kvect = callfunc->Evaluate(0, domain);
		vector<string> sources;
		if (!kvect->aVectorContainer())
			return kifcode->Returnerror("LLN(047): Wrong type: expecting a vector");
		if (kvect->type == kifVectorString)
			sources = ((KifVectorString*)kvect)->values;
		else {
			for (int i = 0; i < kvect->Size(); i++)
				sources.push_back(kvect->ValueOnIndex(i)->String());
		}
		int err = load_merge_problems(sources);
		if (err) {
			string msg = "LLN(049): ";
			msg += errormessage;
			return kifcode->Returnerror(msg);
		}
		return kifTRUE;
	}

	int analysis_problems(INT64* offsets, KifVector* kvect, bool training) {
		int i, j;
		INT64 error_code = 0;

		/**
		* error_code:
		* 0	no error
		* > 0	input format error. The error_code value
		* 	indicates the line number.
		* -1	can not open file
		* -2	memory exhausted
		* -3	input files contain different numbsers of instances
		* -4   no file given
		*/


		// Overwrite offsets
		if (training) {
			offsets[0] = svmproblems[0].prob.n;
			for (i = 1; i < num_srcs; i++)
				offsets[i] = offsets[i - 1] + svmproblems[i].prob.n;
		}


		for (j = 0; j < svmproblems[0].prob.l; j++)
		{
			INT64 base = 0;
			KifVector* kvi = new KifVector(kifcode, NULL);
			kvi->values.push_back(kifcode->Providefloat(svmproblems[0].prob.y[j]));
			KifMapIntegerFloat* kval = new KifMapIntegerFloat(kifcode, NULL);
			kvi->Push(kval);
			kvect->Push(kvi);
			for (i = 0; i < num_srcs; i++)
			{
				struct feature_node* node;

				for (node = svmproblems[i].prob.x[j]; node->index != -1; node++)
				{
					INT64 index = base + node->index;
					if (index <= offsets[i])
						kval->values[index] = node->value;
					else
						break;
				}
				base = offsets[i];
			}
		}
		return 0;
	}

	KifElement* MethodAnalysisproblems(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (svmproblems.size() || num_srcs == 0)
			return kifcode->Returnerror("LLN(044): No data");

		bool training = callfunc->Evaluate(0, domain)->Boolean();
		INT64 offsets[1000];
		KifVector* kvect = Selectvector(callfunc, contextualpattern);
		analysis_problems(offsets, kvect, training);
		return kifTRUE;
	}

	KifElement* MethodCleaning(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (svmproblems.size() || num_srcs == 0)
			return kifcode->Returnerror("LLN(044): No data");

		for (int i = 0; i < num_srcs; i++)
			freeSVMProblem(svmproblems[i]);
		svmproblems.clear();
		num_srcs = 0;
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* do_predict_with_file(FILE *input, KifVector* output) {

		KifTreeMap* kinfo = NULL;
		if (infos) {
			kinfo = new KifTreeMap(kifcode, NULL);
			kinfo->Setreference(output->reference + 1);
			output->values.push_back(kinfo);
		}


		INT64 correct = 0;
		INT64 total = 0;
		double error = 0;
		double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

		INT64 nr_class = get_nr_class(model_);
		double *prob_estimates = NULL;
		INT64 j, n;
		INT64 nr_feature = get_nr_feature(model_);
		if (model_->bias >= 0)
			n = nr_feature + 1;
		else
			n = nr_feature;

		KifVectorInteger* kindexlabels = NULL;
		if (flag_predict_probability) {
			INT64 *labels;

			if (!check_probability_model(model_))
				return kifcode->Returnerror("LLN(013): probability output is only supported for logistic regression");

			labels = (INT64 *)malloc(nr_class*sizeof(INT64));
			get_labels(model_, labels);
			prob_estimates = (double *)malloc(nr_class*sizeof(double));
			if (infos) {
				kindexlabels = new KifVectorInteger(kifcode, NULL);
				kindexlabels->Setreference(kinfo->reference + 1);
				kinfo->values["labels"] = kindexlabels;
				for (j = 0; j < nr_class; j++)
					kindexlabels->values.push_back(labels[j]);
			}
			free(labels);
		}

		char buff[20];

		max_line_len = 1024;
		line = (char *)malloc(max_line_len*sizeof(char));
		INT64 ix = 0;
		while (readline(input) != NULL)
		{
			INT64 i = 0;
			double target_label, predict_label;
			char *idx, *val, *label, *endptr;
			INT64 inst_max_index = 0; // strtol gives 0 if wrong format

			label = strtok(line, " \t\n");
			if (label == NULL) // empty line
				exit_input_error(total + 1);

			target_label = strtod(label, &endptr);
			if (endptr == label || *endptr != '\0')
				exit_input_error(total + 1);

			while (1)
			{
				if (i >= max_nr_attr - 2)	// need one more for index = -1
				{
					max_nr_attr *= 2;
					x = (struct feature_node *) realloc(x, max_nr_attr*sizeof(struct feature_node));
				}

				idx = strtok(NULL, ":");
				val = strtok(NULL, " \t");

				if (val == NULL)
					break;
				errno = 0;
				x[i].index = (int)strtol(idx, &endptr, 10);
				if (endptr == idx || errno != 0 || *endptr != '\0' || x[i].index <= inst_max_index)
					exit_input_error(total + 1);
				else
					inst_max_index = x[i].index;

				errno = 0;
				x[i].value = strtod(val, &endptr);
				if (endptr == val || errno != 0 || (*endptr != '\0' && !isspace(*endptr)))
					exit_input_error(total + 1);

				// feature indices larger than those in training are not used
				if (x[i].index <= nr_feature)
					++i;
			}

			if (model_->bias >= 0)
			{
				x[i].index = n;
				x[i].value = model_->bias;
				i++;
			}
			x[i].index = -1;

			if (flag_predict_probability)
			{
				INT64 j;
				KifIntegerTreeMap* mp = new KifIntegerTreeMap(kifcode, NULL);
				mp->Setreference(output->reference + 1);
				output->values.push_back(mp);
				predict_label = predict_probability(model_, x, prob_estimates);
				if (infos == 1 || kindexlabels == NULL) {
					KifVectorFloat* kvfloat = new KifVectorFloat(kifcode, NULL);
					kvfloat->Setreference(mp->reference + 1);
					mp->values[predict_label] = kvfloat;
					for (j = 0; j < model_->nr_class; j++)
						kvfloat->values.push_back(prob_estimates[j]);
				}
				else {
					if (infos == 2) {
						KifTreeMapIntegerFloat* kmlabelfloat = new KifTreeMapIntegerFloat(kifcode, NULL);
						kmlabelfloat->Setreference(mp->reference + 1);
						mp->values[predict_label] = kmlabelfloat;
						int ilab;
						for (j = 0; j < model_->nr_class; j++) {
							ilab = kindexlabels->values[j];
							kmlabelfloat->values[ilab] = prob_estimates[j];
						}
					}
				}
			}
			else
			{
				predict_label = predict(model_, x);
				output->Push(kifcode->Providefloat(predict_label));
				ix++;
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
			if (model_->param.solver_type == L2R_L2LOSS_SVR ||
				model_->param.solver_type == L2R_L1LOSS_SVR_DUAL ||
				model_->param.solver_type == L2R_L2LOSS_SVR_DUAL)	{
				kinfo->Push("Mean squared error (regression)", kifcode->Providefloat(error / total));
				kinfo->Push("Squared correlation coefficient (regression)",
					kifcode->Providefloat(((total*sumpt - sump*sumt)*(total*sumpt - sump*sumt)) / ((total*sumpp - sump*sump)*(total*sumtt - sumt*sumt))));
			}
			else {
				kinfo->Push("Accuracy", kifcode->Providefloat(correct / total * 100));
				kinfo->Push("Accuracy (correct)", kifcode->Providefloat(correct));
				kinfo->Push("Accuracy (total)", kifcode->Providefloat(total));
			}
		}

		if (flag_predict_probability)
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

		INT64 correct = 0;
		INT64 total = 0;
		double error = 0;
		double sump = 0, sumt = 0, sumpp = 0, sumtt = 0, sumpt = 0;

		INT64 nr_class = get_nr_class(model_);
		double *prob_estimates = NULL;
		INT64 j, n;
		INT64 nr_feature = get_nr_feature(model_);
		if (model_->bias >= 0)
			n = nr_feature + 1;
		else
			n = nr_feature;

		KifVectorInteger* kindexlabels = NULL;
		if (flag_predict_probability)
		{
			INT64 *labels;

			if (!check_probability_model(model_))
				return kifcode->Returnerror("LLN(013): probability output is only supported for logistic regression");

			labels = (INT64 *)malloc(nr_class*sizeof(INT64));
			get_labels(model_, labels);
			prob_estimates = (double *)malloc(nr_class*sizeof(double));
			if (infos) {
				kindexlabels = new KifVectorInteger(kifcode, NULL);
				kindexlabels->Setreference(kinfo->reference + 1);
				kinfo->values["labels"] = kindexlabels;
				for (j = 0; j < nr_class; j++)
					kindexlabels->values.push_back(labels[j]);
			}
			free(labels);
		}

		max_line_len = 1024;
		line = (char *)malloc(max_line_len*sizeof(char));
		map<long, double>::iterator it;

		double target_label = 0, predict_label;
		KifTreeMapIntegerFloat mapbase(NULL, NULL);
		KifTreeMapIntegerFloat* ktmif;

		for (INT64 ix = 0; ix < input->values.size(); ix++) {
			INT64 i = 0;
			INT64 inst_max_index = 0; // strtol gives 0 if wrong format
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

				if (i >= max_nr_attr - 2)	// need one more for index = -1
				{
					max_nr_attr *= 2;
					x = (struct feature_node *) realloc(x, max_nr_attr*sizeof(struct feature_node));
				}

				errno = 0;
				x[i].index = (int)it->first;
				if (x[i].index <= inst_max_index)
					exit_input_error(total + 1);
				else
					inst_max_index = x[i].index;

				errno = 0;
				x[i].value = it->second;
				// feature indices larger than those in training are not used
				if (x[i].index <= nr_feature)
					++i;
			}

			if (model_->bias >= 0)
			{
				x[i].index = n;
				x[i].value = model_->bias;
				i++;
			}
			x[i].index = -1;

			if (flag_predict_probability)
			{
				INT64 j;
				KifIntegerTreeMap* mp = new KifIntegerTreeMap(kifcode, NULL);
				mp->Setreference(output->reference + 1);
				output->values.push_back(mp);
				predict_label = predict_probability(model_, x, prob_estimates);
				if (infos == 1 || kindexlabels == NULL) {
					KifVectorFloat* kvfloat = new KifVectorFloat(kifcode, NULL);
					kvfloat->Setreference(mp->reference + 1);
					mp->values[predict_label] = kvfloat;
					for (j = 0; j < model_->nr_class; j++)
						kvfloat->values.push_back(prob_estimates[j]);
				}
				else {
					if (infos == 2) {
						KifTreeMapIntegerFloat* kmlabelfloat = new KifTreeMapIntegerFloat(kifcode, NULL);
						kmlabelfloat->Setreference(mp->reference + 1);
						mp->values[predict_label] = kmlabelfloat;
						int ilab;
						for (j = 0; j < model_->nr_class; j++) {
							ilab = kindexlabels->values[j];
							kmlabelfloat->values[ilab] = prob_estimates[j];
						}
					}
				}
			}
			else
			{
				predict_label = predict(model_, x);
				output->Push(kifcode->Providefloat(predict_label));
				ix++;
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
			if (model_->param.solver_type == L2R_L2LOSS_SVR ||
				model_->param.solver_type == L2R_L1LOSS_SVR_DUAL ||
				model_->param.solver_type == L2R_L2LOSS_SVR_DUAL)	{
				kinfo->Push("Mean squared error (regression)", kifcode->Providefloat(error / total));
				kinfo->Push("Squared correlation coefficient (regression)",
					kifcode->Providefloat(((total*sumpt - sump*sumt)*(total*sumpt - sump*sumt)) / ((total*sumpp - sump*sump)*(total*sumtt - sumt*sumt))));
			}
			else {
				kinfo->Push("Accuracy", kifcode->Providefloat(correct / total * 100));
				kinfo->Push("Accuracy (correct)", kifcode->Providefloat(correct));
				kinfo->Push("Accuracy (total)", kifcode->Providefloat(total));
			}
		}

		if (flag_predict_probability)
			free(prob_estimates);

		return output;
	}

	KifElement* do_cross_validation() {
		INT64 i;
		INT64 total_correct = 0;
		double total_error = 0;
		double sumv = 0, sumy = 0, sumvv = 0, sumyy = 0, sumvy = 0;
		double *target = Malloc(double, prob.l);

		KifMapStringFloat* kres = new KifMapStringFloat(kifcode, NULL);

		cross_validation(&prob, &param, nr_fold, target);
		if (param.solver_type == L2R_L2LOSS_SVR ||
			param.solver_type == L2R_L1LOSS_SVR_DUAL ||
			param.solver_type == L2R_L2LOSS_SVR_DUAL)
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

	KifElement* exit_with_help(string o) {
		string msg = "LLN(001): Error with options:";
		msg += o;
		return kifcode->Returnerror(msg);
	}

	KifElement* parse_options(KifElement* kmap)
	{
		void(*print_func)(const char*) = &print_null;	// default printing to stdout

		// default values
		if (!loaded) {
			param.solver_type = L2R_L2LOSS_SVC_DUAL;
			param.C = 1;
			param.eps = INF; // see setting below
			param.p = 0.1;
			param.nr_weight = 0;
			param.weight_label = NULL;
			param.weight = NULL;
			//param.ovo = 0;
			flag_cross_validation = 0;
			bias = -1;
		}

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
				if (solvers.find(val) == solvers.end())
					param.solver_type = atoi(STR(val));
				else
					param.solver_type = solvers[val];
				break;
			case 'c':
				param.C = atof(STR(val));
				break;

			case 'p':
				param.p = atof(STR(val));
				break;

			case 'e':
				param.eps = atof(STR(val));
				break;

			case 'B':
				bias = atof(STR(val));
				break;

			case 'w':
				++param.nr_weight;
				param.weight_label = (INT64 *)realloc(param.weight_label, sizeof(int)*param.nr_weight);
				param.weight = (double *)realloc(param.weight, sizeof(double)*param.nr_weight);
				msg = key.substr(1, key.size() - 1);
				param.weight_label[param.nr_weight - 1] = atoi(STR(msg));
				param.weight[param.nr_weight - 1] = atoi(STR(val));
				break;

			case 'M':
				//param.ovo = atoi(STR(val));
				break;
			case 'v':
				flag_cross_validation = 1;
				nr_fold = atoi(STR(val));
				if (nr_fold < 2)
				{
					msg = "LLN(019): n-fold cross validation: n must >= 2";
					return exit_with_help(msg);
				}
				break;
			case 'q':
				print_func = &print_null;
				break;

			default:
				return exit_with_help(key);
			}
		}

		itr->Release();

		set_print_string_function(print_func);

		if (param.eps == INF)
		{
			switch (param.solver_type)
			{
			case L2R_LR:
			case L2R_L2LOSS_SVC:
				param.eps = 0.01;
				break;
			case L2R_L2LOSS_SVR:
				param.eps = 0.001;
				break;
			case L2R_L2LOSS_SVC_DUAL:
			case L2R_L1LOSS_SVC_DUAL:
			case MCSVM_CS:
			case L2R_LR_DUAL:
				param.eps = 0.1;
				break;
			case L1R_L2LOSS_SVC:
			case L1R_LR:
				break;
			case L2R_L1LOSS_SVR_DUAL:
			case L2R_L2LOSS_SVR_DUAL:
				param.eps = 0.1;
				break;
			}
		}
		return kifTRUE;
	}

	KifElement* exit_input_error(int line_num) {
		char msg[100];
		sprintf_s(msg, 100, "LLN(002): Wrong input format at line %d", line_num);
		return kifcode->Returnerror(msg);
	}

	char* readline(FILE *input)
	{
		INT64 len;

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
		INT64 max_index, inst_max_index, i;
		INT64 elements, j;
		FILE *fp = fopen(filename, "r");
		char *endptr;
		char *idx, *val, *label;

		if (fp == NULL)
		{
			char buff[100];
			sprintf_s(buff, 100, "can't open input file %s", filename);
			return kifcode->Returnerror(buff);
		}

		prob.l = 0;
		elements = 0;
		max_line_len = 1024;
		line = Malloc(char, max_line_len);
		while (readline(fp) != NULL)
		{
			char *p = strtok(line, " \t"); // label

			// features
			while (1)
			{
				p = strtok(NULL, " \t");
				if (p == NULL || *p == '\n') // check '\n' as ' ' may be after the last feature
					break;
				elements++;
			}
			elements++; // for bias term
			prob.l++;
		}
		rewind(fp);

		prob.bias = bias;

		prob.y = Malloc(double, prob.l);
		prob.x = Malloc(struct feature_node *, prob.l);
		x_space = Malloc(struct feature_node, elements + prob.l);

		max_index = 0;
		j = 0;
		for (i = 0; i < prob.l; i++)
		{
			inst_max_index = 0; // strtol gives 0 if wrong format
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

			if (prob.bias >= 0)
				x_space[j++].value = prob.bias;

			x_space[j++].index = -1;
		}

		if (prob.bias >= 0)
		{
			prob.n = max_index + 1;
			for (i = 1; i < prob.l; i++)
				(prob.x[i] - 2)->index = prob.n;
			x_space[j - 2].index = prob.n;
		}
		else
			prob.n = max_index;

		fclose(fp);
		return kifTRUE;
	}

	//Load the training data from vectors...
	KifElement* load_problem(KifVectorFloat* labels, KifVector* data) {
		if (loaded)
			return kifcode->Returnerror("LLN(010): Clean your data first");

		INT64 max_index, inst_max_index, i;
		INT64 elements, j;

		prob.l = 0;
		elements = 0;
		for (j = 0; j < data->Size(); j++)
			elements += data->values[j]->Size() + 1;
		prob.l = labels->Size();

		prob.bias = bias;

		prob.y = Malloc(double, prob.l);
		prob.x = Malloc(struct feature_node *, prob.l);
		x_space = Malloc(struct feature_node, elements + prob.l);

		max_index = 0;
		j = 0;
		KifTreeMapIntegerFloat* ktmif;
		KifTreeMapIntegerFloat mapbase(NULL, NULL);
		KifElement* mymap;

		for (i = 0; i < prob.l; i++)
		{
			inst_max_index = 0; // strtol gives 0 if wrong format
			prob.x[i] = &x_space[j];

			prob.y[i] = labels->values[i];

			mymap = data->values[i];

			if (mymap->type != kifTreeMapIntegerFloat) {
				if (!mymap->aContainer())
					return kifcode->Returnerror("LLN(011): Wrong map type");
				mapbase.values.clear();
				KifIteration* itr = (KifIteration*)mymap->Newiterator(false);
				for (itr->Begin(); itr->End() == kifFALSE; itr->Next())
					mapbase.values[itr->IteratorKeyInteger()] = itr->IteratorValueFloat();
				itr->Release();
				ktmif = &mapbase;
			}
			else
				ktmif = (KifTreeMapIntegerFloat*)mymap;

			map<long, double>::iterator it;
			for (it = ktmif->values.begin(); it != ktmif->values.end(); it++) {

				errno = 0;
				x_space[j].index = (int)it->first;
				if (x_space[j].index <= inst_max_index)
					return exit_input_error(i + 1);
				else
					inst_max_index = x_space[j].index;

				errno = 0;
				x_space[j].value = it->second;
				++j;
			}

			if (inst_max_index > max_index)
				max_index = inst_max_index;

			if (prob.bias >= 0)
				x_space[j++].value = prob.bias;

			x_space[j++].index = -1;
		}

		if (prob.bias >= 0)
		{
			prob.n = max_index + 1;
			for (i = 1; i < prob.l; i++)
				(prob.x[i] - 2)->index = prob.n;
			x_space[j - 2].index = prob.n;
		}
		else
			prob.n = max_index;

		return kifTRUE;
	}



	KifElement* MethodCrossvalidation(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!loaded)
			return kifcode->Returnerror("LLN(008): Load your training data first");
		return do_cross_validation();
	}

	KifElement* MethodLoadModel(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (loaded)
			return kifcode->Returnerror("LLN(011): Clean your data first");

		flag_predict_probability = 0;
		model_file_name = callfunc->Evaluate(0, domain)->String();

		if ((model_ = load_model(STR(model_file_name))) == 0)
			return kifcode->Returnerror("LLN(012): can't open model file");
		loaded = true;
		return kifTRUE;
	}

	KifElement* MethodLoad(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		infilename = callfunc->Evaluate(0, domain)->String();

		KifElement* koptions = callfunc->Evaluate(1, domain);
		if (koptions->aContainer() == false)
			return kifcode->Returnerror("LLN(003): Options should be a map");

		model_file_name = "";
		if (callfunc->Size() == 3)
			model_file_name = callfunc->Evaluate(2, domain)->String();

		if (parse_options(koptions) == kifRAISEERROR)
			return kifRAISEERROR;

		//Loading the data
		if (read_problem(STR(infilename)) == kifRAISEERROR)
			return kifRAISEERROR;

		const char *error_msg = check_parameter(&prob, &param);

		string msgerr;
		if (error_msg)
		{
			msgerr = "LLN(005): ERROR: ";
			msgerr += error_msg;
			return kifcode->Returnerror(msgerr);
		}

		KifElement* retour = kifTRUE;
		model_ = train(&prob, &param);
		if (model_file_name != "") {
			if (save_model(STR(model_file_name), model_)) {
				msgerr = "LLN(004): can't save model to file ";
				msgerr += model_file_name;
				return kifcode->Returnerror(msgerr);
			}
		}

		loaded = true;
		return kifTRUE;
	}

	KifElement* MethodSave(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (model_ == NULL || !loaded)
			return callfunc->kifcode->Returnerror("LLN(021): No model to save");

		model_file_name = callfunc->Evaluate(0, domain)->String();
		if (save_model(STR(model_file_name), model_)) {
			string msgerr = "LLN(004): can't save model to file ";
			msgerr += model_file_name;
			return kifcode->Returnerror(msgerr);
		}

		return kifTRUE;
	}

	KifElement* MethodClean(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!loaded)
			return kifFALSE;
		CLEAN();
		RAZ();
		return kifTRUE;
	}

	KifElement* MethodOptions(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		KifElement* koptions = callfunc->Evaluate(0, domain);
		if (koptions->aContainer() == false)
			return kifcode->Returnerror("LLN(003): Options should be a map");
		if (parse_options(koptions) == kifRAISEERROR)
			return kifRAISEERROR;
		return kifTRUE;
	}

	KifElement* MethodTrainingSet(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (loaded)
			return kifcode->Returnerror("LLN(011): Clean your data first");
		KifElement* klabels = callfunc->Evaluate(0, domain);
		KifElement* kdata = callfunc->Evaluate(1, domain);

		if (klabels->type != kifVectorFloat || kdata->type != kifVector)
			return kifcode->Returnerror("LLN(007): Wrong type for labels or data");


		if (klabels->Size() != kdata->Size())
			return kifcode->Returnerror("LLN(016): The two containers should have the same size");

		return load_problem((KifVectorFloat*)klabels, (KifVector*)kdata);
	}

	KifElement* MethodDoPredictFromFile(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!loaded)
			return kifcode->Returnerror("LLN(018): Load your model first");

		string inputfile = callfunc->Evaluate(0, domain)->String();
		{
			ifstream fl(STR(inputfile), modeOuverture);
			if (fl.fail())
				return kifcode->Returnerror("LLN(014): Can't open input file");
		}

		FILE* input;
		input = fopen(STR(inputfile), "r");
		if (input == NULL)
			return kifcode->Returnerror("LLN(014): Can't open input file");

		flag_predict_probability = callfunc->Evaluate(1, domain)->Integer();

		x = (struct feature_node *) malloc(max_nr_attr*sizeof(struct feature_node));
		infos = callfunc->Evaluate(1, domain)->Integer();

		KifVector* output = Selectvector(callfunc, contextualpattern);
		do_predict_with_file(input, output);
		fclose(input);
		free(x);
		x = NULL;
		return output;
	}

	KifElement* MethodDoPredict(KifElement* contextualpattern, KifDomain* domain, KifCallFunction* callfunc, int idthread) {
		if (!loaded)
			return kifcode->Returnerror("LLN(018): Load your model first");

		KifElement* klabels = NULL;
		KifElement* kdata = NULL;

		if (callfunc->Size() == 4) {
			klabels = callfunc->Evaluate(0, domain);
			kdata = callfunc->Evaluate(1, domain);
			flag_predict_probability = callfunc->Evaluate(2, domain)->Integer();
			infos = callfunc->Evaluate(3, domain)->Integer();
			if (klabels->type != kifVectorFloat || kdata->type != kifVector)
				return kifcode->Returnerror("LLN(007): Wrong type for labels or data");
			if (klabels->Size() != kdata->Size())
				return kifcode->Returnerror("LLN(016): The two containers should have the same size");
		}
		else {
			kdata = callfunc->Evaluate(0, domain);
			flag_predict_probability = callfunc->Evaluate(1, domain)->Integer();
			infos = callfunc->Evaluate(2, domain)->Integer();
			if (kdata->type != kifVector)
				return kifcode->Returnerror("LLN(007): Wrong type for labels or data");
		}

		KifVector* output = Selectvector(callfunc, contextualpattern);

		x = (struct feature_node *) malloc(max_nr_attr*sizeof(struct feature_node));
		do_predict((KifVectorFloat*)klabels, (KifVector*)kdata, output);
		free(x);
		x = NULL;
		return output;
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
map<short, KifCallMethod> KifLiblinear::kifexportedmethods;
map<short, LiblinearMethod>  KifLiblinear::linkedmethods;
map<string, string> KifLiblinear::infomethods;
hmap<short, bool> KifLiblinear::methodassigned;

static KifLiblinear* local;

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifLiblinearElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	local = new KifLiblinear(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(string name, LiblinearMethod func, unsigned int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	KifLiblinear::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	KifLiblinear::linkedmethods[idname] = func;
	KifLiblinear::methodassigned[idname] = true;
	KifLiblinear::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
//kifliblinear MUST BE THE NAME of your new library... This name is used to find
//the proper entry point within your code.
extern "C" {
	Exporting bool Init_kifliblinear(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}

		kifcode->KifInitLib();
		///The new type will be: Liblinear. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifLiblinearElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: KifLiblinear)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifLiblinearElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("liblinear", CreatekifLiblinearElement);

		KifLiblinear_type = res;

		return true;
	}

	Exporting bool kifliblinear_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifliblinear(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization("load", &KifLiblinear::MethodLoad, P_TWO | P_THREE, "load(string inputdata,smap options,string outputfilename): load your training data with option. outputfilename is optional.");
		MethodInitialization("train", &KifLiblinear::MethodLoad, P_TWO | P_THREE, "train(string inputdata,smap options,string outputfilename): train your training data with option. outputfilename is optional.");
		MethodInitialization("savemodel", &KifLiblinear::MethodSave, P_ONE, "savemodel(string outputfilename): save your model in a file");
		MethodInitialization("options", &KifLiblinear::MethodOptions, P_ONE, "options(smap actions): Set the training options");
		MethodInitialization("crossvalidation", &KifLiblinear::MethodCrossvalidation, P_NONE, "crossvalidation(): Relaunch the cross validation with new parameters");
		MethodInitialization("cleandata", &KifLiblinear::MethodClean, P_NONE, "cleandata(): clean internal data");
		MethodInitialization("trainingset", &KifLiblinear::MethodTrainingSet, P_TWO, "trainingset(fvector labels,vector data): create your training set out of a iftreemap vector");
		MethodInitialization("loadmodel", &KifLiblinear::MethodLoadModel, P_ONE, "loadmodel(string filename): Load your model");
		MethodInitialization("predictfromfile", &KifLiblinear::MethodDoPredictFromFile, P_THREE, "predictfromfile(string input,bool flag_predict_probability,int infos): Predict from a file");
		MethodInitialization("predict", &KifLiblinear::MethodDoPredict, P_THREE | P_FOUR, "predict(fvector labels,vector data,bool flag_predict_probability,int infos): Predict from a vector of iftreemap. labels is optional.");

		MethodInitialization("stloadproblems", &KifLiblinear::MethodLoadproblems, P_ONE, "stloadproblems(svector files): Load and merge a list of training files (libshorttext).");
		MethodInitialization("staddproblems", &KifLiblinear::MethodAddproblemvectors, P_TWO, "staddproblems(fvector labels,vector data): create your training set out of a iftreemap vector (libshorttext), which is merged with previous training sets.");
		MethodInitialization("stcomputeidf", &KifLiblinear::MethodComputeidf, P_ONE, "stcomputeidf(fvector idfval): Compute the idv_val vector (libshorttext).");
		MethodInitialization("stnormalize", &KifLiblinear::MethodNormalize, P_FIVE, "stnormalize(fvector idfval,bool binary, bool norm, bool tf, bool idf): Normalize the idv_val vector (libshorttext).");
		MethodInitialization("stanalyzeproblems", &KifLiblinear::MethodAnalysisproblems, P_ONE, "stanalyzeproblems(bool training): Analyze the training files (libshorttext).");
		MethodInitialization("stclean", &KifLiblinear::MethodCleaning, P_NONE, "stclean(): Cleaning internal structures (libshorttext).");

		kifGlobals->kifAllFunctions[KifLiblinear_type] = KifLiblinear::methodassigned;
		kifGlobals->kifAllInformation[KifLiblinear_type] = KifLiblinear::infomethods;

		return true;
	}

	Exporting bool kifliblinear_Terminate(KifCode* kifcode) {
		KifLiblinear::kifexportedmethods.clear();
		KifLiblinear::linkedmethods.clear();
		KifLiblinear::methodassigned.clear();
		KifLiblinear::infomethods.clear();
		return true;
	}
}





