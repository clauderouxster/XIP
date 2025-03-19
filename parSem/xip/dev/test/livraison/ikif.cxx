map styles;

styles["#"]=[FL_BLACK,FL_HELVETICA,16];
styles["execution"]=[FL_BLUE,FL_HELVETICA_ITALIC,16];
styles["erreur"]=[FL_RED,FL_HELVETICA_ITALIC,16];

kif session;
session.open();

window w;
editor e;

string std;
std.getstd(true);
vector instructionlist;

function affiche(editor e,string s,string type) {
	int pos=e.cursor();
	e.append(s+"\r");
	e.setstyle(pos,pos+s.size()+1,type);        
	pos=e.size();
	e.cursor(e.size());
	e.setstyle(pos-1,pos,"#");
}

int compte=0;
int lastpos=-1;
int precedent=-1;
bool fonction=false;

function clavier(window w,string skey,int ikey,editor e) {
	int iskey=skey.ord();	
	string code;
	//ctrl-e, evaluation
	int pos;
	boolean add=true;
	string error;
	int i;

	if (iskey==5 && ikey==101) {
		std="";
		code=e.selection();
	}
	else
		if (ikey==65293) {		
			std="";
			pos=e.cursor();
			if (lastpos!=-1) {
				if (pos>lastpos)
					pos=lastpos;				
			}
			vector v=e.linecharbounds(pos-1);
			code=e[v[0]:];
		}
		else			
			return;
		
	code=code.trim();
	if (code=="") {
		if (pos>1 && pos<e.size())
			e.setstyle(pos-1,pos,"#");
		return;
	}

	switch(code) {
		"exit":	exit(-1);
		"clear": {
			e.value("");
			return;
		}
		"help": {
			affiche(e,"\r\r- The absence of a semi-colon at the end of a line displays its content","erreur");
			affiche(e,"- help: display a help message","erreur");
			affiche(e,"- list: list the lines already compiled","erreur");
			affiche(e,"- clean: clean the memory from all codes","erreur");
			affiche(e,"- clear: clear the screen","erreur");
			affiche(e,"- save file: save compiled lines into file","erreur");
			affiche(e,"- Ctrl-e: evaluate selected lines","erreur");
			affiche(e,"- !nb: execute line nb (according to 'list')","erreur");
			affiche(e,"- exit: quit the session\r\r","erreur");	
			return;
		}
		"clean":  {
			affiche(e,"Cleaning","execution");
			//we close our current session
			session.clean();
			//then we reopen it
			session.open();
			//we clean our list of instructions
			instructionlist.clear();
			return;
		}
		"list": {
			int cpt=1;
			for (code in instructionlist) {
				affiche(e,""+cpt+"> "+code,"execution");
				cpt++;
			}
			return;
		}
	}

	if (code[:4]=="save") {
		printlnerr(code);
		error=code[5:].trim();		 
		file s(error,"w");
		for (code in instructionlist)
			s.write(code+"\r");
		affiche(e,"Saving "+error,"execution");
		return;
	}

	if (code[0]=='!') {
		code=instructionlist[code[1:].int()-1];
		if (code=="")
			return;
	}
	else {
		////If we add some code between '{...}'
		if ('while' in code || 'for' in code) {			
			lastpos=pos;
			if (code[-1]==')')
				return;
		}
		else {
			if ("function" in code || "thread" in code) {
				lastpos=pos;
				fonction=true;
			}
		}
		if (code[-1]=='{') {
			compte++;
			return;
		}

		if (code[-1]=='}')
			compte--;

		if (compte)
			return;

		lastpos=-1;
		if (code[-1]!=';' && code[-1]!='}') {
			code="println("+code+");";
			add=false;
		}
	}
	pos=e.cursor();

	try {
		i=session.compile(code);
		session.run(i);
		if (std!="") {
			std=std.trim();
			std+="\r";
			e.append(std);			
			e.setstyle(pos,e.size(),"execution");
		}
		if (add && code not in instructionlist)
			instructionlist.push(code);
		if (fonction)
			affiche(e,"Compiled function","execution");
		fonction=false;
	}
	catch(error) {
		e.append("> "+error+"\r");
		e.setstyle(pos,e.size(),"erreur");        
	}
	pos=e.size();
	e.cursor(pos);
	e.setstyle(pos-1,pos,"#");
}


w.begin(50,50,1200,800,"KiF interpreter");
w.sizerange(10,10,0,0);
e.create(1,1,1199,799,"");
e.addstyle(styles);
e.cursorstyle(FL_LINE_CURSOR);
w.onkey(FL_KEYUP,clavier,e);
w.end();
w.resizable(e);
affiche(e,"Type 'help' for help\r","erreur");
w.run();
