#include "parsbin.h"

#define MAX_NB_PATH 100
extern char g_environ_var[MAX_NB_PATH+1][1000];
extern VECTA <char*> g_environ_var_inconnues;
extern int g_nbvarenv;
void lance_erreur(char* message);
extern "C" {
   void NormalisationNomFichier(char* nomFichier,char* tampon);
}

void GrmRead(ifstream& grm,char* buf,int max);
void GrmReadLine(ifstream& grm,char* buf,int max);
void GrmTrim(ifstream& grm,uchar* buf,int max,char norm);

char CurrentLoadGrmFiles(char* grmname,
                         char* ntm,
                         char* hmm,
                         char* grammar, 
                         char* tokenize,
                         char* lookup,
                         lookup_strategy& strategy,
                         char* lookup_flags,
                         char& conversion,
                         VECTA<char*>& parameters,
                         vector<string>& g_definition_var) {
   
   char bufread[1000];
   hmm[0]=0;
   ntm[0]=0;
   grammar[0]=0;
   char crypte=NON;
   
   int ik;    
   for (ik=0;ik<g_environ_var_inconnues.dernier;ik++) {
      if (g_environ_var_inconnues[ik]!=NULL)
         libere(g_environ_var_inconnues[ik]);
   }
   
   g_environ_var_inconnues.raz();
   
   ifstream grm(grmname,modeOuverture);
   
   if (grm.fail()) {
      sprintf(bufread,"Cannot open: %s",grmname);
      lance_erreur(strdup(bufread));
   }
   
   
   for (ik=0;ik<MAX_NB_PATH;ik++)
      g_environ_var[ik][0]=0;    
   g_nbvarenv=0;
   
   while (!grm.eof()) {
      
      GrmTrim(grm,(uchar*)bufread,999,NON);
      
      if (!*bufread)
         break;
      
      if (!strcmp(bufread,"conversion:")) {
         GrmTrim(grm,(uchar*)bufread,999,OUI);            
         if (!strcmp(bufread,"utf82ascii"))
            conversion=OUI;
         continue;
      }
      
      
      //First line is grammar
      
      //Second line is ntm
      
      //thrid line is hmm
      
      //following lines are parameter files            
      if (!strcmp(bufread,"path:")) {
         if (g_nbvarenv>=MAX_NB_PATH) {
            sprintf(g_environ_var[g_nbvarenv],"Too many environment variable defined (Max is %d)",MAX_NB_PATH);
            lance_erreur(strdup(g_environ_var[g_nbvarenv]));
         }
         
         GrmTrim(grm,(uchar*)g_environ_var[g_nbvarenv],999,2);
         if (g_environ_var[g_nbvarenv][0]!=0) {
            if (strlen(g_environ_var[g_nbvarenv])>=999) {
               strcpy(g_environ_var[g_nbvarenv],"Environment variable too long");
               lance_erreur(strdup(g_environ_var[g_nbvarenv]));
            }
            
#ifdef WIN32
            _putenv(g_environ_var[g_nbvarenv]);
#else
            putenv(g_environ_var[g_nbvarenv]);
#endif 
            g_nbvarenv++;
         }
         continue;
      }
      
      //lecture de variable de definition
      if (!strcmp(bufread,"variable:")) {
         
         GrmTrim(grm,(uchar*)bufread,999,OUI);            
         if (bufread[0]!=0) {
            if (strlen(bufread)>999) {
               strcpy(bufread,"Variable name is too long");
               lance_erreur(strdup(bufread));
            }
            g_definition_var.push_back(bufread);
         }
         continue;
      }
      
      //lecture de la strategy de lookup
      if (!strcmp(bufread,"strategy:")) {
         GrmTrim(grm,(uchar*)bufread,999,OUI);            
         if (bufread[0]!=0) {
            if (strlen(bufread)>999) {
               strcpy(bufread,"Variable name is too long");
               lance_erreur(strdup(bufread));
            }
            if (!strcmp(bufread,"DEPTH"))
               strategy=DEPTH;
            else
               if (!strcmp(bufread,"BREADTH"))
                  strategy=BREADTH;
            else {
               strcpy(bufread,"Unknown value for STRATEGY: should be DEPTH or BREADTH");
               lance_erreur(strdup(bufread));
            }
         }
         continue;
      }
      
      if (!strcmp(bufread,"flags:")) {
         GrmTrim(grm,(uchar*)bufread,999,OUI);
         if (bufread[0]!=0) {
            if (strlen(bufread)>999) {
               strcpy(bufread,"Flag string too long");
               lance_erreur(strdup(bufread));
            }
            strcpy(lookup_flags,bufread);            
         }
         continue;
      }
      
      if (!strcmp(bufread,"lookup:")) {
         GrmTrim(grm,(uchar*)bufread,999,OUI);
         if (bufread[0]!=0) {
            if (strlen(bufread)>999) {
               strcpy(bufread,"Pathname for NTM too long");
               lance_erreur(strdup(bufread));
            }
            strcpy(lookup,bufread);            
         }
         continue;
      }
      
      if (!strcmp(bufread,"tokenizer:")) {
         GrmTrim(grm,(uchar*)bufread,999,OUI);
         if (bufread[0]!=0) {
            if (strlen(bufread)>999) {
               strcpy(bufread,"Pathname for NTM too long");
               lance_erreur(strdup(bufread));
            }
            strcpy(tokenize,bufread);            
         }
         continue;
      }
      
      if (!strcmp(bufread,"ntm:")) {
         GrmTrim(grm,(uchar*)bufread,999,OUI);
         if (bufread[0]!=0) {
             if (strlen(bufread)>999) {
                strcpy(bufread,"Pathname for NTM too long");
               lance_erreur(strdup(bufread));
            }
            strcpy(ntm,bufread);            
         }
         continue;
      }
      
      if (!strcmp(bufread,"hmm:")) {
          GrmTrim(grm,(uchar*)bufread,999,OUI);
          if (bufread[0]!=0) {
            if (strlen(bufread)>999) {
               strcpy(bufread,"Pathname for HMM too long");
               lance_erreur(strdup(bufread));
            }       
            strcpy(hmm,bufread);                            
         }
         continue;
      }
      
      if (!strcmp(bufread,"grammar:")) {
         GrmTrim(grm,(uchar*)bufread,999,OUI);
         if (bufread[0]!=0) {
            if (strlen(bufread)>999) {
               strcpy(bufread,"Pathname for grammar too long");
               lance_erreur(strdup(bufread));
            }
            strcpy(grammar,bufread);
         }
         continue;
      }
      
      if (!strcmp(bufread,"gram:")) {
         crypte=OUI;
         GrmTrim(grm,(uchar*)bufread,999,OUI);
         if (bufread[0]!=0) {
            if (strlen(bufread)>999) {
               strcpy(bufread,"Pathname for grammar too long");
               lance_erreur(strdup(bufread));
            }
           strcpy(grammar,bufread);
         }
         continue;
      }
      
      if (!strcmp(bufread,"parameter:")) {
         GrmTrim(grm,(uchar*)bufread,999,OUI);
         if (bufread[0]!=0)
            parameters.push_back(DupliqueChaine(bufread));
         continue;
      }
      
      char err[100];
      sprintf(err,"Unknown GRAMMAR file field:\"%s\"",bufread);
      lance_erreur(err);
   }
   
   return crypte;
}
