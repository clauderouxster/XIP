#ifndef myconnect_h
#define myconnect_h

void CreateConnection(char*);
void Disconnection();
void LockTableConcepts();
void UnLockTableConcepts();

class Iddoc {
	public:
	char type;
	int max;
	int idoc;
	int ipar;
	int isent;
	int nbdep;
	int poids;
	VECTA<int> matches;
	string sentence_text;
	Iddoc(char ty,int id,int ip,int is) {
		type=ty;
		idoc=id;
		ipar=ip;
		isent=is;
		nbdep=0;
		max=0;
		poids=1;
	}
};

#endif
