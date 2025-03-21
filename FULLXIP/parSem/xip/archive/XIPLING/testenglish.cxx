file source('C:\eclipse\XIPLING\lingvataenglish.src','r');
fst lenglish('lingvataenglish.fst',"",0,false);

string s;
vector trad;

for (s in source) {
	s=s.replace(' ','');
	s=s.trim();
	if ("+En" not in s) {
		trad=lenglish.up(s);
		println(trad);
		if ("+?" in trad[0])
			exit(-1);	
	}
}

