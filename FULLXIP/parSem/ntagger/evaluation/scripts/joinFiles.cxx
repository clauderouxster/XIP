function compare(string i,string j) {
     if (i<j)
          return(true);
     return(false);
}

//merge all comparison files in one main file
string wsjfolderName='/home/fnoorala/Desktop/hmmComparison/wsj';
svector wsjv=kifsys.listdirectory(wsjfolderName);
svector wsjfilepath;
string y;
for (y in wsjv){
     if(y.regexip("%d%d")){
          //vector of path for wsj files
          wsjfilepath.push(y+_sep+'kifcompare');
     }
}

//full path

svector wsjfiles_fullpath;
int ii;
for(ii=0;ii<wsjfilepath.size();ii++){
     wsjfiles_fullpath.push(wsjfolderName+_sep+wsjfilepath[ii]);
}


string a;
string b;
svector main;
svector svect;

for (a in wsjfiles_fullpath.sort(compare)){
     svector wsjfiles=kifsys.listdirectory(a);
     for(b in wsjfiles.sort(compare)){
          if(".comp" in b && ~(".comp~")in b){
           	    println(a+_sep+b);
                    file f(a+_sep+b,'r');
                   // main.push(a+_sep+b);
                    svect=f.read();
                    string w;
                    for(w in svect)
                         main.push(w.trim());
                    f.close();
               }
     }
}

//println(main.size());
file f(wsjfolderName+_sep+"main.txt",'w');
string line;
for(line in main)
     f.writeln(line.trim());
f.close();
