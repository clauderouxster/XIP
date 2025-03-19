//***************************************************************
string wsjfolderName = '/home/fnoorala/Desktop/hmmComparison/wsj';
string mainfilepath = wsjfolderName + "/main.txt";
//***************************************************************
file f(mainfilepath, 'r');
svector svect = f.read();
f.close();


string ln;
svector member;
svector adj;
for (ln in svect){
	//remove line spaces
	// expression for discared lines with ==>
	if (ln != '' && ~('==>' in ln))
		// we split the everyline based on "/"
		member = ln.trim().split("/");

	//convert NNPS-->NNP and NNS -->NN
	if (member[1] == 'NNPS') member[1] = 'NNP'; //println(member[0]+" "+member[1]+" "+member[3]);}
	if (member[1] == 'NNS') member[1] = 'NN';//println(member[0]+" "+member[1]+" "+member[3]);}

	//make equal if ambiguity parts are similar
	if (member[1].regexip("?+|?+") && member[3].regexip("?+|?+"))
	if (member[1][:"|"][:-1] in member[3] && member[1]["|":][1:] in member[3]) member[1] = member[3];

	//if ambiguity solved by xip and the tag in xip included in PTB tag then make them equal
	if (member[1].regexip("?+|?+") && ~member[3].regexip("?+|?+"))
	if (member[3] in member[1]) member[1] = member[3];

	adj.push(member[0] + "/" + member[1] + "/" + member[3]);
}


file r(wsjfolderName + _sep + "adjusted-main.txt", 'w');
iterator it = adj;
for (it.begin(); it.nend(); it.next())
r.writeln(it.value().trim());
