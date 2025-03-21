function test(int m) {
	if (m==10) {
		map mm;
		mm[10]=20;
		return(mm);
	}
	return({});
}


map m;

m=test(10);
println(m);

