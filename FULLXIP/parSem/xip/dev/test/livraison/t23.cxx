frame add {
	int k;

	function _initial(int i) {
		k=i;
	}

	//We re-declare the “+” operator
	function +(add a, add b) {
		if (a==this) {
			k+=b.k;
			return(this);
		}
		add c(0);
		c.k=a.k+b.k;
		return(c);
	}
	function string() {
		return(k);
	}
}

add a(10),b(2),c(0);
c=a+b;
println(c);

c+=b;
println(c);
