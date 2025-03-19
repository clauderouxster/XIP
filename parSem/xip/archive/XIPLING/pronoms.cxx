map sem={'e':'loc','i':'temp','a':'','o':'proper','u':'num','X':''};
map cases={'Nom':'','Acc':'n','Dat':'d','Gen':'s'};
map genres={'k':'','f':'fem','m':'masc'};
string s,c,C,g,sfeat;
bool one=true;
map features;
for (g in genres.keys()) {
	for (s in sem.keys()) {
		if (s=='X')
			s='';
		for (c in cases.keys()) {
			for (C in cases.keys()) {
				string root=g+s+"i"+cases[c]+"er"+cases[C];
				map feat;
				if (genres[g]!="")
					feat["antecedentgender"]=genres[g];
				if (s!='')
					feat["antecedentcase"]=c;
				feat["case"]=C;
				feat["suffix"]="i"+cases[c]+"er"+cases[C];
				feat["basis"]=feat["suffix"];
				feat["root"]=g+s;
				feat["pos"]="Pron";
				if (s!='' && sem[s]!="")
					feat[sem[s]]="+";
				print("basicwords['"+root+"']=",feat+";\r");
				sfeat=feat["suffix"];
				if (null==features[sfeat])
					features[sfeat]=feat;
			}
			if (s=='')
				break;		
		}
	}
}

for (sfeat in features) {
	features[sfeat]["root"]="ka";
	print("onending['"+sfeat+"'] =",features[sfeat]+";\r");
}
