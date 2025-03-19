//string ph='DETERM(0,NOUN#4[noun:+,fonc:+,closed:+,lemme:fille,fem:+,sg:+,p3:+,surface:"à"],DET#0[lemme:à,fem:+,maj:+,startbis:+,fonc:+,closed:+,sg:+,def:+,det:+,start:+,surface:"La"]). DETERM(0,NOUN#12[noun:+,fonc:+,closed:+,lemme:voiture,fem:+,sg:+,p3:+,surface:"à"],DET#10[fonc:+,indef:+,closed:+,lemme:un,fem:+,sg:+,det:+,surface:"une"]). DETERM(0,NOUN#22[sfde:+,sg:+,desn:+,noun:+,fonc:+,closed:+,masc:+,p3:+,surface:"neveu",lemme:neveu],DET#20[fonc:+,closed:+,masc:+,sg:+,def:+,det:+,surface:"le",lemme:le]).';
string ph='DETERM(0,NOUN#2["noun":"+","fonc":"+","closed":"+","masc":"+","sg":"+","p3":"+","surface":"chien","lemme":"chien"],DET#0["startbis":"+","fonc":"+","closed":"+","masc":"+","sg":"+","def":"+","det":"+","start":"+","surface":"le","lemme":"le"]). DETERM(0,NOUN#14["noun":"+","fonc":"+","closed":"+","lemme":"nuit","fem":"+","sg":"+","p3":"+","time":"+","surface":"nuit"],DET#12["fonc":"+","closed":"+","lemme":"le","fem":"+","sg":"+","def":"+","det":"+","surface":"la"]). PREPOBJ(0,NOUN#6["proper":"+","maj":"+","noun":"+","fonc":"+","masc":"+","closed":"+","sg":"+","pren":"+","p3":"+","surface":"Jean","lemme":"Jean"],PREP#4["prepinf":"+","form":"FA","sfa":"+","fonc":"+","prep":"+","dir":"+","surface":"à","lemme":"à"]). PREPOBJ(0,NOUN#14["noun":"+","fonc":"+","closed":"+","lemme":"nuit","fem":"+","sg":"+","p3":"+","time":"+","surface":"nuit"],PREP#10["sfdans":"+","lemme":"dans","form":"FDANS","fonc":"+","prep":"+","surface":"dans"]).';

vector xv;
xv='surface":' in ph;
println(xv);
xv='surface":' in ph;
println(xv);
int i,j,ib,ibm;
for (i in xv) {
	ib=ph.byteposition(i);
	ibm=ph.byteposition(ph.find('"',i+9)+1);
	println(ib,ibm,ph.charposition(ib),i);
}


println(xv);
for (i in xv) {
	ib=ph.byteposition(i);
	ibm=ph.byteposition(ph.find('"',i+9)+1);
	println(ib,ibm);
}

string phs="La dame a mangéé une glace à la résidence.";
for (i=0;i<phs.size();i++)
	print(phs[i]," ");
println();

ph=phs+chr(0x2212);
xv=ph.ord();
println("Code=",xv,0x2212);
for (i in xv)
	print(chr(i)," ");
println();

//A map describing the styles available within the editor
map m={'#':[FL_BLACK,FL_COURIER,FL_NORMAL_SIZE ], 'truc':[ FL_DARK_RED,FL_COURIER,FL_NORMAL_SIZE ],"vert":[ FL_DARK_GREEN,FL_COURIER,FL_NORMAL_SIZE ],'noir':[ FL_BLACK,FL_COURIER,FL_NORMAL_SIZE ]};

//we modify with button one item in our map... We keep the same key
function test(button b, editor e) {
	m["rouge"]=[ FL_DARK_GREEN,FL_COURIER,FL_NORMAL_SIZE ];
	m["bleu"]=[ FL_DARK_BLUE,FL_COURIER,FL_NORMAL_SIZE ];
	m.pop("truc");
	e.addstyle(m);
	vector v=e.selection();
	e.setstyle(v[0],v[1],"rouge");
}

window w;
editor e;
button b(e) with test;

w.begin(300,200,1300,700,"Modification");
w.sizerange(10,20,0,0);		

e.create(200,220,1000,200,"Editeur");
e.addstyle(m);
e.value("This is an interesting style, which I have implemented for the fun of it...");
e.setstyle(10,22,'truc');

b.create(1230,20,30,30,FL_Regular,FL_NORMAL_BUTTON,"Ok");

w.end();
w.run();
