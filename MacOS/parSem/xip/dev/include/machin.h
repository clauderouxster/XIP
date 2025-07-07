#ifndef machin_h
#define machin_h



class machin {
public:

    char* mot;
    Traits* traits;

    machin(char* m, Traits* tr) {
        traits=tr;
        mot=strdup(m);
    }
    ~machin() {
        free(mot);
    }
};


#ifdef gloglo
machin* Glup[10000];
int iglup=0;
#else
extern machin* Glup[10000];
extern int iglup=0;
#endif

#endif
