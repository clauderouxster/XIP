

//we loop in a file
string s;
int i;

i=0;
while (i!=10)
	i++;

i=0;
while (i < 5)
	i++;

string p="10";


file f('c:\xip\test\kifcxx\t1.cxx',"r");
for (s in f)
	println(s);


//we loop in a vector of ints...
vector v=[1,2,3,4,5,6];

for (i in v)
	println(i);


map m={"a":1,"b":2,"c":3,"d":4,"e":5,p:6};

for (s in m.keys())
	println(s);

for (i in m)
	println(i);

for (i in [1,2,3,4,5,6,7,8,9,p])
	println(i);

