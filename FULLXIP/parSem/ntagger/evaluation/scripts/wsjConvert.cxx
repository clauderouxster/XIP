use('kifsqlite');	

//**************************Convert Function*************************************
function remove_slash_backslash(string s){
return(s[:'\/'][:-2]+'\'+s['\/':][2:]);

}



function convert(string filepath,string folderName){
file f(filepath,'r');
svector svect=f.read();
f.close();
string ln;
svector string;
string st;
svector subst;
svector sv;
for (ln in svect){
	//remove line spaces	
	if(ln=='' || ln=='======================================') continue;
	
	
	string=ln.split(" ");
	for(st in string){     
				
				if(st.regexip("?+/?+"))	{
					if('\/' in st) st=remove_slash_backslash(st);
					subst=st.split("/");
					//println(subst);
					sv.push(subst[0]+_sep+subst[1]);
						}
		
			}
	}	



file r(folderName+_sep+filepath['wsj_':]+'.txt','w');
//file r(folderName+_sep+'wsj_0100.pos.txt','w');
string line;
for(line in sv)
	r.write(line,_endl);
r.close();
}

//convert('/home/fnoorala/Desktop/hmmComparison/wsj/00/wsj_0044.pos',_paths[0]);

//*************************************************************
string wsjfolderName='/home/fnoorala/Desktop/hmmComparison/wsj'; 
svector wsjv=kifsys.listdirectory(wsjfolderName);

svector	wsjfilepath;
svector stdfilepath;
string y;
for (y in wsjv){
	if(y.regexip("%d%d")){
		//create std folder in each wsj directory
		//string path=wsjfolderName+_sep+y+_sep+'std';
		//kifsys.createdirectory(std);
		//vector of path for wsj files
			 stdfilepath.push(y+_sep+'std');
			 wsjfilepath.push(y);
			      }
		}

//full path
//full path
svector wsjfiles_fullpath;
int ii=20; // 9 ='01' 11='02' 20=='00'
//for(ii=0;ii<wsjfilepath.size();ii++){
		wsjfiles_fullpath.push(wsjfolderName+_sep+wsjfilepath[ii]);
//					}

//println(wsjfiles_fullpath);


string a;
string b;
svector svect;
for (a in wsjfiles_fullpath){
	svector wsjfiles=kifsys.listdirectory(a);
	for(b in wsjfiles){
		if(".pos" in b) 
			{
			//println(a+_sep+b);
			string file=a+_sep+b;
			string folder=file[:'wsj_'][:-4]+_sep+'std';
			println(file['wsj_':]);
			convert(file,folder);
			}
	

}}



