vector vchemin=_paths[1].split('\')[:-1];
println(vchemin,_paths);

int cpt=0;
function testage(string s,string r) {
	if (r==s)
		println(cpt,"> Ok:",r);
	else
		println("Wrong:",s,"-->",r);
	cpt+=1;
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
     print("As expected","\n");

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
while (xdd.string() in su) 
	su=su.replace(xdd,"y");

testage("xmmm ( yxadjyxaj xtr(ajdayjdx) )",su);

su="123456789";
string scom;
while (xdd in su) 
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

f5=inframe(70,"070");
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
f1.copy(f3);
println("Copy through a function:",f1,f3);
modifie(f1);
println("Copy through a function after modification:",f1,f3);

int test_i=10;
string test_j="20";

i=10;
string j="100";
i=i+j;
j=i+j;
rsx=(test_i+test_j)+" "+i+" ici "+j+" "+test_i+" "+test_j+" "+test_j+test_i+" "+(test_i+test_j);
testage("30 110 ici 110100 10 20 2010 30",rsx);

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
	println("TRANSCONNEXE:",f.v,before,after);
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

connexe c1(10);
connexe c2(20);
connexe c3(30);

c1.addanode(100);
c1.addanode(200);
c1.addanode(300);

c1.i=10; //the transconnexe function will be automatically called

vector xvconnex;
xvconnex.push(c2);
xvconnex.push(c1);
xvconnex.push(c3);

xvconnex[1].addanode(400);
c1.i=30;


function compte(int i) {
	int j=10;
	i+=1;
	i+=1;
	i+=1;
	j+=i;
	println("j=",i,j);
	wait("Here");
	println("I=",i,j);
}

function recast() {
	cast("ICI");
}


function comptebis(int i) {
	int j=10;
	i+=1;
	i+=1;
	i+=1;
	j+=i;
	println("bis j=",i,j);
	wait("Here");
	println("bis I=",i,j);
	wait("Here","truc","machin");

	println("Finaly:",i,j);
}

for (i=5;i<20;i+=5) {
	compte(i);
	comptebis(i+10);
}
println("We come back");
println("Etats:",waiting());
try {
	cast("Heres");
}
catch() {
	cast("Here");
}
cast("machin");


function attendre(string s) {
	println("On attendra ce qu'il faudra",s);
	wait(s);
	println("On a attendu");
}

function testt(string s) {
	boolean test=false;
	int i;
	while (true) {
		if (test==false) {
			println("On attend");
			for (i=0;i<10;i+=1) {
				if (i==4) {
					println("Dans la boucle");
					attendre(s);
				}
				println("On se rejouit",i);
			}
			test=true;
		}
		else
			println("On revient");
		wait(s);
		println(s);
	}
}


testt("toto");
testt("tutu");
testt("tete");
testt("titi");


cast("titi");
cast("titi");
map attend=waiting();
println("Etats:",attend);
cast("titi");
kill();
attend=waiting();
println("Etats:",attend);
vtxt=[];
if (vtxt==[])
	println("Ok final");

println(gcsize());


