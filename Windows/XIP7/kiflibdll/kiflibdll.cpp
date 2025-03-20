// kiflibdll.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "kiflibdll.h"


// This is an example of an exported variable
KIFLIBDLL_API int nkiflibdll=0;

// This is an example of an exported function.
KIFLIBDLL_API int fnkiflibdll(void)
{
	return 42;
}

// This is the constructor of a class that has been exported.
// see kiflibdll.h for the class definition
Ckiflibdll::Ckiflibdll()
{
	return;
}
