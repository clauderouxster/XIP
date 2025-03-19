file f('C:\XIP\Test\kifcxx\codes.txt',"r");
string s;
vector v,vv;
int i=0;
for (s in f) {
	s=s.trim();
		v=s.split(" ");
		v=v[1].split("\t");
		print('slidertype["'+v[0]+'"]='+v[0]+";\r");
	i++;
}
