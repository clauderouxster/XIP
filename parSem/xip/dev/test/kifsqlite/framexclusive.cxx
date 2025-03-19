
//This frame exposes three methods
frame disp {

	//exclusive
	exclusive edisplay(string s) {
		println("Exclusive:",s);
	}

	//protected
	protected pdisplay(string s) {
		println("Protected:",s);
	}
}

//We also implement a task frame
frame task {
	string s;
	//with a specific "disp" instance
	disp cx;

	function _initial(string x) {
		s=x;
	}

	//Then we propose three methods
	//We call our local instance with protected
	function pdisplay() {
		cx.pdisplay(s);
	}

	//We call our local instance with exclusive
	function edisplay() {
		cx.edisplay(s);
	}

	//we call the global instance with exclusive
	function display(disp c) {
		c.edisplay(s);
	}
}

//the common instance
disp c;
vector v;
int i;
for (i=0;i<100;i++) {
	task t("T"+i);
	v.push(t);
}

//In this case, the display will be ordered as protected is not reentrant
//only one pdisplay can run at a time
for (i=0;i<100;i++)
	v[i].pdisplay();

//In this case, the display will be a mix of all edisplay working in parallel
//since, exclusive only protects methods within one instance... Strings will
//be completly mixed one into the others.
for (i=0;i<100;i++)
	v[i].edisplay();

//In this last case, we have one single common object "disp" shared by all "task"
//The display will be again ordered as with protected, since this time we run into the same
// "c disp" instance.
for (i=0;i<100;i++)
	v[i].display(c);

