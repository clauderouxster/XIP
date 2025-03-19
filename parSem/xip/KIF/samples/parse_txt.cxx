kif:

int nb=0;

frame parsing {
	int i;
	int ref;
	vector phrase;
	vector np;
	vector vp;

	function launch(string s) {
		if (i<phrase.size())
			cast(s+ref);
	}

	function addnp() {
		if (np.size()!=0 && np[-1]!=phrase[i-1])
			np.push("!");
		np.push(phrase[i]);		
	}

	function addvp() {
		if (vp.size()!=0 && vp[-1]!=phrase[i-1])
			vp.push("!");
		vp.push(phrase[i]);
	}

	//this function will implement a determiner
	autorun det() {
		waitall("det"+nb);
		addnp();
		i+=1;
		launch(phrase[i]);		
	}

	//this function will implement a noun
	autorun noun() {
		waitall("noun"+nb);		
		addnp();
		i+=1;
		launch(phrase[i]);
	}

	//This function will implement an adjective discoverer
	autorun adj() {
		waitall("adj"+nb);		
		addnp();
		i+=1;
		launch(phrase[i]);
	}

	//this function will implement a verb
	autorun verb() {
		waitall("verb"+nb);
		addvp();
		i+=1;
		launch(phrase[i]);
	}

	function _initial(string ph) {
		//we extract a vector out of our string
		phrase=ph.split(" ");
		i=0;
		ref=nb;
		//We launch our parser on the first category
		launch(phrase[i]);
		//We display the np and vp vectors
		println("NP:",np);
		println("VP:",vp);
	}
}

string s="det noun verb det adj noun";
parsing p(s);
nb+=1;
s="det adj noun verb det noun";
parsing pp(s);

