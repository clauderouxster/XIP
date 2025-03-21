vector v;
vector cats;
vector terms;
vector Categories;
vector Terms;
map offsetbeg;
map offsetend;
map CategSentiment;
map TermSentiment;

parser p('/home/brun/parSem/xip/dev/GRAMMARS/ENGLISH/APPLICATIONS/SCOOP/GRMFILES/scoop-restaurant.grm');

function readXml(xml n, self s){
	if(n.name()=="text"){
		v.push(n);
	}
	elif(n.name()=="aspectCategories"){
		cats.push(n);
	}
	elif(n.name()=="aspectTerms"){
		terms.push(n);
	}
}

function getdependencies(dependency d){
	if(d.name()=="OPINION-ON-ASPECT-CATEGORY"){
		vector v1 = d.parameters();
		node n1 = v1[0];
		string s1 = n1.surface();
		Categories.push(n1);
		if(d.feature("NEGATIVE","+")==1)
			CategSentiment[s1] = "NEGATIVE";
		elif(d.feature("POSITIVE","+")==1)
			CategSentiment[s1] = "POSITIVE";
		elif(d.feature("CONFLICT","+")==1)
			CategSentiment[s1] = "CONFLICT";
		elif(d.feature("NEUTRAL","+")==1)
			CategSentiment[s1] = "NEUTRAL";
		println("Category is ", n1.surface(), d.features(),"--->", CategSentiment[s1]);
	}
	if(d.name()=="OPINION-ON-ASPECT-TERM"){
		vector v2 = d.parameters();
		node n2 = v2[0];
		string s2 = n2.surface();
                int left,right;
                n2.offset(left,right);
		Terms.push(n2);
                
		if(d.feature("NEGATIVE","+")==1)
                    TermSentiment[s2] = ["NEGATIVE",left,right];
		elif(d.feature("POSITIVE","+")==1)
			TermSentiment[s2] = ["POSITIVE",left,right];
		elif(d.feature("CONFLICT","+")==1)
			TermSentiment[s2] = ["CONFLICT",left,right];
		elif(d.feature("NEUTRAL","+")==1)
			TermSentiment[s2] = ["NEUTRAL",left,right];
		println("Term is ", n2.surface(), d.features(),"--->", TermSentiment[s2]);
	}
}

string rule=@"
script:


if(OPINION-ON-ASPECT-TERM$2(#1)) {
	getdependencies($2);
}

if(OPINION-ON-ASPECT-CATEGORY$1(#1)) {
	getdependencies($1);
}

"@;

p.addendum(rule,true);

// input xml file
xmldoc inputXml with readXml;
//inputXml.load('/opt/ParSem_db/Corpora/SemEval14/Restaurants/Restaurants_Train.xml');
inputXml.load(_args[0]);

// output xml file
//xmldoc outputXml;
//outputXml.create(inputXml.node().name());
//xml nd=outputXml.node();
//xml nnn(inputXml.node().child().name());
//nd.child(nnn);
//nnn.new("titi");
//nnn.content("Toto is happy");
//nd.child(nnn);
//outputXml.save('/home/dpopa/Work/SemEval/data/Restaurants/Restaurants_Train_XRCEresultV1.xml');


// get the actual contents of the nodes corresponding to the "text" tag into svector sv
xml n;
for(n in v){
	if(n!=null){
		string s;
		s = n.content();
		if(s!=null){
			Categories.clear();
			CategSentiment.clear();
			Terms.clear();
			TermSentiment.clear();
			println("Start..");
			// parse each string (each text node content) and get the dependencies related to it
			p.parse(s);
			println(CategSentiment, Categories);
			vector keys;
			string CurrentKey;
			string CurrentValue;
			if(TermSentiment.size()>0){
				xml ParentNode = n.parent();
				xml sib;
				// add new child "aspectTerms"
				sib.new("aspectTerms");
				keys = TermSentiment.keys();
				for(CurrentKey in keys){
					// add new child "aspectTerm"
					xml sib2;
					sib2.new("aspectTerm");
					ssprimemap ssm;
					// add category
					ssm["term"] = CurrentKey;
					// add polarity
					CurrentValue = TermSentiment[CurrentKey][0].lower();
                                        ssm["polarity"] = CurrentValue;
                                        ssm["from"]=TermSentiment[CurrentKey][1] ;
                                        ssm["to"]=TermSentiment[CurrentKey][2] ;
                                        println(ssm);
					println("Term Key: ", CurrentKey, " Value: ", CurrentValue);
					sib2.properties(ssm);
					sib.child(sib2);
				}
				ParentNode.child(sib);
			}


			if(CategSentiment.size()>0){
				xml ParentNode = n.parent();
				xml sib;
				// add new child "aspectCategories"
				sib.new("aspectCategories");
				keys = CategSentiment.keys();
				for(CurrentKey in keys){
					// add new child "aspectCategory"
                                   
                                    xml sib2;
                                    sib2.new("aspectCategory");
                                    ssprimemap ssm;
                                    // add category
                                    ssm["category"] = CurrentKey;
                                    // add polarity
                                    CurrentValue = CategSentiment[CurrentKey].lower();
                                    ssm["polarity"] = CurrentValue;
                                    println("Category Key: ", CurrentKey, " Value: ", CurrentValue);
                                    sib2.properties(ssm);
                                    sib.child(sib2);
				}
				ParentNode.child(sib);
			}

		}
	}
}
xml c,t;
for(c in cats){
	c.delete();
}
for(t in terms){
	t.delete();
}
inputXml.save('/opt/ParSem_db/Corpora/SemEval14/tools/Restaurants_Train_XRCEresultV1.xml');
inputXml.close();


