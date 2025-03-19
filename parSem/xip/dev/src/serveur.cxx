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
   filename   : serveur.cxx
   Date       : 10/01/1999
   Purpose    : XIP server implementation
   Programmer : Claude ROUX
   Reviewer   : 
*/


#include "parsbin.h"
extern ostream* flot_erreur;

#ifdef XIPSERVER

#include <sys/utsname.h>        /* for uname() */
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <signal.h>
#include <pthread.h>

#ifdef SOLARIS
#define socklen_t int
#endif

static GlobalParseur* leParseurCourant=NULL;
static int laSocket=-1;
static int semaphore=0;
static int erreurSurvenue=NON;
static int dejaConnecte=NON;
//En cas d'erreur sur une socket, on relance la creation
void erreurSocket(int i) {
    *flot_erreur<<"Error Socket: "<<i<<endl;   

    leParseurCourant->Detruit();
    //close(i);
    //shutdown(i,2);
    dejaConnecte=OUI;
    leParseurCourant->LancerServeur();
}


void nettoyageSocket(int i) {
    
    if (leParseurCourant->socketcourante!=-1) {
        *flot_erreur<<"Closing Socket"<<endl;        
        close(leParseurCourant->socketcourante);
    }
    erreurSurvenue=2;
    exit(-1);
}


//Structure pour renvoyer le resulat d'une analyse...
class envoie {
public:
    int longueur;
    int ns;
    int socket;
    ostrstream* os;

    //Fonction d'envoi du resultat...
    void envoyer() {
        char ch[100];
        sprintf(ch,"%d|",longueur);
        
        send(ns,ch,strlen(ch),0);
        send(ns, os->str(),longueur,0);
    }

    envoie(int l,int n,ostrstream* oo,int sock) {
        longueur=l;
        ns=n;
        os=oo;
        socket=sock;
    }


    ~envoie() {
        delete os;
    }
};
    

//Fonction qui est utilise pour renvoyer le resultat vers le client
void* envoyer(void* arg) {
    envoie* env=(envoie*)arg;
    env->envoyer();
    delete env;
    return NULL;
}


void InitialisationSignaux() {            
    dejaConnecte=NON;
    signal(SIGINT,nettoyageSocket);
}

void GlobalParseur::LancerServeur() {


    static struct sockaddr_in servAddr;
    struct sockaddr_in cliAddr;
    int fromlen,res,ns,sock;
    socklen_t len;
    char ENTREES[1000];
    const int MAXHOSTNAME=1000;
    char nomDuServeur[MAXHOSTNAME+1];

    leParseurCourant=this;
    signal(SIGPIPE,erreurSocket);

    if (dejaConnecte==NON) {
        struct hostent *hp;

    
        gethostname(nomDuServeur, MAXHOSTNAME);           /* who are we? */
        *flot_erreur<<nomDuServeur<<">>>"<<lePortServeur<<endl;
        hp= gethostbyname(nomDuServeur);                  /* get our address info */

        if (hp == NULL)                             /* we don't exist !? */
            exit(-1);
        
        memcpy((char *)&servAddr.sin_addr,hp->h_addr,hp->h_length);     /* set address */
        servAddr.sin_family= hp->h_addrtype;              /* this is our host address */
        /* this is our port number */
        servAddr.sin_port= htons(lePortServeur);  
    
        //servAddr.sin_family = AF_INET;
        //servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
        //servAddr.sin_port = htons(lePortServeur);
    
        len = sizeof(servAddr);

        sock=socket(AF_INET,SOCK_STREAM,0);

        if (bind(sock,(struct sockaddr*)&servAddr,len) < 0) {
            perror("Erreur: bind: ");
            exit(-1);
        }

        semaphore=1;
 
        if (listen(sock, 20) <0) {
            perror("Erreur listen:");
            exit(-1);
        }        
   
        socketcourante=sock;
    }

    while (1) {
        *flot_erreur<<"WAITING"<<endl;
        
        if ((ns = accept(socketcourante,(struct sockaddr*)&cliAddr, &len)) < 0) {
            dejaConnecte=NON; 
            perror("Error accept : ");
            close(socketcourante);
            Detruit();
            LancerServeur();
            exit(-1);
        }

        *flot_erreur<<"Starting"<<endl;
        leParseurCourant=this;

        char ch[100];
        int taille;
        int nbl;
        int compte;            
        OS=new ostrstream;
        
        ostrstream* lect=new ostrstream;
                
        nbl=read(ns,ENTREES,500);
        ENTREES[nbl]=0;

        //La premiere partie est composee des parametres
        char parametres[12];
        int ipar=0;
        char* pt=ENTREES;
        while ( *(pt-1) !='|') {
            parametres[ipar++] = *pt-48;
            pt+=2;
        }

        InitialisationParametres(parametres);
            
        strcpy(ENTREES,pt);
        pt=strchr(ENTREES,'|');
        if (pt==NULL) {
            taille=strlen(ENTREES);
            nbl=read(ns,ENTREES+taille,500);
            ENTREES[taille+nbl]=0;
        }
        

        taille=atoi(ENTREES);
            
        if (taille==-1)
            break;

        compte=0;

        pt=strchr(ENTREES,'|');
        if (pt!=NULL) {
            pt++;
            compte=strlen(pt);
            if (compte!=0)
                *lect << pt;
        }

        ENTREES[0]=0;
        while (compte<taille && nbl!=0) {    
            nbl=read(ns,ENTREES,500);
            ENTREES[nbl]=0;
            *lect << ENTREES;
            compte+=nbl;
        }
        //Petite verification
                
        if (lect->str()==NULL || lect->pcount()==0) {
            strcpy(ch,"-1|no string to process");
            send(ns,ch,strlen(ch),0);
            delete OS;
            delete lect;
            continue;
        }
                
        lect->str()[lect->pcount()]=0;

        if (strstr(lect->str(),"STOPXIPSERVER")) {
            *flot_erreur<<"Server stops"<<endl;
            nettoyageSocket(-1);            
            break;
        }

        *flot_erreur<<"Received text... Processing..."<<endl;
                
            
        AssocieChaine(lect->str());
        XML=OUI;
        Analyse(parametres[7],parametres[6]);

        int longueur= OS->pcount();

        if ( OS->str()==NULL || longueur==0) {
            strcpy(ch,"-1|no result...");
            send(ns,ch,strlen(ch),0);
            delete OS;
            delete lect;
            continue;
        }

        *flot_erreur<<num_phrase<<" sentences analyzed... Returning result"<<endl;
        OS->str()[longueur]=0;

        envoie* env = new envoie(longueur,ns,OS,socketcourante);
        OS=NULL;
        delete lect; 

        try {                
            //Threads standard
            pthread_attr_t tattr;
            pthread_t tid;
            int ret = pthread_create(&tid, NULL, envoyer, (void*)env);
        }
        catch(...) {
            *flot_erreur<<"Connection lost"<<endl<<flush;
            delete env;
        }
    }
    close(socketcourante);
    laSocket=-1;
}

#endif
