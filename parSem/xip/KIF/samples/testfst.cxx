kif:
map voc;
vector v;


voc["abc\t+Masc+Noun"]="abc";
voc["qabc\t+Rel+Verb"]="qabc";
voc["eabc\t+Masc+Noun"]="eabc";
voc["rabc\t+Masc+Noun"]="rabc";
voc["yabc\t+Masc+Noun"]="yabc";
voc["iabc\t+Masc+Noun"]="iabc";
voc["zabc\t+Masc+Noun"]="zabc";
voc["xabc\t+Masc+Noun"]="xabc";

fst tokx;
string s=_paths[0]+'\essaix.fst';
print("FST: ",s,"\n");
tokx.compile(voc,s);
v=tokx.up("qabc");
print("V=",v,"\n");


fst tok;
s=_paths[0]+'\essai.fst';
print("FST: ",s,"\n");

tok.init();
iterator it;
it=voc;
for (it.begin();it.nend();it.next())
	tok.add(it.value(),it.key());
tok.save(s);

v=tok.up("qabc");
print("V=",v,"\n");

fst myfst;
map words;
vector res;

words["toto\t+Noun"]="titi";
words["titi\t+Noun"]="tutu";

//In this case, a text file: myfile.fst.txt will be created before compiling the FST
myfst.compile(words,"myfile.fst");
res=myfst.up("titi");
print("Res=",res,"\n");	//display: [‘toto	+Noun’]

fst myfstbis;
iterator it=words;
//Another possibility, to avoid the file creation is to create the FST line by line...
//First we prepare our FST
myfstbis.init();
//Then we add line after line: surface,lemma
for (it.begin();it.nend();it.next())
	myfstbis.add(it.value(),it.key());
//Eventually we save it
myfstbis.save("myfstbis.fst");
res=myfstbis.up("tutu");
print("Res=",res,"\n");	//display: [‘titi	+Noun’]
pause(100);
