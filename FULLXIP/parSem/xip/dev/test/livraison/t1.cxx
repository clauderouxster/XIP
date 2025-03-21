function test(vector v) {
	v.push(4);
	println(v);
}


vector c=[1,2,3];

test(c);
println(c);


int i;
for (i=0;i<10;i++) {
	test([7,6,5]);
}
