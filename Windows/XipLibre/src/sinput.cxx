/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2003 - 2007 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the XIP library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : XIP
   Version    : 9.60
   filename   : simput.cxx
   Date       : 10/01/1999
   Purpose    : pattern extraction methods to extract information from strings into variables.
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "parsbin.h"
#ifdef UNIX
#include <stdarg.h>
#endif

TansSinput::TansSinput(char immediat,char copy, char decimal,char* instring,char* stopstring) {
    DejaInitialisee=1;
    Init(immediat,copy,decimal,instring,stopstring);
}


void TansSinput::GetError(char* msg,char** start) {
	strcpy(msg,ErrorMessage);
	*start=StartOfString;
}



char TansSinput::isNumber(char* ch,int j,char DecimalCharacter)
{
    if (isdigit(ch[j]))
        return 1;
    
    if (ch[j]=='-' || (DecimalCharacter && ch[j]==DecimalCharacter))
        if (isdigit(ch[j+1]))
            return 1;    

    return 0;
}


int TansSinput::TestAlphaString(unsigned char charac) {
    if (StopString[0])
        if (strchr(StopString,charac))
            return 0;

    if (InString[0])
        if (strchr(InString,charac))
            return 1;

    return isalnum(charac);
}




void TansSinput::Init(char immediat,char copy, char decimal,char* instring,char* stopstring) {
	InitialesDonnees.iImmediateMatching=immediat;
	strcpy(InitialesDonnees.iStopString,stopstring);
	strcpy(InitialesDonnees.iInString,instring);
	InitialesDonnees.decimal=decimal;
	InitialesDonnees.copy=copy;
	DejaInitialisee=1;
}




char TansSinput::MemoryTest(int posvar) {
    int length1,length2,test;
    
    int testmemoire=d.declencheChaine[posvar];
    if (testmemoire!=-1) {
        testmemoire=d.memchaine[testmemoire];
        if (d.copy) {
            length1=strlen(d.string[posvar]);
            length2=strlen(d.string[testmemoire]);
        }
        else {
            length1=*(d.longueur[posvar]);
            length2=*(d.longueur[testmemoire]);
        }
        if (length1==length2) {
            if (d.copy)
                test=memcmp(d.string[posvar],d.string[testmemoire],length1);
            else
                test=memcmp(*(d.chaine[posvar]),*(d.chaine[testmemoire]),length1);
            
            if (!test)
                return -1;
            else
                return 0;
        }
        else {
            if (d.copy)
                test=memcmp(d.string[posvar],d.string[testmemoire],length2);
            else
                test=memcmp(*(d.chaine[posvar]),*(d.chaine[testmemoire]),length2);
            
            if (!test)
                return length2;
            else
                return 0;
        }
    }
    
    return -1;
}

//This function applies on string the expression defined in the d.description field
char TansSinput::FindString(char* ch,char** start,char** afterstart,char** endOfstring,int debut,int IM) {
    int i=0;
    char flot;
    int posvar,j,longueur=-1;
    char c,empty=1,alnu=0,chiffre,variation,testChar,fin;
    int length;

    
    char* ne;
    int testmemoire,nextI;
    char* startCh;
    int comma;

    if (!debut) {
        *start=NULL;
	*afterstart=NULL;
    }

    for (i=debut;i<= d.isc;i++) {
        //If it's -1, then we have to evaluate a int, or a float or a string...
        if (!d.lsouschaine[i])
            continue;
        if (d.lsouschaine[i]<0) {
            posvar=(d.lsouschaine[i]+1)*-1;
            empty=1;alnu=0;variation=0;
            switch(d.souschaine[i][0]) {
            case 'f':
                j=0;
                if (!i && !IM) {
                    while (ch[j] && !isNumber(ch,j,d.DecimalCharacter)) j++;
                    ch+=j;
                }
                
                j=0;
                comma=-1;
                while (ch[j] && isNumber(ch,j,d.DecimalCharacter)) {
                    if (ch[j]==d.DecimalCharacter)
                        comma=j;
                    j++;
                }
                if (!j) {
                    if (d.erreur && d.depthError < i) {
                        sprintf(ErrorMessage,"No float found at:%d",posvar);
                        StartOfString=*start;
                        d.depthError=i;
                    }
                    return 0;
                }
                if (*start==NULL)
                    *start=ch;
                c=ch[j];ch[j]=0;
                if (comma!=-1)
                    ch[comma]='.';
                *(d.flottant[posvar])=(float)atof(ch);
                if (comma!=-1)
                    ch[comma]=d.DecimalCharacter;
                ch[j]=c;
                ch+=j;
                if (*afterstart==NULL)
                    *afterstart=ch;
                
                *endOfstring=ch;
                testmemoire=d.declencheFlottant[posvar];
                if (testmemoire!=-1)
                    if (*(d.flottant[posvar]) != *(d.flottant[d.memflottant[testmemoire]])) {
                        if (d.erreur && d.depthError < i) {
                            sprintf(ErrorMessage,"Memory float error at:%d",posvar);
                            StartOfString=*start;
                            d.depthError=i;
                        }
                        return 0;             
                    }
                break;
            case 'd':
                j=0;
                if (!i && !IM) {
                    while (ch[j] && !isNumber(ch,j,0)) j++;
                    ch+=j;
                }
                j=0;
                while (ch[j] && isNumber(ch,j,0)) j++;
                if (!j) {
                    if (d.erreur && d.depthError < i) {
                        sprintf(ErrorMessage,"No integer found at:%d",posvar);
                        StartOfString=*start;
                        d.depthError=i;
                    }
                    return 0;
                }
                if (*start==NULL)
                    *start=ch;
                c=ch[j];ch[j]=0;
                *(d.entier[posvar])=atoi(ch);
                ch[j]=c;
                ch+=j;
                
                if (*afterstart==NULL)
                    *afterstart=ch;
                
                *endOfstring=ch;
                testmemoire=d.declencheEntier[posvar];
                if (testmemoire!=-1)
                    if (*(d.entier[posvar]) != *(d.entier[d.mementier[testmemoire]])) {
                        if (d.erreur && d.depthError < i) {
                            sprintf(ErrorMessage,"Memory integer error at:%d",posvar);
                            StartOfString=*start;
                            d.depthError=i;
                        }
                        return 0;
                    }
                break;
            case '?':                
                if (*ch==0)
                    return 0;
                if (*start==NULL)
                    *start=ch;
                d.string[posvar][0]=*ch;
                ch++;                
                *endOfstring=ch;
                break;
            case 'b':
                j=0;
                if (*start==NULL)
                    *start=ch;
                while (ch[j]!=0 && isspace(ch[j]))  j++;
                ch+=j;
                *endOfstring=ch;
                break;
            case 'V':
                variation=1;
            case 'S': //This string is only composed of alpha-num chars
                j=0;
                if (!i && !IM) {//we look for the first alpha-num string...
                    while (ch[j]!=0 && !TestAlphaString((unsigned char)ch[j]))  j++;
                    ch+=j;
                }
                alnu=1;         
            case 's': 
                empty=0;		 
            case 'e':
             if (!d.copy)
                 *(d.chaine[posvar])=ch;
             
             if (*start==NULL)
                 *start=ch;
             if (i==d.isc || !d.lsouschaine[i+1]) { //if it's the end...
                 length=strlen(ch);
                 j=0;
                 if (alnu) {//%S style, only alpha-num accepted...
                     while (ch[j]!=0 && TestAlphaString((unsigned char)ch[j]))  j++;
                     if (!j) {
                         if (d.erreur && d.depthError < i) {
                             sprintf(ErrorMessage,"No alpha-num characters to fill up a final %%S (or %%V) at :%d",
                                     posvar);
                             StartOfString=*start;
                             d.depthError=i;
                         }
                         return 0;
                     }
                     length=j;
                 }
                 
                 if (d.copy) {
                     memcpy(d.string[posvar],ch,length);
                     d.string[posvar][length]=0;
                 }
                 else
                     *(d.longueur[posvar])=length;
                 
                 *endOfstring=ch+length;
                 if (*afterstart==NULL)
                     *afterstart=*endOfstring;
                 
                 testmemoire=MemoryTest(posvar);
                 
                 if (!testmemoire) {
                     if (d.erreur && d.depthError < i) {
                         sprintf(ErrorMessage,"Memory error for final string at:%d",posvar);
                         StartOfString=*start;
                         d.depthError=i;
                     }
                     return 0;
                 }
                 
                 if (testmemoire != -1 && !variation) {
                     if (d.erreur && d.depthError < i) {
                         sprintf(ErrorMessage,"Memory error for final string at:%d",posvar);
                         StartOfString=*start;
                         d.depthError=i;
                     }
                     return 0;
                 }
                 
                 if (testmemoire != -1) {
                     if (d.copy)
                         d.string[posvar][testmemoire]=0;
                     else
                         *(d.longueur[posvar])=testmemoire;
                     *endOfstring=ch+testmemoire;
                     if (*afterstart==NULL)
                         *afterstart=*endOfstring;
                 }
                 
                 return 1;
             }
             flot=0;fin=0;
             startCh=ch;chiffre=0;testChar=1;
             while (*ch != 0) {
                 if (d.lsouschaine[i+1]<0) {
                     switch(d.souschaine[i+1][0]) {//let's have a look on the next sequence                     
                     case 'f': flot=d.DecimalCharacter;
                     case 'd':
                         chiffre=1;
                         j=0;
                         while (ch[j] && !isNumber(ch,j,flot)) j++;
                         if (!ch[j]) {
                             if (d.erreur && d.depthError < i) {
                                 sprintf(ErrorMessage,"No integer following the string at:%d",posvar);
                                 StartOfString=*start;
                                 d.depthError=i;
                             }
                             return 0; //there's no number in the rest of the sequence
                         }
                         ch+=j; //we are on the first digit
                         length=ch-startCh;
                         if (d.copy) {
                             memcpy(d.string[posvar],startCh,length);
                             d.string[posvar][length]=0;
                         }
                         else
                             *(d.longueur[posvar])=length;						 
                         break;                                 
                     default:
                         if (!alnu) {
                             strcpy(ErrorMessage,"a sequence %s%s is impossible");
                             StartOfString=*start;
                             return 0;
                         }
                         
                         
                         if (ch != startCh) {//We try with one character less for %S 
                             if (variation) {
                                 ch--; //so one more for the %s or %e following
                                 if	(ch==startCh) {//this sort of string cannot be empty
                                     if (d.erreur && d.depthError < i) {
                                         sprintf(ErrorMessage,"Variation impossible at :%d",posvar);
                                         StartOfString=*start;
                                         d.depthError=i;
                                     }
                                     return 0;
                                 }
                                 if (d.copy)
                                     d.string[posvar][strlen(d.string[posvar])-1]=0;
                                 else
                                     --*(d.longueur[posvar]);
                             }
                             else {
                                 if (d.erreur && d.depthError < i) {
                                     d.depthError=i;
                                     sprintf(ErrorMessage,"No suitable %%S string found at :%d",posvar);
                                     StartOfString=*start;
                                 }
                                 return 0;
                             }
                         }
                         else {
                             j=0; //we are in the case where we are looking for a string only composed
                             while (startCh[j]!=0 && TestAlphaString((unsigned char)startCh[j]))  j++; //of alpha-num
                             if (!j) {
                                 if (d.erreur && d.depthError < i) {
                                     d.depthError=i;
                                     sprintf(ErrorMessage,"No suitable %%S string found at :%d",posvar);
                                     StartOfString=*start;
                                 }
                                 return 0;
                             }
                             ch=startCh+j; //we are on the first non alpha-num chararcter...
                             if (d.copy) {
                                 memcpy(d.string[posvar],startCh,j);
                                 d.string[posvar][j]=0;
                             }
                             else
                                 *(d.longueur[posvar])=j;
                             testChar=0; //we don't need to test the content of this string anymore..
                         }
                     }
                     nextI=i+1;
                     if (nextI >= d.isc)
                         fin=1;
                 }
                 else {//We have to find a specific sequence of characters in the rest of the string
                     c=d.souschaine[i+1][d.lsouschaine[i+1]];
                     d.souschaine[i+1][d.lsouschaine[i+1]]=0;
                     ne=strstr(ch,d.souschaine[i+1]);
                     
                     if (ne==NULL) { //we didn't find this string
                         if (d.erreur && d.depthError < i) {
                             d.depthError=i;
                             sprintf(ErrorMessage,"%s has not been found",d.souschaine[i+1]);
                             StartOfString=*start;
                         }
                         d.souschaine[i+1][d.lsouschaine[i+1]]=c;
                         return 0;
                     }
                     d.souschaine[i+1][d.lsouschaine[i+1]]=c;
                     length=ne-startCh;
                     if (d.copy) {
                         memcpy(d.string[posvar],startCh,length);
                         d.string[posvar][length]=0;
                     }
                     else
                         *(d.longueur[posvar])= length;
                     ch=ne+d.lsouschaine[i+1];
                     nextI=i+2;
                     if (nextI > d.isc)
                         fin=1;
                 }
                 
                 if (d.copy)
                     length=strlen(d.string[posvar]);
                 else
                     length=*(d.longueur[posvar]);
				 
                 j=0;
                 if (alnu && testChar) {//%S style, only alpha-num accepted...
                     while (j<length && TestAlphaString((unsigned char)startCh[j]))  j++;
                     if (j!=length) {
                         if (d.erreur && d.depthError < i) {
                             d.depthError=i;
                             sprintf(ErrorMessage,"Illegal character in a %%S string at:%d",posvar);
                             StartOfString=*start;
                         }
                         if (*afterstart==NULL)
                             *afterstart=startCh+j; //we will try with the next possible string for a %S
                         return 0;
                     }
                 }
	

                 *endOfstring=ch;
                 
                 if (*afterstart==NULL)
                     if (alnu)
                         *afterstart=ch;
                     else
                         *afterstart=startCh+1;
                 
                 if (FindString(ch,start,afterstart,endOfstring,nextI,IM)) {
                     fin=1;
                     break;
                 }
                 if (chiffre) { //we have to consume this number (to avoid a loop)
                     j=0;
                     while(ch[j] && isNumber(ch,j,flot)) j++;
                     ch+=j;
                 }
             }
             
             if (!*ch && !fin)
                 return 0;
			 

             if (MemoryTest(posvar) != -1) {
                 if (d.erreur && d.depthError < i) {
                     d.depthError=i;
                     sprintf(ErrorMessage,"Memory string error at :%d", posvar);
                     StartOfString=*start;
                 }
                 return 0;
             }
             
             if (d.copy)
                 length=strlen(d.string[posvar]);
             else
                 length=*(d.longueur[posvar]);
             
             if (!empty && !length) {
                 if (d.erreur && d.depthError < i) {
                     d.depthError=i;
                     sprintf(ErrorMessage,"String empty at:%d", posvar);
                     StartOfString=*start;
                 }
                 return 0;                    
             }
             
             return 1;
            }
        }
        else {
            while (*ch != 0) {
                if (!i && !IM) { //If we are looking for the first substring in a non immediate way
                    c=d.souschaine[i][d.lsouschaine[i]];
                    d.souschaine[i][d.lsouschaine[i]]=0;
                    ne=strstr(ch,d.souschaine[i]);
                    
                    if (ne==NULL) {
                        if (d.erreur && d.depthError < i) {
                            d.depthError=i;
                            sprintf(ErrorMessage,"%s has not been found",d.souschaine[i]);
                            StartOfString=*start;
                        }
                        d.souschaine[i][d.lsouschaine[i]]=c;
                        return 0;
                    }
                    
                    d.souschaine[i][d.lsouschaine[i]]=c;
                }
                else {
                    if (memcmp(ch,d.souschaine[i],d.lsouschaine[i])) {
                        if (d.erreur && d.depthError < i) {
                            d.depthError=i;
                            c=d.souschaine[i][d.lsouschaine[i]];
                            d.souschaine[i][d.lsouschaine[i]]=0;
                            sprintf(ErrorMessage,"%s has not been found",d.souschaine[i]);
                            StartOfString=*start;
                            d.souschaine[i][d.lsouschaine[i]]=c;
                        }
                        return 0;
                    }
                    ne=ch;
                }
                
                ch=ne+d.lsouschaine[i];
                *endOfstring=ch;
                if (*start==NULL)
                    *start=ne;
                
                if (*afterstart==NULL)
                    *afterstart=ch;

                if (FindString(ch,start,afterstart,endOfstring,i+1,IM))
                    return 1;
                return 0;
            }
        }
    }
    
    if (i<= d.isc)
        return 0;
    return 1;
} 


//d.description contains the following little things :
// %d stands for a int
// %s is a string without any information of length
// %% for the character %
// A string is composed of a pointer to the beginning of a string plus the length of it...
// Nothing is copied from the original string to a new one. Simply, the inside string is
// isolated with its beginning as a pointer and its length.
// Exemple: here is a d.description = 123%s 
// If we apply it on the following string : result=sinput("abcd 123essay","123%s",string,length);
// the answer will be :
// result = 123essay
// string = essay
// length = 5
// So for each string, a length is required...
//The stop string is initialized with a %(...%) structure. If non is present, the isalnum is used
//in lieu of this structure. This %(...%) has to appear at the beginning of the d.description
//structure.

char* TansSinput::Extract(char* inputstring, const char* descrip,char** endOfstring, ...) {

    if (!DejaInitialisee) {
	InitialesDonnees.iImmediateMatching=0;
	InitialesDonnees.iStopString[0]=0;
	InitialesDonnees.iInString[0]=0;
	InitialesDonnees.decimal='.';
	InitialesDonnees.copy=1;
	DejaInitialisee=1;
    }

    int i,j;
    int l=strlen(descrip);
    int argfound; 
    char* start;
    char* afterstart;
    char* istr;
    int memoire;
    int strng=0,number=0,flot=0;

    va_list args;
    va_start(args,endOfstring);

    if (l > 100) {
        strcpy(ErrorMessage,"Description list to long (no more than 100 characters)");
        StartOfString=NULL;
        return NULL;
    }

    strcpy(d.description,descrip);

    d.ien=0;
    d.ifl=0;
    d.isc=0;
    d.ich=0;
    d.ImmediateMatching=InitialesDonnees.iImmediateMatching;
    d.copy=InitialesDonnees.copy;
 
    strcpy(StopString,InitialesDonnees.iStopString);
    strcpy(InString,InitialesDonnees.iInString);
    d.DecimalCharacter=InitialesDonnees.decimal;
 
 
    for (i=0;i<10;i++) {
        d.mementier[i]=-1;
        d.memflottant[i]=-1;
        d.memchaine[i]=-1;
    }
    for (i=0;i<MAXARGS;i++) {
        d.declencheEntier[i]=-1;
        d.declencheChaine[i]=-1;
        d.declencheFlottant[i]=-1;
    }

    StartOfString=NULL;

    i=0;

    //Immediate matching
    if (d.description[i]=='%' && d.description[i+1]=='<') {
        i+=2;
        d.ImmediateMatching=1;
    }
 
    if (d.description[i]=='%' && d.description[i+1]=='>') {
        i+=2;
        d.ImmediateMatching=0;
    }

    //Cut
    if (d.description[i]=='%' && d.description[i+1]=='C') {
        i+=2;
        d.copy=0;
    }

    //Copy
    if (d.description[i]=='%' && d.description[i+1]=='c') {
        i+=2;
        d.copy=1;
    }
 
    d.erreur=0;

    if (d.description[i]=='%' && d.description[i+1]=='M') {
        i+=2;
        d.erreur=1;
    }

    if (d.description[i]=='%' && d.description[i+1]==',') {
        i+=2;
        d.DecimalCharacter=',';
    }

    if (d.description[i]=='%' && d.description[i+1]=='(') {
        j=i+2;				 
        while (d.description[j] && d.description[j-1] != ')' && d.description[j] !='%')
            j++;
        if (d.description[j] !='%') {
            strcpy(ErrorMessage,"The stop character list doesn't end with %)");
            return NULL;
        }				 
        memcpy(StopString,d.description+i+2,j-2);
        StopString[j-i-2]=0;
        i=j+2;
    }

    if (d.description[i]=='%' && d.description[i+1]=='[') {
        j=i+2;				 
        while (d.description[j] && d.description[j-1] != ']' && d.description[j] !='%')
            j++;
        if (d.description[j] !='%') {
            strcpy(ErrorMessage,"The in character list doesn't end with %]");
            return NULL;
        }				 
        memcpy(InString,d.description+i+2,j-2);
        InString[j-i-2]=0;
        i=j+2;
    }


    d.lsouschaine[d.isc]=0;
    d.souschaine[d.isc]=d.description+i;

    while (i <l ) {
        if (d.description[i]=='%') {
            if (d.description[i+1] == '%' || 
                (d.description[i+1] >= '0' && d.description[i+1] <= '9') ) {
                
                //Beware of the lost character
                //the length is shorten by 1
                if (d.description[i+1] == '%')
                    l--;

                strcpy(d.description+i,d.description+i+1);
                i+=1;
                d.lsouschaine[d.isc]++;
            }
            else {
                argfound=0;
                memoire=-1;
                switch(d.description[i+1]) {
                case 'f':
                    if (d.lsouschaine[d.isc])
                        d.isc++;
                    d.lsouschaine[d.isc]=-1*(d.ifl+1);
                    d.flottant[d.ifl++]=va_arg(args,float*);
                    argfound=1;
                    if (d.description[i+2] >= '0' && d.description[i+2] <= '9') {
                        memoire=d.description[i+2]-'0';
                        if (d.memflottant[memoire]==-1)
                            d.memflottant[memoire]=d.ifl-1;
                        else
                            d.declencheFlottant[d.ifl-1]=memoire;
                    }
                    flot=1;
                    number=0;
                    strng=0;
                    break;
                case 'd' :
                    if (d.lsouschaine[d.isc])
                        d.isc++;
                    d.lsouschaine[d.isc]=-1*(d.ien+1);
                    d.entier[d.ien++]=va_arg(args,int*);
                    argfound=1;
                    if (d.description[i+2] >= '0' && d.description[i+2] <= '9') {
                        memoire=d.description[i+2]-'0';
                        if (d.mementier[memoire]==-1)
                            d.mementier[memoire]=d.ien-1;
                        else
                            d.declencheEntier[d.ien-1]=memoire;
                    }
                    number=1;
                    strng=0;
                    flot=0;
                    break;
                case '?':
                    if (d.lsouschaine[d.isc])
                        d.isc++;
                    d.lsouschaine[d.isc]=-1*(d.ich+1);
                    d.string[d.ich++]=va_arg(args,char*);
                    argfound=1;
                    strng=1;
                    number=0;
                    flot=0;
                    break;
                case 'e':
                case 'S':
                case 'V':
                case 's':
                    if (d.lsouschaine[d.isc])
                        d.isc++;
                    d.lsouschaine[d.isc]=-1*(d.ich+1);
                    if (d.copy) 
                        d.string[d.ich++]=va_arg(args,char*);
                    else {
                        d.chaine[d.ich]=va_arg(args,char**);
                        d.longueur[d.ich++]=va_arg(args,int*);
                    }
                    argfound=1;
                    strng=1;
                    number=0;
                    flot=0;
                    if (d.description[i+2] >= '0' && d.description[i+2] <= '9') {
                        memoire=d.description[i+2]-'0';
                        if (d.memchaine[memoire]==-1)
                            d.memchaine[memoire]=d.ich-1;
                        else
                            d.declencheChaine[d.ich-1]=memoire;
				 }
                    break;
                case 'b':
                    argfound=1;
                    if (d.lsouschaine[d.isc])
                        d.isc++;
                    d.lsouschaine[d.isc]=-1;
                    break;
                default:
                    strcpy(ErrorMessage,"No recognized operator after %");
                    return NULL;
                }
                if (argfound) {
                    d.souschaine[d.isc++]=d.description+i+1;
                    if (memoire==-1)
                        i+=2;
                    else
                        i+=3;
                 
                    d.souschaine[d.isc]=d.description+i;
                    d.lsouschaine[d.isc]=0;
                }
            }
        }
        else {
            i++;
            d.lsouschaine[d.isc]++;
            strng=0;
            number=0;
            flot=0;
        }
    }

    va_end(args);

//Now we scan the input string. The result sent is the first position where the first
//substring has been found...
    istr=inputstring;
    *endOfstring=NULL;
    StartOfString=NULL;
    ErrorMessage[0]=0;
    d.depthError=-1;
    while (istr != NULL && *istr) {	
        if (FindString(istr,&start,&afterstart,endOfstring,0,d.ImmediateMatching))
            return start;
        if (d.ImmediateMatching)
            return NULL;
        istr=afterstart;
    }

    return NULL;
}


#ifdef TESTINPUT
void TestSinput() {

    TansSinput sinput;

    char* theEnd;
    char c1[100];
    char c2[100];
    char c3[100];
    char str[100];
    char e1[100];
    int val1,val2,length;
    c1[0]='1';c2[0]='2';c3[0]='3';e1[0]='4';
    char* end;
    char* string;
    int value;

    char* input="The values are: 13,12,14,12";
    char* result;
    char aChar;
    
    // %d matches a number
    // %f matches a float
    // %? matches any character
    // %s matches any string
    // %e matches any string, can be empty
    // %S matches any strings whose characters belong to the authorized set of characters and do not belong to the unauthorized set of characters
    // %V0 matches a string along another variable

    result=sinput.Extract(input,"%d%?",&end,&value,&aChar); //value=13;aChar=','
    result=sinput.Extract(input,"%d",&end,&value); //value=13
    // '%<' means immediate matching. The beginning of the string has to match the pattern.
    result=sinput.Extract(input,"%<%d",&end,&value); //no result (string does not start with a number)
    result=sinput.Extract(input,"%<The values are: %d",&end,&value); //value=13
    result=sinput.Extract(input,"The %s are",&end,str); //str = "values"
    // '%(*%)' This pattern means that these characters cannot belong to a %S type of string...
    result=sinput.Extract(input,"%(se%)The %S",&end,str); // str= "valu"

    // '%[*%]' This pattern means that these characters can belong to %S type of string
    // Default: alnum characters...
    result=sinput.Extract(input,"%[:%]The values %S",&end,str); // str = "are:"

    // '%C' (cut command)
    //The sub-strings are returned as a pointer to the initial string with the length of this sub-string...
    result=sinput.Extract(input,"%C%(rs%)The %S",&end,&string,&length); //string="values are: 13,12,14,12";length=7
    
    //Memory example: The 0 imposes that val1 and val2 have the same value
    result=sinput.Extract(input,"%d0%s%d0",&end,&val1,str,&val2); //val1=12;ch=",14,";val2=12
    result=sinput.Extract("la dame du chien&chiens+d; ","%S&%S+%S;",&theEnd,c1,c2,c3);//c1="chien";c2="chiens";c2=12
    result=sinput.Extract("la dame du chien&chiens+d; ","%S&%S%b%?%S;",&theEnd,c1,c2,&aChar,c3); //aChar='+'

    //Different kind of memory usage... The memories variable go from 0 to 9... 10 different variables can be managed
    //by the system in the same time...
    //%M is a command to return a description of an error...
    //Below: c1="DOCU";e1=" abcd";c2="le texte";c3=DOCU
    result=sinput.Extract("Ceci est un essai: <DOCU abcd>le texte</DOCU>","%M<%S0%e>%s</%S0>",&theEnd,c1,e1,c2,c3);
    if (!result)
        sinput.GetError(c1,&end);
    
    result=sinput.Extract("DO<DOC abcd>le texte</DOC>","%M%s0>%s</%S0>",&theEnd,c1,e1,c2,c3); //No result
    if (!result)
        sinput.GetError(c1,&end);
    //The variation V0 is used to customize a string according to the other memory string
    //In the example below, c1 and c3 will contain DOC... 
    //c1 is re-customized according to the content of c3...
    result=sinput.Extract("<DOCU abcd>le texte</DOC>","%M<%V0%e>%s</%S0>",&theEnd,c1,e1,c2,c3); 
    if (!result)
        sinput.GetError(c1,&end);
    //Below: c1="DOC";e1=" abcd" ;c2="le texte";c3="DOC"
    result=sinput.Extract("<DOC abcd>le texte</DOC>","%M<%S0%e>%s</%S0",&theEnd,c1,e1,c2,c3);
    if (!result)
        sinput.GetError(c1,&end);
    
    result=sinput.Extract("<DOCU abcd>le texte</DOC>","%M<%S0%e>%s</%S0",&theEnd,c1,e1,c2,c3);//result=NULL
    if (!result)
        sinput.GetError(c1,&end);

    result=sinput.Extract("<DOCU abcd>le texte</DOCUI>","%M<%S0%e>%s</%S0",&theEnd,c1,e1,c2,c3); //result=NULL
    if (!result)
        sinput.GetError(c1,&end);
    //In this last example, c1 and c3 will contain DOCU... theEnd="I>".
    //c3 is resized according to the content of c1
    result=sinput.Extract("<DOCU abcd>le texte</DOCUI>","%M<%S0%e>%s</%V0",&theEnd,c1,e1,c2,c3);
    if (!result)
        sinput.GetError(c1,&end);
}
#endif

