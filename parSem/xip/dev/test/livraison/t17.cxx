
file f('C:\XIP\Test\kifcxx\codes.txt',"r");

string s="MethodInitialization";
int base=s.size();
int p,pp;
string champ,ch;
for (s in f) {
	s=s.trim();
	p=',"' in s;
	pp='(' in s;
	ch=s[base:pp];
	if (champ!=ch) {
		champ=ch;
		println(champ);
	}
	if (p!=-1)
		print(s[p+2:-3],"\r");
}
