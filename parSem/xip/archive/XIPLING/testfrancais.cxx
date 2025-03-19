file source('C:\eclipse\XIPLING\lingvatafrench.src','r');
fst lfrench('lingvatafrench.fst',"",0,false);

string s;
vector trad;

for (s in source) {
	s=s.replace(' ','');
	s=s.trim();
	if ("+Fr" not in s) {
		trad=lfrench.up(s);
		println(trad);
		if ("+?" in trad[0])
			exit(-1);	
	}
}

