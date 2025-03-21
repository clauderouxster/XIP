
use('UNIX','/opt/ParSem/users/Roux/xip/parSem/xip/dev/xipFst/lib/linux/sharedLib/kifltk');

int couleur=FL_BLACK;
//A small frame to record our data
frame mycoord {

	int color;
	int x,y,l,h;
	string label;

	function _initial(string lab) {
		color=couleur;
		x=0;
		y=0;
		l=10;
		h=10;
		label=lab;
	}

	function Color() {
		return(color);
	}

	function X() {
		return(x);
	}

	function Y() {
		return(y);
	}

	function L() {
		return(l);
	}

	function H() {
		return(h);
	}
	function string() {
		return(label);
	}
}

//we declare our object, which will record our data
vector objects;
mycoord coords("initial");

//our new display...
//Every time the window will be modified, this function will be called with a mycood object
function display(window w,vector o) {	
	int i;
	//we select our color, which will be apply to all objects that follow
	w.cursor(FL_CURSOR_CROSS,FL_BLACK,FL_WHITE);	
	for (i=0;i<o;i++) {
		w.drawcolor(o[i].Color());
		w.lineshape(FL_DASH,4);
		w.rectangle(o[i].X(),o[i].Y(),o[i].L(),o[i].H());
	}	
	if (o.size()!=0) {
		string s="Coord:"+o[-1].X()+":"+o[-1].Y()+":"+o[-1].L()+":"+o[-1].H();
		w.drawcolor(FL_RED);
		w.lineshape(FL_SOLID,1);
		w.drawtext(s,10,20);
	}
}

bool moving=false;
bool first=false;
//This function will be called for every single move the mouse
function move(window w,map mousecoord,mycoord o) {
	if (moving==true) {
		//we then use the mouse coordinates to position our rectangle
		if (first) {
			o.l=10;
			o.h=10;
			first=false;
		}
		else {
			o.l=mousecoord["x"]-o.x;
			o.h=mousecoord["y"]-o.y;
		}
		//we then redraw our window...
		w.redraw();
	}
}
int nb=0;
function click(window w,map mousecoord,mycoord o) {	
	string s;
	if (moving==true) {
		moving=false;
		nb+=1;
		coords=mycoord("I"+nb);		
	}
	else {		
		o.x=mousecoord["x"];
		o.y=mousecoord["y"];
		first=true;
		objects.push(o);
		moving=true;
		w.redraw();
	}
	//we then redraw our window...
	
}



window wnd(objects) with display;
function Red(slider s,mycoord o) {
	vector v=wnd.rgbcolor(couleur);
	couleur=wnd.rgbcolor(s.value(),v[1],v[2]);
	o.color=couleur;
	wnd.redraw();
}

function Green(slider s,mycoord o) {
	vector v=wnd.rgbcolor(couleur);
	couleur=wnd.rgbcolor(v[0],s.value(),v[2]);
	o.color=couleur;
	wnd.redraw();
}

function Blue(slider s,mycoord o) {
	vector v=wnd.rgbcolor(couleur);
	couleur=wnd.rgbcolor(v[0],v[1],s.value());
	o.color=couleur;
	wnd.redraw();
}


//we declare our window together with its associated drawing function and the object coord

slider red(coords) with Red;
slider green(coords) with Green;
slider blue(coords) with Blue;

//We need to instanciate the mouse call back
wnd.begin(100,100,1000,800,"Drawing");
wnd.sizerange(10,10,0,0);
//we add a mouse call back
wnd.mouse(FL_MOVE,move,coords);
wnd.mouse(FL_RELEASE,click,coords);

red.create(50,30,100,20,FL_ALIGN_LEFT,true,"Red");
green.create(50,60,100,20,FL_ALIGN_LEFT,true,"Green");
blue.create(50,90,100,20,FL_ALIGN_LEFT,true,"Blue");

red.bounds(0,255);
green.bounds(0,255);
blue.bounds(0,255);

wnd.end();

wnd.run();
