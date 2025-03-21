println(_args);
//A map describing the styles available within the editor
map m={'#':[FL_BLACK,FL_COURIER,FL_NORMAL_SIZE ], 
'movement':[ FL_RED,FL_COURIER,FL_NORMAL_SIZE],
"determinant": [ FL_BLUE,FL_COURIER,FL_NORMAL_SIZE]};

//We define the words that we want to recognize in the text
vector mvt=["move","run","stride","walk","drive"];
vector det=["le","la","un","une"];

//whenever the text is modified, we check for our above words
function modified(editor e,int pos, int ninserted,int ndeleted,int restyled,string del,self obj) {
	//we unmark everything first	
	e.setstyle(0,e.size(),"#");
	//then, we mark all our movement words
	e.annotate(mvt,"movement");
	e.annotate(det,"determinant");
}

//we need our message window to be displayed at the precise location of our mouse
window wmessage;
winput wi;
//This method is called whenever the mouse cursor is on a non default style
function infostyle(int x,int y,map sz,string style) {
	if (wmessage!=null)
		return;

	//we create a borderless window, with a yellow background
	wmessage.begin(x,y,sz["w"]+20,sz["h"]+20,style);	
	wmessage.backgroundcolor(FL_YELLOW);
	wmessage.border(false);
	box b;
	//we display our style, which is a string
	b.create(5,5,sz["w"]+5,sz["h"]+5,style);	
	wmessage.end();

}


//This function is called when the mouse is moved in the editor
function vmouse(editor e,map infos,self n) {	
	//we get the style at the cursor position which matches a position in the text
	string style=e.getstyle(infos["cursor"],infos["cursor"]+1);		
	
	//If it is not a standard style
	if (style!='#' && style!="") {
		println(wi.word(infos["cursor"]),wi[5:10]);
		println(e[infos["cursor"]:" "],":",e.word(infos["cursor"]));
		//we create our sticky note
		infostyle(infos["xroot"],infos["yroot"],e.textsize(style),style);					
	}
	else
		//if it is the standard style or no style at all, we close our window...
		if (wmessage!=null)
			wmessage.close();
}

window w;
editor e with modified;

//we create our window
w.begin(300,200,1300,700,"Marking run");
w.sizerange(10,20,0,0);		
//then our editor
e.create(200,220,1000,200,"Editor");
//we add the style
e.addstyle(m);
e.value("La dame run stride et walk dans la rue");
e.annotate(mvt,"movement");
e.cursorstyle(FL_LINE_CURSOR);
//and we also need a mouse callback
e.onmouse(FL_MOVE,vmouse,null);

wi.create(200,500,1000,100,true,"Input");
wi.value("La dame mange de la glace avec de la vanille");
w.end();
w.run();
