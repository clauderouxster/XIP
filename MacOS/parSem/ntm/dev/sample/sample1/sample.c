#include <stdarg.h>
#ifdef _WINDOWS
#include <stdlib.h>
#endif

#include "ntm.h"


/* This is an example of a client program that uses the NTM API
   to read tokens from an input text and to lookup the tokens
   in lexical FSTs
   Please read the API documentation
***********************************************************/

int main(int argn, char **argv) {

    static NtmContext ct;
    long initpos = 0, strategy_ind=0;
    NtmToken *token=NULL, *t=NULL;
    char sout[5002];
    FILE *textStream;

    // Initialize the CSFM functions
    ntmCfsmInit();

    // Initialize the NTM context ct
    ntmInit(&ct);

    // set the indice option (otherwise ntm won't get the token indices)
    ntmSetPrintIndices(&ct, TRUE);

    // Now, load the (normalizing and lexical) FSTs that will be used for analysis
    // in the NTM context ct
    // assuming the FSTs are declared in the NTM script file 'ntmscript'
    ntmLoadScript(&ct, "ntmscript");

    // Check if there was an error while reading the NTM script
    if (ntmError(&ct)) {
        fprintf(stderr, "\n%s\n\n", ntmMessage(&ct));
        exit(1);
    }        

    // Now open file "input.txt"
    textStream = fopen( "input.txt", "r");
    if (textStream == NULL) {
	 fprintf(stderr, "Unable to read text from '%s'\n\n", "input.txt");
         exit(1);
    }

    // and set its stream as the input (text) object in the NTM context ct
    ntmSetInputTextStream(&ct, textStream);


    // Start reading and analyzing tokens from the input text

    token = ntmReadTokens(&ct);
    // while no error and the end of text is not reached
    while ( !ntmError(&ct) && !ntmIsEofToken(token) ) {
      // if there is a warning, print the message on stderr
      if (ntmWarning(&ct)) {
	fprintf(stderr, "\n####%s\n", ntmMessage(&ct));
	ntmResetWarning(&ct);
      }
      // print the token analysis on the string 'sout'
      ntmPrintTokens(&ct, token, sout, 5000);
      // and then print the string 'sout' on stdout
      fputs(sout, stdout);
      
      // read next token
      token = ntmReadTokens(&ct);
    }

    // if an error occurred, print error the message
    if (ntmError(&ct)) {
      fprintf(stderr, "\n%s\n\n", ntmMessage(&ct));
    }


    // Free the allocated input object
    ntmClearInputObject(&ct);

    fclose(textStream);

    ntmCfsmRelease();

    return 0;
}




