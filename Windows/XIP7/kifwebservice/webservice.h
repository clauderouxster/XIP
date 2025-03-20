class Callwebservice {
	void* client;
	void* kifservice;
	void* soap;
public:
	char buff[2048];
	
	string host;
	int port;

	Callwebservice(void*,bool);
	~Callwebservice();

	int run(string host,int port);
	string Generatewsdl(string location,int prt);

	int execute0(string, string&);
	int execute1(string, string, string&);
	int execute2(string, string, string, string&);
	int execute3(string, string, string, string, string&);
	int execute4(string, string, string, string, string, string&);
	int execute5(string, string, string, string, string, string, string&);

	int methods(string&);
	int getmethods(string endpoint,string&);
	int calling(string endpoint, string name, vector<string>& param, string& ret);
	string hostport();

	string error() {
		return buff;
	}
};


