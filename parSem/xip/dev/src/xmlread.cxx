
#include <string.h>
#include <stdio.h>

#include <fstream.h>

#ifdef WIN32
#include <io.h>
#include <Winsock.h>
#include <strstrea.h>
#else
#include <ctype.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <strstream.h>
#endif

#ifdef XIPDLL_EXPORT
#define Exportation __declspec(dllexport)
#else
#define Exportation
#endif


#define NON 0
#define OUI 1

#include "xipresult.h"
#include "sinput.h"
#include "client.h"

static VECTA<char*> codageXML;
static VECTA<char*> caractereXML;
char readLeft(ifstream& filename,unsigned char *buffer,char stop);
char readString(ifstream& filename,char *buffer);
char readLine(ifstream& filename,char *buffer);


void XIPBuildParameter(char* parametres) {
    //Value of each variable:
    //reduce=0,1,2      from complete display to very reduced display            (2)
    //rule number=0,1   display the extraction rule number                       (0)
    //tagger =0,1       tagger only                                              (0)
    //extraction=0,1    do not extract or extract dependencies                   (1)
    //offset = 0,1      compute the offset of each word                          (1)
    //display tree=0,1  display a tree on screen                                 (0)
    //tree = 0,1        send the chunk tree in the result                        (1)
    //renumber=0,1      words are locally renumbered in each sentence or not     (0)
    //nsc=0,1           ntm specific should 0 be by default                      (0)
    //unknown=0,1       ntm specific should 0 be by default                      (0)
    //analysis=0,1      ntm only if 0                                            (1)

    sprintf(parametres,"%d:%d:%d:%d:%d:%d:%d:%d:%d:%d:%d|", 2,0,0,1,1,0,1,0,0,0,1);
    
}

void readingCharacter::DisplayXML() {

    while(readSoFar<MaxSize) {
        ReadingBuffer[0]=0;
        currentSize=read(theSocket,ReadingBuffer,4999); 
        ReadingBuffer[currentSize]=0;
        cout<<ReadingBuffer;
        readSoFar+=currentSize;
    
        //cout<<readSoFar<<":"<<MaxSize<<endl;
    }
    cout<<ReadingBuffer<<endl;
}

unsigned char readingCharacter::LireSuite() {

    if (currentIndex < currentSize)
        return ReadingBuffer[currentIndex++];
 

    if (readSoFar>=MaxSize) {
        EOFILE=OUI;
        return 0;
    }
  
    ReadingBuffer[0]=0;
    currentIndex=0;
    currentSize=read(theSocket,ReadingBuffer,4999);
    if (currentSize != -1) {
        readSoFar+=currentSize;
        ReadingBuffer[currentSize]=0;
        currentIndex=1;
        return ReadingBuffer[0];
    }    
    EOFILE=OUI;
    return 0;
}

void readingCharacter::LireUneBalise(char* balise,char init) {
    unsigned char c=init;
    int i=0;

    balise[0]=0;
    
    while (EOFILE==NON && c!='<')
        c=LireSuite();
    
    if (EOFILE==OUI)
        return;

    balise[i++]=c;

    char agit=OUI;
    while (EOFILE==NON && (agit==NON || c!='>')) {
        c=LireSuite();
        if (c=='\"')
            agit=1-agit;
        if (agit==OUI)
            c=tolower(c);
        balise[i++]=c;
    }

    balise[i]=0;
}


unsigned char readingCharacter::LirePCDATA(char* pcdata) {
    unsigned char c=0;
    int i=0;

    pcdata[0]=0;
    c=LireSuite();

    while (EOFILE==NON && c!='<') {
        c=LireSuite();
        pcdata[i++]=c;
    }

    if (i>1)
        pcdata[i-2]=0; //on retire le dernier retour chariot
    
    return c;
}

//-----------------------------------------------------------------------------------------------

XipFeature::XipFeature(char* a,char* v) {
    attribute=DupliqueChaine(a);
    value=DupliqueChaine(v);
}


XipFeature::~XipFeature() {
    libere(attribute);
    libere(value);
}

XipNode::XipNode(char* s,char* l, long g,long d) {
    surface=DupliqueChaine(s);
    if (l!=NULL)
        lemma.ajoute(DupliqueChaine(l));
    left=g;
    right=d;
}

XipNode::~XipNode() {
    libere(surface);
    for (int i=0;i<lemma.dernier;i++)
        libere(lemma[i]);
    features.nettoie();
    daughters.nettoie();
}



XipDependency::XipDependency(char* n) {
    name=DupliqueChaine(n);
}

XipDependency::~XipDependency(){
    libere(name);
    features.nettoie();        
}


XipUnit::XipUnit() {
    root=NULL;
}

XipUnit::~XipUnit() {
    delete root;
    dependencies.nettoie();
}



XipResult::~XipResult() {
    sentences.nettoie();
}

//Les print de base pour les objets ci-dessus

void XipFeature::print(ostream& os) {
    os<<attribute<<":"<<value;
}

void XipNode::print(ostream& os,char feature) {
    int i;
    os<<surface;
    for (i=0;i<lemma.dernier;i++)
        os<<":"<<lemma[i];

    os<<"("<<left<<"-"<<right<<")";
    
    if (feature==OUI) {
        if (features.dernier)
            os<<"[";
        
        for (i=0;i<features.dernier;i++) {
            if (i!=0)
                os<<",";
            features[i]->print(os);
        }
    
        if (features.dernier)
            os<<"]";
    }
    
    if (daughters.dernier)
        os<<"{";
    for (i=0;i<daughters.dernier;i++) {
        if (i!=0)
            os<<",";
        daughters[i]->print(os,feature);
    }
    
    if (daughters.dernier)
        os<<"}";
}

void XipNode::print_daughters(ostream& os,char feature) {
    
    if (lemma.dernier!=0)
        os<<surface;
    else {    
        for (register int i=0;i<daughters.dernier;i++) {
            if (i!=0)
                os<<" ";
            daughters[i]->print_daughters(os);
        }
    }    
}

void XipDependency::print(ostream& os,char feature) {
    int i;
    os<<name;
    if (features.dernier) {
        for (i=0;i<features.dernier;i++)
            os<<"_"<<features[i]->attribute;
    }
    os<<"(";
    for (i=0;i<parameters.dernier;i++) {
        if (i)
            os<<",";
        parameters[i]->print_daughters(os);
    }

    os<<")"<<Endl;
}

void XipUnit::print(ostream& os,char feature) {
    if (root==NULL)
        return;
    root->print(os,feature);
    os<<Endl<<Endl;
    for (int i=0;i<dependencies.dernier;i++)
        dependencies[i]->print(os,feature);
}

void XipResult::printbare(ostream& os,char feature) {
    XipUnit* un=sentences[0];
    if (un !=NULL) {
        XipNode* root = un->root;
        if (root!=NULL) {            
            for (int idau=0;idau<root->daughters.dernier;idau++) {
                XipNode* node=root->daughters[idau];
                os<<"JMAX =\t"<<node->surface<<Endl;
                int i;
                os<<"XIP =\t";
                for (i=0;i<node->lemma.dernier;i++) 
                    os<<node->lemma[i]<<"\t";                

                if (feature==OUI) {
                    if (node->features.dernier)
                        os<<"[";
                    
                    for (i=0;i<node->features.dernier;i++) {
                        if (i!=0)
                            os<<",";
                        os<<node->features[i]->attribute;
                    }
                    
                    if (node->features.dernier)
                        os<<"]";
                }
                os<<Endl<<Endl;                    
            }
        }
    }
}

void XipResult::print(ostream& os,char feature) {
    
    for (int i=0;i<sentences.dernier;i++) {
        os<<"Result: "<<i<<Endl;
        sentences[i]->print(os,feature);
        os<<Endl<<Endl;
    }
}
//---------------------------------------------------------------------------------

//Le fichier contient alors des entrees de la forme:
/*
 <LUNIT>

 <NODE tag=v, start=v, end=v> //can contain embedded nodes
     <features>    //optional (global features)
     </features>
     </mainreading lemma=v,pos=v>
     <reading lemma=v, pos=v>
        <features> //local features
          attr=v.
          attr=v.
        </features>
     </reading> //could be other readings.
 </NODE>
 </LUNIT>
*/
static TansSinput xipsinput;




void XMLCodingTable(char* filename,char* field) {
    if (filename[0]==0)
        return;

    ifstream fichier(filename,ios::in|ios::nocreate);
    char k=0,tampon[300];
    int i;

    if (fichier.fail() ) {
        cerr<<"Reading error in file: "<<filename<<". No XML coding table available..."<<endl;
        return;
    }

    while (k!=':' || strcmp(tampon,field))
        k=readString(fichier,tampon);
    
    k=0;
    while (k!=':') {

        k=readLeft(fichier,(unsigned char*)tampon,'=');
        if (k==':')
            break;

        if (tampon[0]==0)
            return;

        //On elimine les blancs en fin de chaine
        for (i=strlen(tampon)-1;i>0;i--)
            if (tampon[i]<=32)
                tampon[i]=0;
            else
                break;

        //s'il s'agit d'un caractere code (un numero de caractere)
        //attention, a ce moment la il peut y avoir plus d'un caractere
        if (tampon[0]=='#') {
            char* ch=tampon;
            char* fin;
            int d;
            char tab[200];
            int pos=0;        
            if (xipsinput.Extract(ch,"#%d%b",&fin,&d)) {
                tab[pos++]=d;
                tab[pos]=0;
                ch=fin;
                while (xipsinput.Extract(ch,",%b#%d%b",&fin,&d)) {
                    tab[pos++]=d;
                    tab[pos]=0;
                    ch=fin;
                }
            }
            caractereXML.ajoute(DupliqueChaine(tab));
        }                
        else
            caractereXML.ajoute(DupliqueChaine(tampon));
        
        //On lit toute la chaine qui suit...
        readLine(fichier,tampon);      

        //On elimine les blancs en fin de chaine
        for (i=strlen(tampon)-1;i>0;i--)
            if (tampon[i]<=32)
                tampon[i]=0;
            else
                break;

        codageXML.ajoute(DupliqueChaine(tampon));
    }
}


char NettoieUneChaine(char* chaine,char* resul,const char* trouve,const char* remplace) {

    char* debut;
    char* courant=chaine;
    char trouver=0;

    resul[0]=0;
    while ((debut=strstr(courant,trouve))) {
        *debut=0;
        strcat(resul,courant);
        strcat(resul,remplace);
        courant=debut+strlen(trouve);
        trouver=1;
    }        
    
    strcat(resul,courant);
    return trouver;
}


void NettoieLaChaine(char* chaine,char* resul) {
    char buffer[1000]; 
    strcpy(resul,chaine);
    for (int i=0;i<caractereXML.dernier;i++) {
        if (NettoieUneChaine(resul,buffer,codageXML[i],caractereXML[i]))
            strcpy(resul,buffer);
    }
}

/*
void NettoieLaChaine(char* chaine,char* resul) { 
    char buffer[1000];  
    NettoieUneChaine(chaine,buffer,"&#34;","\""); 
    NettoieUneChaine(buffer,resul,"&#60;","<");    
    NettoieUneChaine(buffer,resul,"&#62;",">"); 
    char val[]={160,0}; 
    NettoieUneChaine(buffer,resul,"&#160;",val); 
} 

*/

XipNode* readingCharacter::LireNoeud(char* balise,XipNode* pere,VECTA<XipNode*>& lesNoeuds) {

    char surface[1000];
    char lemme[1000];
    char inter[1000];
    int start,end;
    char terminal=NON;
    char pos[1000];
    int num;
    char* findechaine;
    char finunite=NON;
    XipNode* node=NULL;
    XipFeature* feature=NULL;
    char carsuivant=0;

    if (xipsinput.Extract(balise,"<node num=\"%d\" tag=\"%s\" start=\"%d\" end=\"%d\">",
                          &findechaine,&num,surface,&start,&end)) {
        node=new XipNode(surface,NULL,start,end);
        lesNoeuds.affecte(num,node);
    }
    else
        if (xipsinput.Extract(balise,"<token pos=\"%s\" start=\"%d\" end=\"%d\">",
                              &findechaine,pos,&start,&end)) {

            carsuivant=LirePCDATA(inter);
            NettoieLaChaine(inter,surface);
            node=new XipNode(surface,NULL,start,end);
        }

    LireUneBalise(balise,carsuivant); 
    char attribute[1000];
    char value[1000];
    while (xipsinput.Extract(balise,"<feature attribute=\"%s\" value=\"%s\"",&findechaine,attribute,value)) {
        feature=new XipFeature(attribute,value);
        node->features.ajoute(feature);
        LireUneBalise(balise);
    }

    while (xipsinput.Extract(balise,"<reading lemma=\"%e\" pos=\"%s\"",&findechaine,inter,pos)) {

        NettoieLaChaine(inter,lemme);
        LireUneBalise(balise);  
        node->lemma.ajoute(DupliqueChaine(lemme));
        while (xipsinput.Extract(balise,"<feature attribute=\"%s\" value=\"%s\"",&findechaine,attribute,value)) {
            //feature=new XipFeature(attribute,value);
            //node->features.ajoute(feature);
            LireUneBalise(balise);
        }

        if (!strcmp(balise,"</reading>"))
            LireUneBalise(balise);
    }
    
    if (pere!=NULL)
        pere->daughters.ajoute(node);

    while (!memcmp(balise,"<node",5) || !memcmp(balise,"<token",6)) {
        LireNoeud(balise,node,lesNoeuds);
        LireUneBalise(balise);
    }
    
    if (!strcmp(balise,"</node>") || !strcmp(balise,"</token>"))
        return node;
    
    cerr<<"No </NODE> tag"<<Endl;
    return NULL;
}   

XipDependency* readingCharacter::LireDependances(char* balise,VECTA<XipNode*>& lesNoeuds) {
    char* findechaine;
    char name[1000];
    XipDependency* depend;

    if (xipsinput.Extract(balise,"<dependency name=\"%s\">",&findechaine,name))
        depend=new XipDependency(name);    

    LireUneBalise(balise);

    char attribute[1000];
    char value[1000];
    XipFeature* features;
    while (xipsinput.Extract(balise,"<feature attribute=\"%s\" value=\"%s\"",&findechaine,attribute,value)) {
        features=new XipFeature(attribute,value);
        depend->features.ajoute(features);
        LireUneBalise(balise);
    }

    //Lecture des parametres..
    int num,position;

    while (xipsinput.Extract(balise,"<parameter ind=\"%d\" num=\"%d\"",&findechaine,&position,&num)) {
        if (lesNoeuds[num]!=NULL)
            depend->parameters.affecte(position,lesNoeuds[num]);
        LireUneBalise(balise);
    }    
    return depend;
}

void readingCharacter::ResultXML(XipResult& result) {
    char balise[1000];
    XipNode* node;
    XipUnit* unite;
    VECTA<XipNode*> lesNoeuds;

    balise[0]=0;

    LireUneBalise(balise);

    while (EOFILE==NON && strcmp(balise,"</xipresult>")) {
        LireUneBalise(balise);
        //Dans le cas ou l'on rencontre une balise <lunit>, cela signifie que la decomposition en
        //phrase est deja effectuee

        if (!strcmp(balise,"</xipresult>"))
            break;

        if (!strcmp(balise,"<lunit>")) {
            unite=new XipUnit;
            result.sentences.ajoute(unite);
            LireUneBalise(balise);
        }
        else
            return;

        while (EOFILE==NON && strcmp(balise,"</xipresult>")) {
            if (!strcmp(balise,"</lunit>"))
                break;

            lesNoeuds.raz();
            if (!memcmp(balise,"<node",5) || !memcmp(balise,"<token",6) ) {
                node=LireNoeud(balise,NULL,lesNoeuds);
                unite->root=node;
            }
                       
            LireUneBalise(balise);
            
            while (!memcmp(balise,"<dependency",11)) {
                XipDependency* dep=LireDependances(balise,lesNoeuds);
                if (unite!=NULL)
                    unite->dependencies.ajoute(dep);
                LireUneBalise(balise);
            }            
        }
    }
}

   

    

//-----------------------------------------------------------------------------------------------
//Les Methodes que l'on exporte...

Exportation int XIPParseString(char* string,int port, char* hostname,char* parameters,XipResult& result,char xml) {
    register int len;
    char ch[100];
    
    struct sockaddr_in servAddr;


    memset((void*)&servAddr,0,sizeof(servAddr));

    struct hostent *hp;

    //recherche du nom du serveur...
    const int MAXHOSTNAME=1000;
    char nomDuServeur[MAXHOSTNAME+1];
    if (hostname==NULL || hostname[0]==0)
        gethostname(nomDuServeur, MAXHOSTNAME);
    else
        strcpy(nomDuServeur,hostname);
    
    hp=gethostbyname(nomDuServeur);

    if (hp==NULL) {
        cout<<"Erreur sur le nom du host"<<endl;
        exit(-1);
    }
        
    memcpy((char *)&servAddr.sin_addr,hp->h_addr,hp->h_length);
    servAddr.sin_family = hp->h_addrtype;  
    servAddr.sin_port = htons(port);

    int sock=socket(AF_INET,SOCK_STREAM,0);
    char chaine[5001];
    int nb,lu;

    len = sizeof(servAddr);
   
    if (connect(sock, (struct sockaddr*) &servAddr, len) < 0) {
        perror("Le client: connect");
        exit(1);
    }

    char arretServeur=0;
    if (strstr(string,"-1:-2:-3:-4:-5:-6:-7|STOPXIPSERVER"))
        arretServeur=1;

    //In the case we want the server to stop, we send the string that is
    //expected and isolated before and we exit...
    if (arretServeur==1) {
        send(sock,chaine,strlen(chaine),0);                
        close(sock);
        return -1;
    }


    //On envoie les parametres...
    send(sock,parameters,strlen(parameters),0);

    if (string[0]==0) {
        sprintf_s(ch,100,"0|");
        send(sock,ch,strlen(ch),0);
        send(sock, ch,0, 0);
        return -1;
    }

    int taille=strlen(string);
    sprintf_s(ch,100,"%d|",taille);

    //On envoie la taille
    send(sock,ch,strlen(ch),0);
    send(sock,string,taille,0);

    taille=0;

    chaine[0]=0;
    nb=read(sock,chaine,4999); 
    chaine[nb]=0;
    lu=0;

    taille=atoi(chaine);
    char* pt=strchr(chaine,'|');

    if (taille==-1) {
        if (pt==NULL)
            cerr<<"Error server: "<<chaine<<endl;
        else
            cerr<<"Error server: "<<pt+1<<endl;
        close(sock);
        return -1;
    }


    if (pt!=NULL) {
        pt++;
        lu=strlen(pt);
        if (lu!=0)
            cout<<pt;
    }
    
    readingCharacter re(taille,lu,sock);

    if (xml==OUI)
        re.DisplayXML();
    else
        re.ResultXML(result);

    close(sock);
    return 0;
}


Exportation int XIPParseFile(char* filename,int port, char* hostname,char* parameters,XipResult& result,char xml) {

    char chaine[5001];
    int nb;

    int taille=0;
    ostrstream lecture;
    char cc;
    nb=0;
    istream* input;
    if (filename[0]!=0) {
        input=new ifstream(filename,ios::nocreate);
        if (input->fail()) {
            cerr<<"No filename: "<<filename<<endl;
            return -1;
        }
    }
    else
        input=&cin;

    char arretServeur=0;

    while (!input->eof()) {
        cc=input->get();        
        if (nb<35) {
            chaine[nb]=cc;
            chaine[++nb]=0;
            if (nb > 33 && strstr(chaine,"-1:-2:-3:-4:-5:-6:-7|STOPXIPSERVER")) {
                arretServeur=1;
                break;
            }
        }

        lecture<<cc;
        taille++;
    }

    if (filename[0]!=0)
        delete input;

    taille--;
    if (lecture.str()!=NULL && lecture.pcount()!=0) {
        lecture.str()[taille]=0;
        return XIPParseString(lecture.str(),port,hostname,parameters,result,xml);
    }

    return -1;
}

    
    

        



