#include "x_node.hn.h"


static const char* errorlabels[]={""};





string bnf_x_node::x_errormsg(int i) {
	return errorlabels[i];
}


x_node* bnf_x_node::x_parsing(x_reading* xr,x_parsing_mode mode,bool display) {
        fx=xr;
	string lret;
	x_node* tree=new x_node;
	gFail=0;
	lineerror=-1;
	labelerror="";
	errornumber=-1;
	currentpos=0;
	intoken=0;
	char res=%%%(lret,&tree);

	if (currentpos!=fx->stack.size() && mode==FULL) {
            if (display) {
		if (errornumber!=-1) 
			cerr<<endl<<"Error line:"<<lineerror<<"="<<errorlabels[errornumber]<<endl;
		else
			cerr<<endl<<"Error line:"<<lineerror<<endl;
	    }
	    if (tree!=NULL)
		delete tree;
	    return NULL;
	}

	if (res==1)
		return tree;

	delete tree;
	return NULL;
}


char bnf_x_node::x_test_string(string& lret,int sz,string sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}

char bnf_x_node::x_test_pstring(string& lret,int sz,string& sub) {
	if (currentpos>=fx->stack.size())
		return(0);
	if (fx->stack[currentpos]==sub) {
		incrementpos();
		lret+=sub;
		return 1;
	}
	return 0;
}

char bnf_x_node::m_x_node()(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="x_node()";
	if (fx->stacktype[currentpos]==5) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}

char bnf_x_node::m_x_readfile()(string& lreturn,x_node** tree) {
	if (gFail==1) return(0);
	if (currentpos>=fx->stack.size())
		return(0);
	static const char* label="x_readfile()";
	if (fx->stacktype[currentpos]==5) {
		x_init_node(lreturn,fx->stack[currentpos],tree,label,currentpos,true);
		incrementpos();
		return(1);
	}
	return(0);
}
