/* 
 * File:   main.cpp
 * Author: fnoorala
 *
 * Created on July 16, 2013, 10:37 AM
 */

#define STR(x) (char*)x.c_str()
#define USTR(x) (uchar*)x.c_str()
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>
#include <utility>
#include <stdio.h>
#include <string.h>
#include <cstdio>  
#include <time.h>
#include "tagger.h"
clock_t startm, stopm;
#define START if ( (startm = clock()) == -1) {printf("Error calling clock");exit(1);}
#define STOP if ( (stopm = clock()) == -1) {printf("Error calling clock");exit(1);}
#define PRINTTIME printf( "%6.3f seconds used by the processor.", ((double)stopm-startm)/CLOCKS_PER_SEC);

using namespace std;


#include "extractFeature.h"

#include "conversion.h"
extern "C" {
#include <ctype.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "decoder.h"
#include "model.h"
#include "options.h"
#include "progress.h"
#include "quark.h"
#include "reader.h"
#include "sequence.h"
#include "tools.h"
#include "trainers.h"
#include "wapiti.h"

}
 
int main () {
    
     START;
        tagger tag;
        extractFeature ex;
                     
  string path="/opt/ParSem/users/Farhad/parSem/ntagger/src/oct27.splits/sampleTolabel"  ;     
        
    string line;
    vector<string> words;
    ifstream myfile;
    myfile.open(path.c_str());
    if (myfile.is_open())
  {
    while ( myfile.good() )
        {
            getline (myfile,line);
            string arr[1];
            int ii = 0;
            stringstream ssin(line);
           while (ssin.good() && ii < 1){
                ssin >> arr[ii];
              ++ii;
            }
            //cout<<arr[0]<<endl;
            words.push_back(arr[0]);
                          
            }
         myfile.close();
  }
 else cout << "Unable to open file"; 
    

  

  vector<string> ptags=tag.dotag(words);
       
        
      STOP;
      PRINTTIME;

  return 0;
}














