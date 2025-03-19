
frame test {
	int i;
	vector v;
	function _initial(int xi,int xj) {
		i=xi;
		v.push(xj);
	}

	function string() {
		string s=i;
		s+=v;
		return(s);
	}
}

frame othertest {
	int k;
	function test() {
		test t(k,k);	
		return(t);
	}
}


othertest o;
o.k=100;

test o1(10,20);
test o2(100,200);

#ici
o1=o2;
o1.v.push(100);
println(o1,o2);

o1=o;
println(o1);
