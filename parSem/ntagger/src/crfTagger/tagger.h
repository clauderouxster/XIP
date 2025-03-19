/* 
 * File:   tagger.h
 * Author: fnoorala
 *
 * Created on August 6, 2013, 1:34 PM
 */

#ifndef TAGGER_H
#define	TAGGER_H

#include "extractFeature.h"

class tagger {
public:
	int error;
	string modelPath;
	extractFeature ext;
	mdl_t *mdl;
	opt_t opt;

    tagger(string mpath,string brown,string mk);
    virtual ~tagger();
    void  dotrain(string &pattern,string &,string &,string &);

    bool dotag(vector<string>& wordSequence,vector<string>& predictedTag);
    data_t* fill_rawStruct(const vector< vector <string> > &);
    bool readfile(const string &, vector<string> &, vector<string>& );
    void crfInputFile (const string &,string &);
    void dolabel(string &testset,string &result,string &model);
    
    


  
private:

};

#endif	/* TAGGER_H */

