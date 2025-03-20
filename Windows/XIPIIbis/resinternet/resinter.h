#ifndef resinter_h
#define resinter_h

class SortedWords {

public:
    
    SortedWords() {
        verb[0]=0;
        word[0]=0;
    }

    SortedWords(char* v,char* w1) {
        strcpy(verb,v);
        strcpy(word,w1);
    };

    //~SortedWords() {};

    char word[200];
    char verb[200];
    ofstream* fichier;
};

class echange {
public:
    
    echange(ostringstream* o,HWND h) {
        os=o;
        hwnd=h;
    }

    ostringstream* os;
    HWND hwnd;
    HANDLE hmutex;
};

#endif
