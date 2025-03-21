#!/usr/bin/KiF

socket s;

println(s.gethostname());
s.createserver(2012,10);
iterator it;
map m={'a':1,'b':2,'c':3};
vector v;
int client;

client=s.wait();
v=s.read(client);
println(v);
s.write(client,m);

s.close();
