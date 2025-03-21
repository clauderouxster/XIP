string s="C'était une nuit étoilée à en être éclaté";
string c;

for (c in s)
	print(c," ");
println();
riterator i=s;
for (i.begin();i.nend();i.next())
	print(i.key(),i.value()," ");
println();
vector v=s.bytes();
println(v);
c=v.bytes();
println(c,c.size());

v=[47,47,32,82,117,108,101,32,56,13,10];
s=v.bytes();
s=s.replace("R","TRES");
v=s.bytes();
println(s,v);
