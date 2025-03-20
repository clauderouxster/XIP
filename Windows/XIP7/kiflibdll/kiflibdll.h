// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the KIFLIBDLL_EXPORTS
// symbol defined on the command line. This symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// KIFLIBDLL_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifdef KIFLIBDLL_EXPORTS
#define KIFLIBDLL_API __declspec(dllexport)
#else
#define KIFLIBDLL_API __declspec(dllimport)
#endif

// This class is exported from the kiflibdll.dll
class KIFLIBDLL_API Ckiflibdll {
public:
	Ckiflibdll(void);
	// TODO: add your methods here.
};

extern KIFLIBDLL_API int nkiflibdll;

KIFLIBDLL_API int fnkiflibdll(void);
