//
// Copyright(c) 1999-2000 by Fuji Xerox Co., Ltd.  All right reverved.
// Created by Makoto Iizuka
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>


#include "ProcessorFactory.hxx"
#include "Processor.hxx"
#include "Analyzer.hxx"
#include "InputBuilder.hxx"
#include "OutputBuilder.hxx"
#include "OutputWriterFactory.hxx"
#include "OutputWriter.hxx"
#include "MemoryWriter.hxx"
#include "OutputIterator.hxx"

void wakachi(Analyzer* azr, char* filename);	//wakachi-gaki
void epr(Analyzer* azr, char* filename);		//extended plain result

/*
#define OP 0	//0: output to file
				//1: output to stdout with buffering result in memory
*/

#define PRF 1	//Plain Resutl Format
#define WOF 2	//With Offset Format
#define SWF 4	//Single Word Format
#define CWF 8	//Compound Word Format
#define LEX 16	//Lexical
#define SUR 32	//Surface

bool checkParam(int argc, char* argv[]);
int  getFlag(char* argv);

int
main(int argc, char* argv[]) {
  int nFlag = 0;

  // argument value of filename validate check
  if (!checkParam(argc, argv)) return -1;

  // flag check
  if (!(nFlag = getFlag(argv[6]))) return -1;

  ProcessorFactory procFactory;
  Processor* proc = procFactory.createProcessor(argv[3], argv[4]);
  Analyzer* azr = proc->createAnalyzer(argv[5]);

  azr->getInputBuilder()->setFile(argv[1]);
  OutputWriterFactory writerFactory;

/*
  // output to file
#if OP==0  
*/

  //case of wakachi-gaki
  if (strcmp(argv[6], "-wakachi") == 0) {
	wakachi(azr, argv[2]);	//wakachi-gaki

	delete azr;
	delete proc;
	cerr << "... Completed." << endl;
	return 0;

  } else if (strcmp(argv[6],"-epr") == 0) {		// modified by Makoto Iizuka '2001/07/11
	epr(azr, argv[2]);	//extended plain result

	delete azr;
	delete proc;
	cerr << "... Completed." << endl;
	return 0;
  }


  OutputWriter* writer = writerFactory.createFileWriter(argv[2]);

  if (nFlag & PRF)
	azr->getOutputBuilder()->setResultWriter(writer);
  if (nFlag & WOF)
	azr->getOutputBuilder()->setOffsetFlag(true);
  if (nFlag & SWF)
	azr->getOutputBuilder()->setSingleWordWriter(writer);
  if (nFlag & CWF)
	azr->getOutputBuilder()->setCompoundWriter(writer);
  if (nFlag & SUR)
	azr->getOutputBuilder()->setSurfaceFlag(true);
  if (nFlag & LEX)
	azr->getOutputBuilder()->setLexicalFlag(true);

  //azr->getOutputBuilder()->setAsIsWriter(writer);
  azr->getOutputBuilder()->setSorting(true);

  cerr << "Analyzing..." << endl;
  azr->run();
  cerr << "... Done" << endl;
  delete writer;

/*
  // output to stdout
#elif OP==1
  MemoryWriter* memory = writerFactory.createMemoryWriter();

  if (nFlag & PRF)
	azr->getOutputBuilder()->setResultWriter(memory);
  if (nFlag & SWF)
	azr->getOutputBuilder()->setSingleWordWriter(memory);
  if (nFlag & CWF)
	azr->getOutputBuilder()->setCompoundWriter(memory);
  if (nFlag & SUR)
	azr->getOutputBuilder()->setSurfaceFlag(true);
  if (nFlag & LEX)
	azr->getOutputBuilder()->setLexicalFlag(true);

  azr->run();

  OutputIterator* iter = memory->createOutputIterator();
  cout << memory->getSize() << endl;
  iter->first();
  while (!iter->isDone()) {
      if (nFlag & WOF) cout << "(" << iter->getStartIndex() 
							<< " " << iter->getEndIndex() << ") "; 
	  cout << iter->getString() << endl;
      iter->next();
  }
  delete iter;
  delete memory;

#endif
*/

  delete azr;
  delete proc;
  cerr << "... Completed." << endl;
  return 0;
}


bool
checkParam(int argc, char* argv[]) {

	if( argc < 7 ){
		printf("usage : %s arg1 arg2 arg3 arg4 arg5 arg6\n",argv[0]);
		printf("-----\nArguments :\n");		
		printf("\targ1   : Input-File ( EUC )\n");
		printf("\targ2   : Output-File ( EUC )\n");
		printf("\targ3   : Taf4-File \n");
		printf("\targ4   : Lsc-File \n");
		printf("\targ5   : TOT-File ( EUC )\n");
		printf("\targ6   : Segmentation-Type { -pr | -po | -pos | -pol | -wakachi | -all | -sw | -cw | -epr }\n");
		return false;
	}

	FILE *fp;
	const char* fType[] = {"", "Input", "", "Taf4", "Lsc", "TOT", ""};

	for(int i = 0; i < argc - 1; i++) {
		if( strlen((const char *)fType[i]) ) {
			if ( (fp = fopen((const char *)argv[i],"r")) == NULL ) {
				//printf("%s file is not found.\n", fType[i]);
				cerr << fType[i] << " file is not found." << endl;
				return false;
			}
			fclose(fp);
		}
	}
	return true;
}


int
getFlag(char* argv) {

	int nRet = 0;
	const char* cFlag[] = {"-pr", "-po", "-sw", "-cw", "-all", 
						"-pos", 
						"-pol", 
						"-wakachi", 
						"-epr"};
	const int nFlag[] = {PRF, (PRF | WOF), SWF, CWF, (SWF | CWF), 
						(PRF | WOF | SUR), 
						(PRF | WOF | LEX),
						(PRF | SUR),
						PRF
	};

	for(int i = 0; i < 9; i++)
		if( strcmp((const char *)argv,(const char *) cFlag[i]) == 0 ) {
			nRet = nFlag[i];
			return nRet;
		}
	
	//printf("%s is not Segmentation-Type.\n", argv);
	cerr << argv << " is not Segmentation-Type." << endl;
	return nRet;
}