frame test {
	int i;
	
	function int() {
		return(i);
	}

	function string() {
	    return(i);
	}
}


frame othertest {
	int k;
	function test() {
		test t;	
		t.i=k;	
		return(t);
	}
}

othertest o;
o.k=100;
test tst=o;
println(tst);

int k=10+tst;
println(k);

//s is initialized with a map description
string s={'gender':"masc",'pers':2};
map m=s;
println(s,m);
