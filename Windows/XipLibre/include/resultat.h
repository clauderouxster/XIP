#ifndef resultat_h
#define resultat_h

class ExportResult {
public:
	int max;
	char* res[300];

	ExportResult() {max=0;}

	int Add(char* c) {
		if (max > 300)
			return 0;
		
		res[max++]=strdup(c);
		return 1;
	}

	~ExportResult() {
		for (int i=0;i<max;i++)
			free(res[i]);
	}
};

#endif

