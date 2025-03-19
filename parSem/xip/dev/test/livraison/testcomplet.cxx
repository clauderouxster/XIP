float fxl=-1.09;

vector vchemin=_paths[1].split('\')[:-1];
println(vchemin,_paths);

strict function boucle(int i) {
	if (i==0) {
		println("Fin");
		return;
	}
	print(i," ");
	boucle(i-1);
}

boucle(10);
int cpt=0;
function testage(string s,string r) {
	if (r==s)
		println(cpt,"> Ok:",r);
	else   {
		println("Wrong:",s,"-->",r);
		exit(-1);
	}
	cpt++;
}

vector vtxt=["aa/bb/cc","ee/rr/tt"];
string bsx=vtxt[1].split("/")[0]+"=";
testage("ee=",bsx);
bsx=vtxt[1].split("/").size();
testage("3",bsx);

int bv=12;
string uvx;

testage(429981696,(bv^^8));
testage(14,(bv^2));
testage(-13,#(bv));
bv=617;
uvx=bv.format("0x%x");
println(617,bv.format("0x%x"));
int ix=10;
int iy=20;
vector vj=[ix,iy,iy+1,ix+1,ix+iy,ix*iy];
testage("[10,20,21,11,30,200]",vj);
testage("[20,21,11]",vj[ix-9:iy-16]);
testage(20,vj[ix-9+18-28+10]);
testage("[20,21,11,30,200]",vj[ix-9:]);
testage("[10,20,21,11]",vj[:iy-16]);

vector lst;
vector vmx=[1,2,3,4,5,6,7,8,9];
vector vyx=[12,23,34,45,56,67,78,90,12];
vmx+=[1,2,3,4,5,6,7,ix,iy]+vyx;
testage("[14,27,40,53,66,79,92,108,41]",vmx);

vmx=vmx[2:4];
testage("[40,53]",vmx);
vmx=vyx[2:-1];
testage("[34,45,56,67,78,90]",vmx);

vector vstring=['abc','def','ghi','jkl','def','mno'];


if (not 'ab' in vstring)
	println("Ok: ab not in ",vstring);
else
	println("Wrong: ab should not be in:",vstring);

if ('def' in vstring)
	println("Ok: def in ",vstring);
else
	println("Wrong: def should be in ",vstring);

lst='def' in vstring;
testage('[1,4]',lst);


frame subin {
	string d;

	function _initial(string x) {
		d=x;
	}

	function D() {
		return(d);
	}

	function in(self sub) {		
		if (sub.type()=="string") {
			println("TYPE=",sub.type());
			if (sub in d)
				return(true);
			return(false);
		}
		if (sub.type()=="subin") {
			println("TYPE=",sub.type());
			if (d==sub.D())
				return(true);
		}
		return(false);
	}
}

subin sub1("premier");
subin sub2("second");
subin sub3("troisieme");
vector v=[sub1,sub2,sub3];

println("SHOULD DISPLAY 3 TYPE=string");
lst="sec" in v;
if (lst)
	testage("[1]",lst);

println("SHOULD DISPLAY 1 TYPE=subin");
if (sub1 in v)
	println("Ok sub1 in v");

//We implement a trigger function
function react(self avant,self apres) {
	println("dico",avant,apres);
}
//which we associate with dico
map dico with react;

//whenever dico is modified, the "react" function is called
dico={'a':1,'b':6,'c':4,'d':6};

string xdd="123.45";
float f=xdd;
int i=xdd;
println("Conversion:",xdd,i,f);
//Boolean test, it returns true or false
if (6 in dico)
	println("As expected");

//The receiver is a list, then we return the list of indexes
lst=6 in dico;
testage("['b','d']",lst);
bool aff=false;
vector vect=[0,1,2,3,4,5,6,7,8,9];
int sz=vect;
println("Taille:",sz);
string rsx=vect[0]+" "+vect[2:3]+" "+vect[2:-2];
testage("0 [2] [2,3,4,5,6,7]",rsx);

string su="smmm ( ksadjksaj str(ajdakjds) )";
lst="(" in su;
testage('[5,20]',lst);
sz="str(" in su;
if (sz==su.find("str("))
	println("Ok: souschaine",su,"str(",sz);

while ("s" in su) 
	su=su.replace("s","x");

xdd="k";
while (xdd in su) 
	su=su.replace(xdd,"y");

testage("xmmm ( yxadjyxaj xtr(ajdayjdx) )",su);

su="123456789";
string scom;
for (xdd in su) 
	scom+=xdd+",";
testage("1,2,3,4,5,6,7,8,9,",scom);

function compare(int i, int j) {
	if (i>j)
		return(true);
	return(false);
}

vector vx=[5,4,3,2,1,11,22,33,44,55];
vx.sort(compare);
testage("[55,44,33,22,11,5,4,3,2,1]",vx);

vect.sort(compare);
testage("[9,8,7,6,5,4,3,2,1,0]",vect);
if (3 in vect with compare)
	println("OK"); 

lst=3 in vect with compare;
testage("[7,8,9]",lst);

frame inframe {
	int i;
	string si;
	string se;

	function _initial(int k,string u) {
		se=u;
		i=k;
		si="Il vaut:"+i;
	}
	//the type of the parameter can be anything
	function in(int j) {
		if (i==j) 
			return(true);
		return(false);
	}

	function string() {
		return(si);
	}

	function copy(inframe f) {
		i=f.i;
		si=f.si;
	}
}


function modifie(inframe f) {
	f.i=17;
	f.si="Desormais:"+f.i;
}

inframe f1(10,"010");
inframe f2(20,"020"); 
inframe f3(30,"030");
inframe f4(40,"040");
inframe f5(30,"050");

vect=[f1,f2,f3,f4,f5];
testage("[Il vaut:10,Il vaut:20,Il vaut:30,Il vaut:40,Il vaut:30]",vect);
if (20 in f2)
	println("Ok pour les frames");

f5=inframe((30+5)*2,"070");
println("F5 Il vaut:70:",f5,f5.se);

if (30 in vect)
	println("Ok pour les frames dans les vecteurs");
lst=30 in vect;
rsx=lst+" "+vect[lst[0]]+" "+vect[lst[1]];
testage("[2,4] Il vaut:30 Il vaut:30",rsx);

f4=vect[1];
testage("Il vaut:20",f4);
modifie(f4);
println("Duplicate and modification (all):",f4,f2,vect[1]);
self xf=vect[1];
modifie(xf);
println("Duplicate and modification (all):",f4,f2,vect[1]);
testage("Desormais:17",f4);
testage("Desormais:17",f2);
testage("Desormais:17",vect[1]);
f1.copy(f3);
println("Copy through a function:",f1,f3);
testage("Il vaut:30",f1);
modifie(f1);
println("Copy through a function after modification:",f1,f3);
testage("Desormais:17",f1);
testage("Il vaut:30",f3);

int test_i=10;
string test_j="20";

i=10;
string j="100";
i=i+j;
j=i+j;
rsx=(test_i+test_j)+" "+i+" ici "+j+" "+test_i+" "+test_j+" "+test_j+test_i+" "+(test_i+test_j);
testage("30 110 ici 110100 10 20 2010 30",rsx);
pause(0);
vect=[10,i,j,test_i,test_j,20,30,"40",50];
testage("[10,110,'110100',10,'20',20,30,'40',50]",vect);

//We first implement our anode frame
frame anode {
	int i;

	function string() {
		return(i);
	}

	function seti(int j) {
		println("J=",i,j);
		i+=j;

	}
}

//First part of our connexe frame
frame connexe {
	vector v;
}

//a trigger function, the first parameter is a connexe element
function transconnexe(connexe f,int before,int after) {
	iterator it=f.v;
	//we iterate on each element of our vector
	for (it.begin();it.nend();it.next())
		it.value().seti(before+after);
	println(f.v);
}

//Then the rest of our frame
frame connexe {
	//i is associated with our new trigger function
	int i with transconnexe;

	function _initial(int j) {
		i=j;
	}

	function addanode(int j) {
		anode xn;
		xn.i=j+10;
		v.push(xn);
	}
}


frame connexe {

	frame subconnexe {
		function addanode(int k) {
			anode xn;
			xn.i=k-20;
			v.push(xn);
		}
	}
}
println("Test sur C1");
connexe c1(10);
println("Test sur C2");
connexe c2(20);
println("Test sur C3");
connexe c3(30);

println("Addnode C1");
c1.addanode(100);
println("Addnode C2");
c1.addanode(200);
println("Addnode C3");
c1.addanode(300);

println("On appelle transconnexe");
c1.i=10; //the transconnexe function will be automatically called

vector xvconnex;
println("XConnex c2");
xvconnex.push(c2);
println("XConnex c1");
xvconnex.push(c1);
println("XConnex c3");
xvconnex.push(c3);

xvconnex[1].addanode(400);
c1.i=30;

list vlist=[1,2,3,4,5];

vlist.pushfirst(10);
vlist.pushlast(20); //display: [10,1,2,3,4,5,20]
println(vlist);
vlist.popfirst();//display: [1,2,3,4,5,20]

vector vllist=vlist; //transform a list into a vector

println(vlist,vlist[5],vlist.sum(),vlist.product(),vlist[2:5],vlist.infos("sum"));
println(vllist);

iterator it=vlist;
for (it.begin();it.nend();it.next())
	print("Value:",it.value()," ");
println();

for (ix in vlist)
	println("IX:",ix);

if (5 in vlist)
	println("Ok, 5 est bien dans la liste");
if (8 not in vlist)
	println("Ok, pas de 8 dans la liste");


//Next is a test on frames and associate functions.
frame testcallwith;

function calllocal(testcallwith tx,int before,int after) {
	println("LOCAL",tx,before,after);
}

function callframe(testcallwith tx,int before,int after) {
	println("FRAME",tx,before,after);
}


//We declare a frame with an associate function callframe
frame testcallwith with callframe {
	//a local associate function
	int i with calllocal=10;
	int j=30;

	function string() {
		return(i);
	}
}

function callvariable(testcallwith tx,int before,int after) {
	println("VARIABLE",tx,before,after);
}
println("Variables....");
//this variable is associated with an associate variable function
testcallwith callt with callvariable; //This simple declaration will trigger calllocal associated with i (first initialisation)
testcallwith callt2; //This simple declaration will also trigger calllocal associated with i (first initialisation)

callt.i=15; //This modification will trigger calllocal associated with i
callt2.i=20; //This modification will trigger calllocal associated with i
callt.j=15; //This modification will trigger callvariable associated with callt
callt2.j=20; //This modification will trigger callframe associated with testcallwith frame declaration

frame autreclasse {
	int k=100;
	function string() {
		return("autreclasse");
	}
}

frame classe {
	int i=-10;
	autreclasse a;
	
	function local() {
		println("local");
	}

	function local(int i) {
		println("local bis:"+i);
	}

	function string() {
		return("classe");
	}
	

	frame sousclasse {
		int j;

		function local() {
			println("souslocal");
		}

		function local(int i) {
			println("souslocal bis:"+i);
		}

		function string() {
			return("sousclasse");
		}

	}
}

function testmultipledeclaration(string s, autreclasse v) {
	println("Autre:",s,v);
}


function testmultipledeclaration(string s, classe v) {
	println("Classe:",s,v);
}


function testmultipledeclaration(string s, string v) {
	println("String:",s,v);
}

function testmultipledeclaration(int s, int v) {
	println("Int:",s,v);
}

function testmultipledeclaration(float s, float v) {
	println("Float:",s,v);
}

string s1="s1";
string s2="s2";

float fl=10.4;
float gl=23.4567;
int iii=10;
int jjj=20;
v.clear();
v[0]=10;

classe cl;
sousclasse scl;
autreclasse a;

cl.local();
cl.local(100);
scl.local();
scl.local(10);

testmultipledeclaration(s1,scl);
testmultipledeclaration(s2,cl.a);
testmultipledeclaration(cl.i,a.k);
testmultipledeclaration(s1,a);
testmultipledeclaration(fl,gl);
testmultipledeclaration(fl,v[0]);
testmultipledeclaration(s1,s2);
testmultipledeclaration(iii,jjj);

call func;
func=testmultipledeclaration;
func(s1,scl);
func(s2,cl.a);
func(cl.i,a.k);
func(s1,a);
func(fl,gl);
func(fl,v[0]);
func(s1,s2);
func(iii,jjj);


//Test on multiple pops
//A popped element should not be destroyed by Resetreference

frame framepop {
	int i;

	function _initial(int j) {
		i=j;
	}

	function string() {
		return(i);
	}
}


framepop tframepop(0);
v.push(tframepop);

for (i=1;i<1000;i++) {
	tframepop=framepop(i);
	v.push(tframepop);
}
println(v[-1]);
for (i=0;i<1000;i++) {
	tframepop=v.pop();
	print(tframepop,",");
}
println();
println("------------------------------------");
for (i=999;i>=0;i--) {
	tframepop=framepop(i);
	v.push(tframepop);
}

for (i=0;i<1000;i++) {
	print(v[i],",");
}
println();
v.clear();
println("T=",tframepop);
tframepop=null;
if (tframepop==null)
	println("oui...");
tframepop=framepop(1000);
println(tframepop);


frame top {
	vector vect;
	string str="empty";
	int sze=0;

	function _initial(int i) {
		println("I=",i);
	}

	function laststr() {
		println("LAST:",str);
	}

	function addd(string s) {
	     println("ADDED IN TOP FRAME");
	     vect.push(s);
	     sze+=1;
		 println("SIZE:"+sze);
		 str=s;
	}

	function int() {
		return(sze);
	}

	function in(self s) {
		if (s in vect) 
			return(true);
		return(false);
	}

	function equal(int a) {
		if (sze==a)
			return(true);
		return(false);
	}

	function display() {
		println("DISPLAY TOP:",sze);
		iterator it=vect;
		for (it.begin();~it.end();it.next()) 
			println("TOP VECT="+it.key()+":"+it.value());
		println("END TOP==================");
	}
	
	function calldisplay(top x) {
		x.display();
	}

	frame sub {
		string substr="sub";
		
		function display() {
			println("DISPLAY SUB:",substr);
			iterator it=vect;
			for (it.begin();~it.end();it.next()) 
				println("SUB VECT="+it.key()+":"+it.value());
			println("END SUB==================");
		}
	}
}

top topframe(1);
sub subframe(2);


topframe.addd("VALUE TOP 1");
topframe.addd("VALUE TOP 2");
topframe.addd("VALUE TOP 3");

subframe.addd("VALUE SUB 1");
subframe.addd("VALUE SUB 2");
subframe.addd("VALUE SUB 3");
subframe.addd("VALUE SUB 4");

topframe.display();
subframe.display();

topframe.calldisplay(topframe);
topframe.calldisplay(subframe);

kif number('c:/xip/test/kifcxx/numbers.cxx');

vector vl=number.test();
testage("[1529]",vl);
map dyndico={(i+10)*3:"y",rsx+"_U":uvx};
testage("{'3030':'y','30 110 ici 110100 10 20 2010 30_U':'0x269'}",dyndico);

function conv(self s) {
	s=s.upper();
}

vchemin=["ab","cd","ef","gh"];
vchemin.apply(conv);
println(vchemin);
bsx="légers sont les champs";
bsx=bsx.utf8();
println(bsx);
println(gcsize());
