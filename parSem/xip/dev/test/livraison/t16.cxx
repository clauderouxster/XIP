
function modif(self t) {
	if (t)
		println(t);
	t=false;
	println(t);
}
int i;
for (i=0;i<3;i++)
	modif(true);


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
vector lst;

println("SHOULD DISPLAY 3 TYPE=string");
lst="sec" in v;
println(lst);
