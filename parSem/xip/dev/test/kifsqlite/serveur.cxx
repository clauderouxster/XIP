#!/usr/bin/KiF

socket s;

println(s.gethostname());
s.createserver(2012);
iterator it;
map m={'a':1,'b':2,'c':3};
vector v;
int cpt=3;

while (cpt>0) {
	cpt-=1;
	s.wait();
	v=s.read();
	println(v);
	s.write(m);
}
s.close();
