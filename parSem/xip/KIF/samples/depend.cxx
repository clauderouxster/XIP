
function tess(string oldvalue,string newvalue) {
	println("tess",oldvalue,":",newvalue);
}

function essai(string oldvalue,string newvalue) {
	//we then associate i with a function, which will trigger a call to tess at each call to essai
	int i with tess=10; //first call to tess, i is initialized with 10
	//twice actually...
	i=100;//we call tess again
	println("Changer la Valeur:",oldvalue,":",newvalue);
}


string ef;
string sd="10";
//essai is again called twice, once for the initialisation of s with "10"
string s with essai="10";

//then again with new value
s="new value";
println("s=",s);

function essaiint(int oldvalue,int newvalue) {
	println("Changer la Valeur:",oldvalue,":",newvalue);
}

//We can also use such a function into a frame, with one specific constraint: it cannot be a frame method
frame test {
	//first call when x is initialized
	int x with essaiint=100;
}


test toto;
//second call
toto.x=555;

test tutu;
//second call
tutu.x=1000;

println("TOTO:",toto.x," ","TUTU:",tutu.x);
pause(100);

