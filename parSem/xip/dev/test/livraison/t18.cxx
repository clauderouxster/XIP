
int compte=0;

frame tst {
	int i;
	string s;

	function _initial() {
		i=compte;
		s=compte;
		compte++;
	}

	function string() {
		return(s);
	}
}


vector v;
int i;
for (i=0;i<10000;i++) {
	tst t;
	v.push(t);
}
println(gcsize()["garbage"],gcsize()["cursor"]);

for (i=0;i<9000;i++)
	v.pop();
println(gcsize()["garbage"],gcsize()["cursor"]);

for (i=0;i<1000;i++) {
	tst t;
	v.push(t);
}
println(gcsize()["garbage"],gcsize()["cursor"]);

for (i=0;i<v;i++)
	println(v[i]);
println(gcsize()["garbage"],gcsize()["cursor"]);

for (i=0;i<1000;i++) {
	tst t;
	v.push(t);
}
println(gcsize()["garbage"],gcsize()["cursor"]);

for (i=0;i<1000;i++) {
	tst t;
	v.push(t);
}
println(gcsize()["garbage"],gcsize()["cursor"]);

for (i=0;i<1000;i++) {
	tst t;
	v.push(t);
}
println(gcsize()["garbage"],gcsize()["cursor"]);

for (i=0;i<1000;i++) {
	tst t;
	v.push(t);
}
println(gcsize()["garbage"],gcsize()["cursor"]);

for (i=0;i<1000;i++) {
	tst t;
	v.push(t);
}
println(gcsize()["garbage"],gcsize()["cursor"]);
for (i=0;i<8000;i++) {
	tst t;
	v.push(t);
}
println(gcsize()["garbage"],gcsize()["cursor"]);
