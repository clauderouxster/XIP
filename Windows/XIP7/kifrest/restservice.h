class Callrest {
	void* soapptr;
public:

	void* kifrest;
	bool server;
	char buff[2048];
	int port;
	string host;
	
	Callrest(void*);
	~Callrest();

	bool launch(std::string host,int port);
	bool connect(std::string host, int port);
	SOCKET wait();
	bool get(map<string, string>& dico,int client);
	bool put(string& tokens,string stype,bool,int client);
	bool close();
	string hostport();
	int callget(map<string,string>&);
	int callpost(string,char*,map<string,string>&);
	bool iserver() {
		return server;
	}
};


