/* 
 * File:   tagger.cpp
 * Author: fnoorala
 * 
 * Created on August 6, 2013, 1:34 PM
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
using namespace std;

#include "tagger.h"


int TaggerError(void* tg) {
	return ((tagger*)tg)->error;
}

void* CreateTagger(string crfmodel,string crfbrown,string crfmkcls) {
	tagger* tg=new tagger(crfmodel,crfbrown,crfmkcls);
	if (tg->error) {
		delete tg;
		return NULL;
	}
	return (void*)tg;
}

bool TaggingTokens(void* tg,vector<string>& wordSequence,vector<string>& predictedTag) {
	if (tg==NULL)
		return false;
	
	((tagger*)tg)->dotag(wordSequence,predictedTag);
	return true;
}

void CleanTagger(void* tg) {
	if (tg!=NULL)
		delete (tagger*)tg;
}

tagger::tagger(string mpath,string brown,string mk) : ext(brown,mk),opt(opt_defaults) {
	//string modelPath="/opt/ParSem/users/Farhad/parSem/ntagger/src/oct27.splits/model";
	error=0;
	mdl=NULL;
	modelPath=mpath;
	if (ext.error) {
		error=ext.error;
		return;
	}
	
	mdl = mdl_new(rdr_new(false));
	mdl->opt = &opt;
	mdl->opt->model=STR(modelPath);
	FILE *file = fopen(mdl->opt->model, "r");
	if (file == NULL) {
		error=3;
		return;
	}		
	mdl_load(mdl, file);
	//set option in model in order to show just label for each token
	mdl->opt->label=true;
}

tagger::~tagger() {
	if (mdl!=NULL)
		mdl_free(mdl);
}

/*dotag method: In order to label the given sequence of tokens , This method will call
 * by passing the vector of word sequence and also path of model
 */
bool tagger::dotag(vector<string>& wordSequence,vector<string>& predictedTag) {

	predictedTag.clear();
	//extract features for each word and put it in struct format in order to pass them to wapiti for labeling
	vector< vector <string> > alldata;
	
	ext.features(wordSequence,alldata);

	// Next we prepare the model

	//pass each sequence one by one to labeling
	int idx=0;
	vector< vector <string> > temp;
	vector <string>  ftemp;
	data_t* data;

	while(idx < alldata.size()){

		if(alldata[idx].size()>0){
			ftemp.clear();
			for(int ii=0;ii<alldata[idx].size();ii++)
				ftemp.push_back(alldata[idx][ii]);
			temp.push_back(ftemp);       
			idx+=1;
		}
		else {
			data=fill_rawStruct(temp);
			if (data!=NULL) {
				const char *fout[data->len]; 

				//call crf_tag_label() from wapiti 
				crf_tag_label(mdl, data, fout);

				for(int i=0;i<data->len;i++){
					predictedTag.push_back(fout[i]);
				}
				free(data);
			}
			idx+=1;
			temp.clear();
		}
	}

	if (temp.size()>0) {
		data=fill_rawStruct(temp);
		if (data!=NULL) {
			const char *fout[data->len]; 

			//call crf_tag_label() from wapiti 
			crf_tag_label(mdl, data, fout);

			for(int i=0;i<data->len;i++) 
				predictedTag.push_back(fout[i]);
			free(data);
		}
	}

	return true;            
}


/*fill_rawStruct: This method create the struct "data_t" based on word and its 
* features in order to use it in c source code
*/


data_t* tagger::fill_rawStruct(const vector< vector <string> > &features){
	int size=features.size(),cnt=0;

	data_t  *raw;
	raw=(data_t *)xmalloc(sizeof(raw) + sizeof(char *)* size);
	if(features.size()>0){
		for(int i=0; i<features.size(); i++){
			string line;
			if(features[i].size()>0){  
				for(int p=0; p<features[i].size(); p++) {
					line=line+features[i][p]+"\t";
				}
				(*raw).lines[i]=strdup(line.c_str());
			}          
			cnt+=1;
			raw->len = cnt;
		}
		return(raw);
	}
	else
		free(raw);
	return NULL;
}

/*readfile: This method read annotated file and create vector of words or tokens
* and vector of labels separately
*/
bool tagger::readfile(const string &path, vector<string> &tags,vector<string>& words){
	string line;
	ifstream myfile;
	myfile.open(path.c_str());
	if (myfile.is_open())
	{
		while ( myfile.good() )
		{
			getline (myfile,line);
			string arr[2];
			int i = 0;
			stringstream ssin(line);
			while (ssin.good() && i < 2){
				ssin >> arr[i];
				++i;
			}

			words.push_back(arr[0]);
			tags.push_back(arr[1]);                          
		}
		myfile.close();
		return true;
	}

	return false;
}


/*crfInputFile: this method creates all features which is required for each token 
* in order to train and provide model based on CRF. It creates file in 
* mentioned path 
*/
void tagger::crfInputFile (const string &inputPath,string &outputPath){

	vector<string> tags;
	vector<string> words;
	if (!readfile(inputPath,tags,words))
		return;

	vector< vector<string> > fetureV;
	
	ext.features(words,fetureV);

	ofstream myfile;
	myfile.open(outputPath.c_str());
	for(int i=0; i<fetureV.size(); i++){
		if(!fetureV[i].empty()){
			for(int p=0; p<fetureV[i].size(); p++)
				myfile << fetureV[i][p] << "\t";
			myfile<<tags[i];
		}

		myfile<<endl;
	}
	myfile.close();

}


/*******************************************************************************
* Training
******************************************************************************/
/*dotrain : this method train the crf model with L-bfgs based on training data set and 
* development data set then provides model in mentioned path
*/

void tagger:: dotrain(string &pattern, string &traindataset,string &devdataset,string &model) {

	//set up options for trianing model based on CRF and L1,L2 regularization penalty 
	//and compact model
	opt_t opt = opt_defaults;
	mdl_t *mdl = mdl_new(rdr_new(false));
	mdl->opt = &opt;
	mdl->opt->mode=0;
	mdl->opt->compact=false;
	mdl->opt->rho1=0.25;
	// mdl->opt->rho2=0.0001;
	mdl->opt->model=NULL;
	mdl->type=2;
	mdl->opt->maxiter=40;    



	mdl->opt->pattern=STR(pattern);
	// Load the pattern file. This will unlock the database if previously
	// locked by loading a model.
	if (mdl->opt->pattern != NULL) {
		info("* Load patterns\n");
		FILE *fpat = fopen(mdl->opt->pattern, "r");
		if (fpat == NULL)
			pfatal("cannot open pattern file");
		rdr_loadpat(mdl->reader, fpat);
		fclose(fpat);
		qrk_lock(mdl->reader->obs, false);
	}
	// Load the training data. When this is done we lock the quarks as we
	// don't want to put in the model, informations present only in the
	// devlopment set.
	info("* Load training data\n");
	FILE *ftrain = stdin;
	mdl->opt->input=STR(traindataset);
	if (mdl->opt->input != NULL) {
		ftrain = fopen(mdl->opt->input, "r");
		if (ftrain == NULL)
			pfatal("cannot open input data file");
	}
	mdl->train = rdr_readdat(mdl->reader, ftrain, true);
	if (mdl->opt->input != NULL)
		fclose(ftrain);
	qrk_lock(mdl->reader->lbl, true);
	qrk_lock(mdl->reader->obs, true);
	if (mdl->train == NULL || mdl->train->nseq == 0)
		fatal("no train data loaded");
	// If present, load the development set in the model. If not specified,
	// the training dataset will be used instead.
	mdl->opt->devel=STR(devdataset);
	if (mdl->opt->devel != NULL) {
		info("* Load development data\n");
		FILE *fdev = fopen(mdl->opt->devel, "r");
		if (fdev == NULL)
			pfatal("cannot open development file");
		mdl->devel = rdr_readdat(mdl->reader, fdev, true);
		fclose(fdev);
	}
	// Initialize the model. If a previous model was loaded, this will be
	// just a resync, else the model structure will be created.
	if (mdl->theta == NULL)
		info("* Initialize the model\n");
	else
		info("* Resync the model\n");
	mdl_sync(mdl);
	// Display some statistics as we all love this.
	info("* Summary\n");

	info("    nb train:    %d\n", mdl->train->nseq);
	if (mdl->devel != NULL)
		info("    nb devel:    %d\n", mdl->devel->nseq);
	info("    nb labels:   %d\n", mdl->nlbl);
	info("    nb blocks:   %d\n", mdl->nobs);
	info("    nb features: %d\n", mdl->nftr);
	// And train the model...
	info("* Train the model with %s\n", mdl->opt->algo);
	uit_setup(mdl);
	trn_lbfgs(mdl);
	uit_cleanup(mdl);
	// If requested compact the model.

	if (mdl->opt->compact) {
		const uint64_t O = mdl->nobs;
		const uint64_t F = mdl->nftr;
		info("* Compacting the model\n");
		mdl_compact(mdl);
		info("    %d observations removed\n", O - mdl->nobs);
		info("    %d features removed\n", F - mdl->nftr);
	}
	// And save the trained model
	info("* Save the model\n");
	FILE *fmodel = stdout;
	mdl->opt->output=STR(model);
	if (mdl->opt->output != NULL) {
		fmodel = fopen(mdl->opt->output, "w");
		if (fmodel == NULL)
			pfatal("cannot open output model");
	}
	mdl_save(mdl, fmodel);
	if (mdl->opt->output != NULL)
		fclose(fmodel);
	info("* Done\n");
}


/*******************************************************************************
* Labeling
******************************************************************************/
/*dolabel: This method uses the model which comes from dotrain method and label test data 
* then calculates performance measures like Recall, Precision, F-measure for each tag  
*/
void tagger:: dolabel(string &testset,string &result,string &model) {
	opt_t opt = opt_defaults;
	mdl_t *mdl = mdl_new(rdr_new(false));
	mdl->opt = &opt;
	mdl->opt->mode=1;
	mdl->opt->check=true;
	mdl->opt->outsc=true;

	// First, load the model provided by the user. This is mandatory to
	// label new datas ;-)
	mdl->opt->model=STR(model);
	if (mdl->opt->model == NULL)
		fatal("you must specify a model");
	info("* Load model\n");
	FILE *fmodel = fopen(mdl->opt->model, "r");
	if (fmodel == NULL)
		pfatal("cannot open input model file");
	mdl_load(mdl, fmodel);

	// Open input and output files
	mdl->opt->input=STR(testset);
	mdl->opt->output=STR(result);
	FILE *fin = stdin;
	FILE *fout = stdout;
	if (mdl->opt->input != NULL) {
		fin = fopen(mdl->opt->input, "r");
		if (fin == NULL)
			pfatal("cannot open input data file");
	}
	if (mdl->opt->output != NULL) {
		fout = fopen(mdl->opt->output, "w");
		if (fout == NULL)
			pfatal("cannot open output data file");
	}
	// Do the labelling
	info("* Label sequences\n");

	tag_label(mdl, fin, fout);
	info("* Done\n");
	// And close files
	if (mdl->opt->input != NULL)
		fclose(fin);
	if (mdl->opt->output != NULL)
		fclose(fout);

}


