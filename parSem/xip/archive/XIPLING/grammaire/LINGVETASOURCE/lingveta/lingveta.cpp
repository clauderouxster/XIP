// lingveta.cpp : main project file.

#include "stdafx.h"
#include "lingvetan.h"

using namespace System;

int main(array<System::String ^> ^args)
{
	bnf_lingveta bnf;
	for (int i=0;i<1000;i++) {
		x_readstring xf("yuneca filinefa tota titaque nacan vetureletan apudefas kaser totan vendeieg uno apefiagas vendeiag");
		xf.loadtoken(true);
		x_node* xn=bnf.x_parsing(&xf,FULL);
	}
    return 0;
}
