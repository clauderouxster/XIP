//**********************************Comparision function*********************/
//functions
function erase_spaces(string s) {
     return (s.split(" ").join(""));
}

function first_remove(self s,svector v){
     if (s.regexip("?*`?*")) v.push(s.split("`").join(""));
     else if (s.regexip("?*'?*")) v.push(s.split("'").join(""));
          else v.push(s);
}

function second_remove(self s,svector v){
     if (s.regexip('?*"?*')) v.push(s.split('"').join(""));
     else v.push(s);
          }

function remove_slash(self s, svector v){
     if (s!='/') v.push(s);
}

function remove_space(self s,svector v){
     if('\/' in s) s=s[:'\/'][:-2]+'\'+s['\/':][2:];
     if('---' in s || '--' in s) s='';
     if(s!='' && '/' in s && s.regexip("?+%/?+")) {
          v.push(s[:'/'][:-1]);
          v.push(s['/':][1:]);
     }
}


//**************************main function****************************

function compare(string wsjfile,string xipfile,string folder) {

     //read wsj file and create vecor of word,tag
     //string wsjfile='/home/fnoorala/Desktop/hmmComparison/wsj/01/std/wsj_00600.pos.txt';
     //string xipfile='/opt/ParSem_db/Corpora/MapHMM2treebank2/wsj/01/mapping2/wsj_0100.hmm.map';
     //string folder='/home/fnoorala/Desktop/hmmComparison/wsj/01/kifcompare';
     //read wsj file
     file f1(wsjfile,'r');
     svector vect=f1.read();
     f1.close();

     svector wsj1;
     vect.apply(first_remove,wsj1);

     vect.clear();
     wsj1.apply(second_remove,vect);

     wsj1.clear();
     vect.apply(remove_slash,wsj1);

     svector wsj;
     wsj1.apply(remove_space,wsj);

     //read xip file


     file f2(xipfile,'r');
     svector vect2=f2.read();
     f2.close();

     svector xip1;
     vect2.apply(first_remove,xip1);

     vect2.clear();
     xip1.apply(second_remove,vect2);

     xip1.clear();
     vect2.apply(remove_slash,xip1);

     svector xip;
     xip1.apply(remove_space,xip);

     //compare word in wsj with correspondence word in xip


     int wsjsize=wsj.size();
     int xipsize=xip.size();

     //parameters definition and initialization
     int acc1=0;
     int acc2=0;

     int i=0;
     int j=0;

     string str1='';
     string str2='';

     list st;
     string tag1='';
     string tag2='';

     while (i< wsjsize && j< xipsize) {
          int x=acc1+erase_spaces(wsj[i]).size();
          int y=acc2+erase_spaces(xip[j]).size();

          if(x < y) {

               acc1=acc1+erase_spaces(wsj[i]).size();
               str1=str1.trim()+" "+wsj[i];
               tag1=tag1.trim()+","+wsj[i+1];
               i+=2;

          }
          elif(x > y) {
               acc2=acc2+erase_spaces(xip[j]).size();
               str2=str2.trim()+" "+xip[j];
               tag2=tag2.trim()+","+xip[j+1];
               j+=2;
          }

          else{

               if(erase_spaces(wsj[i]).size() < erase_spaces(xip[j]).size())   {
                    str1=str1.trim()+" "+wsj[i];
                    tag1=tag1.trim()+","+wsj[i+1];

                    st.pushlast(str1.trim()+"/"+tag1[1:].trim()+"==>"+j); //j/2
               }
               else if(erase_spaces(wsj[i]).size() > erase_spaces(xip[j]).size())  {
                    str2=str2.trim()+" "+xip[j];
                    tag2=tag2.trim()+","+xip[j+1];

                    st.pushlast(wsj[i].trim()+"/"+wsj[i+1].trim()+"==>"+i); //i/2
               }
               else {
                    if(wsj[i]==xip[j])
                         st.pushlast(wsj[i]+"/"+wsj[i+1].trim()+"/"+xip[j]+"/"+xip[j+1]);

               }


               i+=2;
               j+=2;

               acc1=0;
               acc2=0;

               str1='';
               str2='';

               tag1='';
               tag2='';

          }
     }

     file ff(folder+_sep+wsjfile[-12:][:-8]+".comp",'w');
     string word;
     for(word in st)
          ff.writeln(word.trim());
     ff.close();
}


//*****************Call function to create related files in specific folders**************


string wsjfolderName='/home/fnoorala/Desktop/hmmComparison/wsj';
string xipfolderName='/opt/ParSem_db/Corpora/MapHMM2treebank2/wsj';

svector wsjv=kifsys.listdirectory(wsjfolderName);
svector xipv=kifsys.listdirectory(xipfolderName);

svector xipfilepath;
svector wsjfilepath;

string y;
for (y in wsjv) {
     if(y.regexip("%d%d")) {

          //create compare directory in each wsj directory
          string path=wsjfolderName+_sep+y+_sep+'kifcompare';
          kifsys.createdirectory(path);
          //vector of path for wsj files
          wsjfilepath.push(y+_sep+'std');
     }
}

string x;
for (x in xipv){
     if(x.regexip("%d%d")){
          //vector of path for xip files
          xipfilepath.push(x+_sep+'mapping2');
     }
}
//println(xipfilepath);
//full path

svector xipfiles_fullpath;
svector wsjfiles_fullpath;
int ii;
for(ii=0;ii<wsjfilepath.size();ii++){
     xipfiles_fullpath.push(xipfolderName+_sep+xipfilepath[ii]);
     wsjfiles_fullpath.push(wsjfolderName+_sep+wsjfilepath[ii]);
}

//println(xipfiles_fullpath);
//becareful if you changed the folder names
string a;
string b;
svector mypath;
for (a in xipfiles_fullpath) {
     for (b in wsjfiles_fullpath) {
          //println(a.split("/").join("")[-10:-8]);
          if (a.split("/").join("")[-10:-8]==b.split("/").join("")[-5:-3]) {
               mypath.push(a+"|"+b);

          }
     }
}

//println(mypath);

int i=1; //identify the id for directory which one to apply 00-->0 01-->1
//for (i=0;i<=mypath.size();i++){
string wsjpath=mypath[i]["|":][1:];
string xippath=mypath[i][:"|"][:-1];
svector filespath;
svector xipfiles=kifsys.listdirectory(mypath[i][:"|"][:-1]);
svector wsjfiles=kifsys.listdirectory(mypath[i]["|":][1:]);
for (a in xipfiles)
     if (".hmm.map" in a && ~".hmm.map~" in a)
          for (b in wsjfiles)
               if(".pos.txt" in b && ~".pos.txt~" in b)
                    if (a[:8][-4:]==b[:8][-4:])
                         filespath.push(xippath+_sep+a+"|"+wsjpath+_sep+b);




//}

svector pairs;
for(i=0;i<filespath.size();i++){
     string file1=filespath[i]["|":][1:];
     string file2=filespath[i][:"|"][:-1];
     println(i);
     compare(file1,file2,wsjpath[:-4]+_sep+"kifcompare");

}
