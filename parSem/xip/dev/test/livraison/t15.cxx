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

vector vect=[f1,f2];

f1=vect[1];
println(f2,f1);
