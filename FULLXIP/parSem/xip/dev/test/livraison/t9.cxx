file f('c:\xip\test\kifcxx\t1.cxx',"r");
int i;

for (i in f) 
    println(i,i.chr());

f.close("test");

f.openread('c:\xip\test\kifcxx\t1.cxx');

string s;
for (s in f)
	println(s);
