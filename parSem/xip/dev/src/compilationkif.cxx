void GetXNKeys(map<string,unsigned short>& keys, x_node* xn) {
	if (keys.find(xn->token)==keys.end())
		keys[xn->token]=keys.size();
	
	if (xn->value!="" && keys.find(xn->value)==keys.end())
		keys[xn->value]=keys.size();
	
	for (int i=0;i<xn->nodes.size();i++)
		GetXNKeys(keys,xn->nodes[i]);
}				  

void AfficheXN(x_node* xn) {
		cout<<STR(xn->token)<<" --> "<<STR(xn->value)<<endl;
	
	for (int i=0;i<xn->nodes.size();i++)
		AfficheXN(xn->nodes[i]);
}				  

void SaveXNKeys(ofstream& f,map<string,unsigned short>& keys, x_node* xn) {
	if (xn->nodes.empty())
		f<<"@"<<keys[xn->token]<<"$";
	else
		f<<"@"<<keys[xn->token]<<"#";
	if (xn->value=="")
		f<<0;
	else
		f<<keys[xn->value];
	
	if (xn->nodes.empty())
		return;
	
	f<<"{";
	for (int i=0;i<xn->nodes.size();i++)
		SaveXNKeys(f,keys,xn->nodes[i]);
	f<<"}";
}		  
	
void SaveXN(string filename,x_node* xn) {
	map<string,unsigned short> keys;
	filename+=".kif";
	ofstream f(STR(filename),ios::binary);
	f<<"kif:\r";
	f<<"@JekiffeKiFcommeunouf@\r";
	keys[filename]=1;
	GetXNKeys(keys,xn);
	f<<keys.size();
	map<string,unsigned short>::iterator it;
	for (it=keys.begin();it!=keys.end();it++)
		f<<it->second<<" "<<STR(it->first)<<"\r";
	f<<"*\r";
	SaveXNKeys(f,keys, xn);
}

x_node* ReadXN(ifstream& f,map<unsigned short,string>& keys,bool error) {
	if (f.eof()) {
		error=true;
		return NULL;
	}
	char c=f.get();
	if (f.eof() || error==true || c=='}')
		return NULL;
	if (c!='@') {
		error=true;
		return NULL;
	}
	string k;
	short key,val;
	x_node* xn;

	f>>key;
	f>>c;
	f>>val;
	if (c=='$') {
		xn=new x_node();
		xn->set(keys[key],keys[val]);
		return xn;
	}
	else
		if (c!='#') {
			error=true;
			return NULL;
		}
	f>>c;	
	if (f.eof() || c!='{') {
		error=true;
		return NULL;
	}
	
	xn=new x_node();
	xn->set(keys[key],keys[val]);
	
	x_node* xsub=xn;
	while (xsub!=NULL) {
		xsub=ReadXN(f,keys,error);
		if (xsub!=NULL)
			xn->nodes.push_back(xsub);
	}
	
	if (error) {
		delete xn;
		return NULL;
	}
	
	return xn;
}
	
x_node* LoadXN(string filename) { 
	vector<string> keys;
	keys.push_back(0);
	filename+=".kif";
	ifstream f(STR(filename),ios::binary);
	char buff[1000];
	f.getline(buff,1000,'\r');
	if (!strcmp(buff,"kif:"))
		f.getline(buff,1000,'\r');
	if (strcmp(buff,"@JekiffeKiFcommeunouf@"))
		return NULL;
	
	char* pos;
	short key;
	size_t sz;
	f>>sz;
	keys.reserve(sz+1);
	f.getline(buff,1000,'\r');
	while (!f.eof() && buff[0]!='*') {
		pos=strchr(buff,' ');
		if (pos!=NULL) {
			*pos=0;
			key=atoi(buff);
			keys[key]=pos+1;
		}
		f.getline(buff,1000,'\r');
	}

	if (f.eof())
		return NULL;
	bool error=false; 
	return ReadXN(f,keys,error);
}
	
bool Compiled(string base) {
	string compiled=base+".kif";
	int stinit=-1;
	int stcible=-1;
	struct stat scible;
	struct stat sinit;
	FILE* init=fopen((char*)base.c_str(),"r");
	FILE* cible=fopen((char*)compiled.c_str(),"r");
	if (init!=NULL) {
		if (cible!=NULL) {
#if  defined(WIN32) | defined(APPLE) | defined(XIPFSTAT64)
			stinit=fstat(init->_file,&sinit);
			stcible=fstat(cible->_file,&scible);
#else
			stinit=fstat(init->_fileno,&sinit);
			stcible=fstat(cible->_fileno,&scible);
#endif
			fclose(cible);
		}
		fclose(init);
	}
	else
		if (cible!=NULL) {
			fclose(cible);
			return true;
		}
	if (stinit!=-1 && stcible!=-1 && scible.st_mtime > sinit.st_mtime)
		return true;
	return false;
}
