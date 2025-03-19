
//A small frame to record our data
frame mycoord {
	int x,y;

	function _initial() {
		x=0;
		y=0;
	}
}

//we declare our object, which will record our data
mycoord coords;

bool first=true;
//our new display...
//Every time the window will be modified, this function will be called with a mycood object
function display(window w,mycoord o) {	
	if (first) {
		w.drawcolor(FL_RED);
		w.drawtext("Press T",20,20);
	}
	else {
		w.pushclip(o.x-1,o.y-1,61,61);
		//we select our color, which will be apply to all objects that follow
		w.cursorstyle(FL_CURSOR_CROSS,FL_BLACK,FL_WHITE);
		w.drawcolor(FL_RED);
		w.rectangle(o.x,o.y,60,60);
		//with some text...
		w.drawtext("TEST",o.x+20,o.y+20);
		w.popclip();
	}
}

function presse(button b,window w) {
	printlnerr("Fermeture");
	w.close();
}


//Once triggered, this thread will increment the coordinates et forces a redraw of the window for each new value.
thread bouge(window wnd) {
	int r;
	println("Bouger");
	while (true) {
		if (coords.x>800) {
			window w;
			lock("creation");
			button b(w) with presse;
			w.begin(100,100,200,200,"Thread");
			b.create(10,10,30,30,FL_Regular,FL_NORMAL_BUTTON,"Fin");
			w.end();
			unlock("creation");
			w.alert("OK");
			return;
		}				
		pause(0.001);
		coords.x++;
		coords.y++;
		wnd.redraw();		
	}
}


function pressed(button b,window w) {
	coords.x=0;
	coords.y=0;
	first=false;
	printlnerr("RAZ");
	bouge(w);
}



//we declare our window together with its associated drawing function and the object coord
window wnd(coords) with display;
//We need to instanciate the mouse call back
wnd.timeout(1);
wnd.begin(100,100,1300,900,"Drawing");
button b(wnd) with pressed;
b.create(1230,20,60,60,FL_Regular,FL_NORMAL_BUTTON,"Ok");
wnd.sizerange(10,10,0,0);
//we add a mouse call back
wnd.end();

wnd.run();
