
window w;

function callback_table(table x,map V,window w) {
	println(V);
}

function callback_menu(wchoice c, string s) {
	println(s);
}

vector vmenu;

vmenu=[["Premier",callback_menu,"RRRR"],["second",callback_menu,"OOOOOO"],["troisieme",callback_menu,"BBBBBBB"]];
wchoice wch;


table t(w) with callback_table;
int i,j;

//we create our window
w.begin(300,200,1300,500,"Fenetre");
w.sizerange(10,10,0,0);
t.create(20,20,500,400,"table");
wch.create(20,420,100,50,"Choix");
wch.fontsize(20);
//wch.fontcolor(FL_RED);
t.fontsize(12);
t.colorfg(FL_RED);
t.colorbg(FL_YELLOW);
wch.menu(vmenu);
t.selectioncolor(FL_BLUE);
t.rowheight(20);
t.columnwidth(60);
for (i=0;i<10;i++) {
	t.rowheader(i,"R"+i);
	t.columnheader(i,"C"+i);
	for (j=0;j<10;j++)
		t.add(i,j,"R"+i+"C"+j);
}
w.end();
t.add(12,12,"TOTO");
w.run();


frame block {
	//We first declare our window object
	window w;
	string final;

	function display(winput txt,block bb) {
		println(txt.value());
	}

	winput txt(this) with display;

	function gettext(button b,block bb) {
		final=txt.value();
		w.close();
	}

	function lance(string ph) {
		final=ph;
		//We then begin our window instanciation
		w.begin(300,200,1300,150,"Modification");
		//We want our window to be resizable
		w.sizerange(10,20,0,0);		
		//we create our winput, which is placed within the current window
		txt.create(200,20,1000,50,true,"Selection");
		txt.value(ph);
		button b(this) with gettext;
		b.create(1230,20,30,30,FL_Regular,FL_NORMAL_BUTTON ,"Ok");
		//no more object, we end the session
		w.end();
		w.resizable(txt);
		//we then launch our window
		w.run();
	}
}

function reprise(string ph) {
	block b;
	b.lance(ph);
	return(b.final);
}

println(reprise("Ceci est une tentative"));

//A small frame to record our data
frame mycoord {

	int color;
	int x,y;

	function _initial() {
		color=FL_RED;
		x=0;
		y=0;
	}
}

//we declare our object, which will record our data
mycoord coords;

//our new display...
//Every time the window will be modified, this function will be called with a mycood object
function display(window w,mycoord o) {	
	//we select our color, which will be apply to all objects that follow
	w.cursorstyle(FL_CURSOR_CROSS,FL_BLACK,FL_WHITE);
	w.drawcolor(o.color);
	//a different line shape
	w.lineshape(FL_DASH,10);
	//we draw a rectangle
	w.rectangle(o.x,o.y,250,250);
	//with some text...
	w.drawtext("TEST",o.x+100,o.y+100);
}

//This function will be called for every single move the mouse
function move(window w,map mousecoord,mycoord o) {
	//we then use the mouse coordinates to position our rectangle
	o.x=mousecoord["x"];
	o.y=mousecoord["y"];
	//we then redraw our window...
	w.redraw();
}

function pressed(window w,string skey,int ikey,mycoord o) {
	if (skey=="J") {
		o.x+=10;
		o.y+=10;
		w.redraw();
	}
}



//we declare our window together with its associated drawing function and the object coord
window wnd(coords) with display;

function slidercall(slider s,mycoord o) {
	o.x=s.value();
	wnd.redraw();
}

slider vs(coords) with slidercall;

//We need to instanciate the mouse call back
wnd.begin(100,100,300,300,"Drawing");
wnd.sizerange(10,10,0,0);
//we add a mouse call back
wnd.onmouse(FL_MOVE,move,coords);
wnd.onkey(FL_KEYUP,pressed,coords);

//we create our value slider
vs.create(10,10,180,20,FL_ALIGN_LEFT,true,"Position");
//the position will be between 0 and 300
vs.bounds(0,300);
//with the initial value 100
vs.value(100);

wnd.end();

wnd.run();
