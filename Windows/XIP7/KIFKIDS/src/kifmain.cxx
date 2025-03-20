/*
 * Xerox Research Centre Europe - Grenoble Laboratory
 *
 * Copyright (C) 2010 - 2011 Xerox Corporation, XRCE-Grenoble, All Rights Reserved.
 * This file can only be used with the KiF library, 
 * it should not and cannot be used otherwise.
 */
/* --- CONTENTS ---
   Project    : KiF
   Version    : 1.00
   filename   : kifmain.cxx
   Date       : 09/09/2010
   Purpose    : Implementation of the KiF language
   Programmer : Claude ROUX
   Reviewer   : 
*/

#include "kif.h"
#include "kifversion.h"

#include <stdio.h>

extern "C" {
//--------------------- Initialisation---------------------------------------------
	bool Init_kifsys(KifCode* kifcode,string version);
	bool Init_kifsqlite(KifCode* kifcode,string version);
	bool Init_kifsocket(KifCode* kifcode,string version);
	bool Init_xmldoc(KifCode* kifcode,string version);
	bool Init_kifregex(KifCode* kifcode,string version);
	bool Init_kifsparse(KifCode* kifcode,string version);
#ifdef FLTKCODE	
	bool Init_kifltk(KifCode* kifcode,string version) ;
#endif

#ifdef SOUND
	bool Init_kifmp3(KifCode* kifcode, string version);
#endif
#ifdef AOSOUND
     bool Init_kifsound(KifCode* kifcode,string version) ;
#endif

#ifdef WITHCURL
	bool Init_kifcurl(KifCode* localkifcode,string version);
#else
	bool Init_kifcurl(KifCode* localkifcode,string version) {return true;}
#endif

//--------------------- Termination Modules---------------------------------------------
	bool kifsys_Terminate(KifCode* kifcode);
	bool kifsqlite_Terminate(KifCode* kifcode);
	bool kifsocket_Terminate(KifCode* kifcode);
	bool xmldoc_Terminate(KifCode* kifcode);
	bool kifsparse_Terminate(KifCode* kifcode);
	bool kifregex_Terminate(KifCode* kifcode);
#ifdef FLTKCODE	
	bool kifltk_Terminate(KifCode* kifcode) ;
#endif

#ifdef SOUND
	bool kifmp3_Terminate(KifCode* kifcode);
#endif

#ifdef AOSOUND
     bool kifsound_Terminate(KifCode* kifcode) ;
#endif

#ifdef WITHCURL
	bool kifcurl_Terminate(KifCode* localkifcode);
#else
	bool kifcurl_Terminate(KifCode* localkifcode) {return true;}
#endif

	
//--------------------- Initialisation Modules---------------------------------------------
	bool kifsys_KifInitialisationModule(KifCode* kifcode,string version);
	bool kifsqlite_KifInitialisationModule(KifCode* kifcode,string version);
	bool kifsocket_KifInitialisationModule(KifCode* kifcode,string version);
	bool xmldoc_KifInitialisationModule(KifCode* kifcode,string version);
	bool kifsparse_KifInitialisationModule(KifCode* kifcode,string version);
	bool kifregex_KifInitialisationModule(KifCode* kifcode,string version);
#ifdef FLTKCODE	
	bool kifltk_KifInitialisationModule(KifCode* kifcode,string version) ;
	bool Fltk_window_editor(KifCode*,string);
#else
	bool Fltk_window_editor(KifCode*,string) {}
#endif
#ifdef WITHCURL
	bool kifcurl_KifInitialisationModule(KifCode* localkifcode,string version);
#else
	bool kifcurl_KifInitialisationModule(KifCode* localkifcode,string version) {return true;}
#endif

#ifdef SOUND
	bool kifmp3_KifInitialisationModule(KifCode* localkifcode, string version);
#endif

#ifdef AOSOUND
	bool kifsound_KifInitialisationModule(KifCode* localkifcode, string version);
#endif

//------------------------------------------------------------------
}

#ifdef DOSOUTPUT
void setdosoutput(bool d);
#define conversion2dos(X) s_utf8_to_dos(X)
#else
#define conversion2dos(X) X
#endif

#ifdef KIFNEEDCLOSELIBS
void KifCloseLibraries();
#endif

void displayhelpkif(string wh) {
    cout<<conversion2dos(STR(wh))<<Endl;
    cout<<conversion2dos(KIFMAINSTRINGS[0])<<endl;
    cout<<conversion2dos(KIFMAINSTRINGS[1])<<endl;
    cout<<conversion2dos(KIFMAINSTRINGS[2])<<endl;
    cout<<conversion2dos(KIFMAINSTRINGS[3])<<endl;
    cout<<conversion2dos(KIFMAINSTRINGS[4])<<endl;
    cout<<"\t";
	cout<<conversion2dos(KIFMAINSTRINGS[5])<<endl;
	cout << "\t";
	cout << conversion2dos(KIFMAINSTRINGS[6]) << endl;
	cout << "\t";
	cout << conversion2dos(KIFMAINSTRINGS[7]) << endl;
	cout << "\t";
	cout << conversion2dos(KIFMAINSTRINGS[8]) << endl;
	cout << "\t";
	cout << conversion2dos(KIFMAINSTRINGS[9]) << endl;
	cout << "\t";
	cout << conversion2dos(KIFMAINSTRINGS[10]) << endl;
	cout << "\t";
	cout << conversion2dos(KIFMAINSTRINGS[11]) << endl;
    exit(-1);
}



int main(int argc, char *argv[]) {
	vector<string> kifarguments;
	KifCode kifcode(10000);
	string lnstr;

	KifSetMainThread();
	string wh="kif: ";
    wh+=KIFVERSION;
#ifdef UNIXCOMPUTERNAME
	wh+=" (";
	wh+=COMPUTERNAME;
	wh+=")";
#endif
    wh+=Endl;

	if (argc<=1) {		
#ifdef ARCH64
		cerr<<"64 bits"<<endl;
#endif
#ifndef WIN32
		cerr<<KIFMAINSTRINGS[12]<<endl;
#endif
#ifdef MACXCODE
        cerr<<KIFMAINSTRINGS[13]<<endl;
#endif 
#ifdef WITHCURL
		cerr<<conversion2dos(KIFMAINSTRINGS[14])<<endl;
#endif
#ifdef FLTKCODE	
		cerr<<conversion2dos(KIFMAINSTRINGS[15])<<endl;
#endif		
        cerr<<endl;
        displayhelpkif(wh);
	}

	char piped=0;
	bool debugkif=false;
	bool comp=false;
	//argv[1] is the file name
	string name=argv[1];
	string code="";
	string filename;
	bool console=false;
    string args;
	for (int i=1;i<argc;i++) {
		args=argv[i];
        if (args[0]=='-') {
#ifdef DOSOUTPUT
			if (args == KIFMAINSTRINGS[16]) {
				setdosoutput(false);
				continue;
			}
#endif
            if (args=="-h")
                displayhelpkif(wh);
            
            if (args==conversion2dos("-debug")) {
                debugkif=true;
                continue;
            }

            if (args==conversion2dos(KIFMAINSTRINGS[17])) {
                comp=true;
                continue;
            }

            if (args=="-console") {
                console=true;    
                if (i+1<argc) {
                    if (argv[i+1][0]!='-')
                        filename=argv[++i];
                }
                continue;
            }
                        
            if (args=="-a")
                piped=1;
            else
                if (args=="-p")
                    piped=2;
                else
                    if (args=="-c")
                        piped=0;
            
            if (i+1<argc) {
                if (argv[i+1][0]!='-')
                    code=argv[++i];
            }
            
            if (code=="" && piped!=1) {
                cerr<<conversion2dos(KIFMAINSTRINGS[18])<<endl;
                exit(-1);
            }
            continue;
        }
        
        if (i!=1)
            kifarguments.push_back(args);
	}

    if (piped==1) {
        while (!cin.eof()) {
			getline(cin,lnstr);
			if (lnstr.size())
                kifarguments.push_back(lnstr);
			lnstr = "";
        }
    }

	KifRecordLibraries("Init_kifsparse",Init_kifsparse);
	KifRecordLibraries("Init_kifsys",Init_kifsys);
	KifRecordLibraries("Init_kifcurl",Init_kifcurl);
	KifRecordLibraries("Init_kifsqlite",Init_kifsqlite);
	KifRecordLibraries("Init_kifsocket",Init_kifsocket);
	KifRecordLibraries("Init_xmldoc",Init_xmldoc);	
	KifRecordLibraries("Init_kifregex",Init_kifregex);
#ifdef FLTKCODE	    
	KifRecordLibraries("Init_kifltk",Init_kifltk);
#endif

#ifdef SOUND
	KifRecordLibraries("Init_kifmp3", Init_kifmp3);
#endif

#ifdef AOSOUND
     KifRecordLibraries("Init_kifsound",Init_kifsound);
#endif

	KifRecordMainLibraries("kifsparse_KifInitialisationModule",kifsparse_KifInitialisationModule);
	KifRecordMainLibraries("kifsys_KifInitialisationModule",kifsys_KifInitialisationModule);
	KifRecordMainLibraries("kifcurl_KifInitialisationModule",kifcurl_KifInitialisationModule);
	KifRecordMainLibraries("kifsqlite_KifInitialisationModule",kifsqlite_KifInitialisationModule);
	KifRecordMainLibraries("kifsocket_KifInitialisationModule",kifsocket_KifInitialisationModule);
	KifRecordMainLibraries("xmldoc_KifInitialisationModule",xmldoc_KifInitialisationModule);	
	KifRecordMainLibraries("kifregex_KifInitialisationModule",kifregex_KifInitialisationModule);
#ifdef FLTKCODE	    
	KifRecordMainLibraries("kifltk_KifInitialisationModule",kifltk_KifInitialisationModule);
#endif

	KifRecordTerminateModule("kifsparse_Terminate",kifsparse_Terminate);
	KifRecordTerminateModule("kifsys_Terminate",kifsys_Terminate);
	KifRecordTerminateModule("kifcurl_Terminate",kifcurl_Terminate);
	KifRecordTerminateModule("kifsqlite_Terminate",kifsqlite_Terminate);
	KifRecordTerminateModule("kifsocket_Terminate",kifsocket_Terminate);
	KifRecordTerminateModule("xmldoc_Terminate",xmldoc_Terminate);	
	KifRecordTerminateModule("kifregex_Terminate",kifregex_Terminate);
#ifdef FLTKCODE	    
	KifRecordTerminateModule("kifltk_Terminate",kifltk_Terminate);
#endif

#ifdef SOUND
	KifRecordMainLibraries("kifmp3_KifInitialisationModule", kifmp3_KifInitialisationModule);
#endif

#ifdef AOSOUND
     KifRecordMainLibraries("kifsound_KifInitialisationModule",kifsound_KifInitialisationModule);
#endif

	KifReloadLibraries(&kifcode);

	//If the console has been requested...
	if (console) {
		//We record the libraries...
		kifcode.InitArguments(kifarguments);		
		Fltk_window_editor(&kifcode,filename);
		exit(0);
	}
	

	if (debugkif==OUI)
		KiFSetdebug(true);

	kifGlobals->compile=comp;


    //executing a file
	if (code=="") {
        if (name[0]=='-') {
            cerr<<conversion2dos(KIFMAINSTRINGS[19])<<endl;
            exit(-1);
        }
        
		kifcode.InitArguments(kifarguments);
		name=Getfilename(argv[1]);
		ifstream* fkif=new ifstream(STR(name),ios::in|ios::binary);
#ifndef _DEBUG
		try {
#endif
			kifcode.Setbaseline(1);
			kifcode.Loadfile(fkif,name,0);
#ifndef _DEBUG
		}
		catch(KifRaiseError* m) {
			cerr<<STR(m->message)<<endl;
			delete m;
			exit(-1);
		}
#endif
        kifcode.Run();
        if (Error()==true)
            cerr<<STR(Errorstring())<<endl;

	kifcode.Terminate();
#ifdef KIFNEEDCLOSELIBS
	KifCloseLibraries();
#endif
        exit(0);
	}
    
    kifcode.Evaluate(kifcode.kstart,code,true);		
    if (Error()==true) {
        cerr<<KIFMAINSTRINGS[20]<<STR(Errorstring())<<endl;
        exit(-1);
    }

    //just a bit of code...
    if (piped!=2) {
        kifcode.InitArguments(kifarguments);		
        kifcode.Run();
        if (Error()==true)
            cerr<<STR(Errorstring())<<endl;
        exit(0);
    }
        
    vector<string> thepiped;
	lnstr = "";
    while (!cin.eof()) {
		getline(cin, lnstr);
		if (lnstr.size())
            thepiped.push_back(lnstr);
		lnstr = "";
    }

    //we loop for each argument...
	KifCode* kcode=NULL;
    for (int i=0;i<thepiped.size();i++) {
		if (!i)
			kcode=&kifcode;
		else {
			kcode=new KifCode(1000);
			kcode->Evaluate(kcode->kstart,code,true);
		}
        kifarguments.push_back(thepiped[i]);
        kcode->InitArguments(kifarguments);		
        kcode->Run();
        if (Error()==true) {
            cerr<<KIFMAINSTRINGS[20]<<STR(Errorstring())<<endl;
            exit(-1);
        }
        kifarguments.pop_back();
		if (i)
			delete kcode;
    }
}


