#!/usr/bin/KiF
socket s;

string name=s.gethostname();
println(name);
s.connect(name,2012);
vector v=[1,2,3,4,5];
s.write(v);
map m=s.read();
println(m);
s.close();
