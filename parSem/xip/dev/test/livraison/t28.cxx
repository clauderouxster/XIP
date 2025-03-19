frame test {
	int i;
	string s="OSCAR";
	vector v=[1,2,3];

	function affiche() {
		println(i,s,v);
	}
	
	function long() {
		return(i);
	}

	function string() {
		return(s);
	}


	frame subtest {
		int k;
		function affiche() {
			println(k);
		}
	}
}

test a;
subtest b;
a.i=10;
b.k=100;

a.affiche();
b.affiche();

pause(10);
