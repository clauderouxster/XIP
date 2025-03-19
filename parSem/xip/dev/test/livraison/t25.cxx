vector v=[['a','b'],[1,2],['u','v','c'],[10,13]];

if (['u','v'] not in v)
	println("Non");
else
	println("ok");

map m={1:{'a':2,'b':3},2:[1,2],4:{'u':5,'v':6},6:[10,13]};
println(m);

if ({'u':5,'v':7} not in m)
	println("Non");
else
	println("ok");

pause(10);
