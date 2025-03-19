use('LINUX','/opt/ParSem/users/Roux/xip/parSem/xip/dev/xipFst/lib/linux/sharedLib/kifltk');


vector v=[1,2,3];

vector vv;

vv.push(v);

v=vv;
println(v,_OS);
