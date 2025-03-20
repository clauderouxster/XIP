//Debut
kif:

string endl="\n\r";
int i=10;
//Premier test
print("I=10:"+i+endl);



map tabmap;

tabmap[12+34]=25;

boolean testbool=false;
if (testbool==false) {
	print("YOOOOOOOOOOOOOOOOOOOOO"+endl);
}

int ij = 100+10;

print("IJ=========================="+ij+endl);

int wl;
map testmap;

file mydoc;

openwrite(mydoc,"C:\\XIP\\XIP7\\kif\\testage.txt");

write(mydoc,"Essai",endl,"truc");

close(mydoc);


function myfunc(int g0) {
	int b0=120;
	print("B0="+b0+endl);
	b0 = g0+10;
	print("BO====="+b0,endl);
	for (b0=0;b0<10;b0=b0+1) {
		if (b0==3) {
			continue;
		}
		print("B0:"+b0+endl);
	}
	return(b0);
}

frame bis {
	vector mapk1;
	int j;
	
	
	function funcbis(int h) {
		int k;
		k=h+j;
		return(k);
	}
}

frame test {          
	int entier2;
	string chaine2;
	bis you2;

	function decompte(int l) {
		print("L="+l+endl);
		if (l==0) {
			return(1);
		}
		decompte(l-1);
		print("l était: "+l+endl);
	}
	
	
	
	function func(int g2) {
		int u2;
		u2 = entier2+g2;
		return(u2);
	}
	function String() {
		return(chaine2);
	}
	frame tutu {
		int k3;
		string ss3;
	}

	function fonc(tutu essai) {
		essai.entier2 = 1234;
		print("Ici on teste l'echange de variables frame:"+essai.entier2+endl);
	}
	function retourne() {
		tutu tata;
		tata.entier2=100;
		return(tata);
	}
}

tutu toto;
test xx;
tutu groupe;
map tititi;
bis machin;
string mach;

toto = xx.retourne();
print("Toto:"+toto.entier2+endl);

vector decoupe;
vector recoit;

split("a.b.c.d.e",".",decoupe);
mach="absjdjca";
ij=decoupe;
print("Taille:"+(ij+decoupe)+endl);

print("Val:"+decoupe[3]+endl);
for (ij=0;ij<decoupe;ij=ij+1) {
	print("Decoupe:"+ij+":"+decoupe[ij]+endl);
}

for (ij=0;ij<10;ij=ij+1) {
	if (ij==5) {
		break;
	}
	print("Ij="+ij+endl);
}

ij=10;
while (ij>0) {
	print("IJ while:"+ij+endl);
	ij=ij-1;
}



ij = "c" in mach;
print("c in mach:"+ij+endl);
if ("a" in mach) {
	print("YEEE"+endl);
}

recoit =  "a" in decoupe;


print("Decoupe:"+decoupe[3]+endl);
testmap["a"] = 10;
testmap["b"] = 11;
testmap["c"] = 12;

if (testmap["a"]==null) {
	print("Erreur...endl");
}

vector titivect;
titivect = testmap;
breakpoint;
for (ij=0;ij<titivect;ij=ij+1) {
	print("Clef:"+titivect[ij]+endl);
}

tititi = testmap;

toto.decompte(5);

toto.you2.mapk1[0] = 908;

toto.you2.j = 234+(56/10);
wl = toto.you2.funcbis(100);


wl = 10;
wl = 100;


xx.chaine2 ="ESSAI";
xx.chaine2 ="TRUC";
print("Test:"+xx+endl);

wl = 2;
if (wl==1 | wl==2) {
	wl =10;
	print("Toto=="+wl+endl);
}

print("Essai:"+wl+endl);


toto.you2.mapk1[1] = 12;
print("Ici et la:"+(toto.you2.mapk1[1]+toto.you2.mapk1[0])+endl);

wl = myfunc(10);

toto.chaine2 = "MVOUIS";
tutu tito=toto;
tito.entier2 = 100;

int youglou = toto.you2.mapk1[0];

tito.entier2 = 200;
tito.chaine2 = "kk";
toto.entier2 = 12+23*4;
toto.entier2 = 12;
groupe = toto;

print("Ici:"+(toto.you2.mapk1[1]+toto.you2.mapk1[0])+endl);

print("Groupe:"+groupe.entier2+endl);
	  
print("Valeurs:"+(toto.entier2+tito.entier2)+endl);

xx.fonc(toto);

wl = toto.func(24);
print("WL=="+wl+endl);
toto.k3==10;

print("Finalement:"+toto.entier2+endl);

clean(toto.you2.mapk1);

