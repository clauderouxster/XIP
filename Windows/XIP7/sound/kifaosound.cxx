
/* --- CONTENTS ---
Project    : KIFF
Version    : 10.00
filename   : kifaosound.cxx
Date       : 09/09/2010
Purpose    : Sound library template for KiF on raspberry pi
Programmer : Claude ROUX
Reviewer   :
*/

#ifdef AOSOUND
#include "kif.h"
#include "kifversion.h"
#include "kifcontainer.h"
#include "kifmaps.h"

//YOU NEED TO DEFINE: SOUND on the compiling line

//We use libao, libmpg123 and libsndfile sur le raspberry
/*
sudo apt-get install libao-dev
sudo apt-get install libmpg123-dev
sudo apt-get install libsndfile1-dev
*/

//Documentations: 
/*
http://www.mega-nerd.com/libsndfile/
http://snippets.khromov.se/raspberry-pi-shairport-build-instructions/

For instance, to solve an error message that occurs on raspberry pi (but does not prevent the system from playing the music)

“Unknown PCM cards.pcm.front”

You can modify:

nano /usr/share/alsa/alsa.conf
Change the line “pcm.front cards.pcm.front” to “pcm.front cards.pcm.default”
*/

//We have used the following examples to implement this library
/*
http://hzqtc.github.io/2012/05/play-mp3-with-libmpg123-and-libao.html
http://www.music.columbia.edu/pipermail/music-dsp/2002-January/047060.html
https://gist.github.com/maxammann/52d6b65b42d8ce23512a
*/


#ifdef WIN32
#define Exporting __declspec(dllexport)
#define ssize_t size_t
#endif

//To play files
#include <ao/ao.h>

//Used to decode MP3 files...
#include <mpg123.h>

//To sample sound file before hand and get the right information to ao_play
#include <sndfile.h>

#define BITS 8

//Replace sound by your own definition
short KifNewId(string n);
string KifIdString(short n);

//Declarations
//Each kifobject has a specific type identifier, which in this case is dynamically generated
static KifType sound_type;

//We create a map between our methods, which have been declared in our class below. See MethodInitialization for an example
//of how to declare a new method.
class Kifsound;
//This typedef defines a type "soundMethod", which expose the typical parameters of a new KiF method implementation
typedef KifElement* (Kifsound::*soundMethod)(KifDomain*, int, KifCallFunction*);


static void intialization() {
	static bool init = false;
	if (!init) {
		ao_initialize();
		mpg123_init();
		init = true;
	}
}

const int BUFFER_SIZE = 8192;

class Kifsound : public KifBasic {
public:
	//We export the methods that will be exposed for our new object
	//this is a static object, which common to everyone
	static map<short, KifCallMethod> kifexportedmethods;
	//We associate the method pointers with their names in the linkedmethods map
	static map<short, soundMethod> linkedmethods;
	static hmap<short, bool> methodassigned;
	static map<string, string> infomethods;

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Your personal variables here...
	//ao variable
	mpg123_handle *mh;
	unsigned char *mp3_buffer;
	size_t mp3_buffer_size;

	ao_sample_format format;

	SNDFILE* soundfile;
	short *buffer;

	int driver;

	//KiF variable
	string path;
	bool loaded;
	bool stop;
	bool mp3;

	ao_device *device;

	//we could be playing our file from a thread
	KifThreadLock _locker;
	//---------------------------------------------------------------------------------------------------------------------
	Kifsound(KifCode* kifcode, KifElement* base) : _locker(NULL, false), KifBasic(kifcode, base, sound_type) {
		intialization();
		loaded = false;
		stop = false;
		mp3 = false;
		soundfile = NULL;
		mh = NULL;
		mp3_buffer = NULL;
		buffer = NULL;
		driver = ao_default_driver_id();
		device = NULL;
		mp3_buffer_size = 0;
	}

	~Kifsound() {
		if (device != NULL)
			ao_close(device);

		if (soundfile != NULL)
			sf_close(soundfile);

		if (mp3_buffer != NULL)
			free(mp3_buffer);

		if (buffer != NULL)
			free(buffer);

		if (mh != NULL) {
			mpg123_close(mh);
			mpg123_delete(mh);
		}
	}

	//How to set the inner value
	//This function is called whenever an initialisation occurs: x=y;
	//If an index has been specified (x[xx]) in the KiF code, then kindex will be different from NULL
	bool Setvalue(KifElement* kval, KifElement* kindex, KifElement* domain) {
		return true;
	}

	//Very Important, you should always implement this function in order to distinguish this object from other objects
	//It can return whatever value you want
	int Yourowntype() {
		return sound_type;
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//Duplication of an element, this method is called whenever a variable is used as a parameter in a function call
	KifElement* Copy(KifDomain* kp, KifElement* doma = kifNULL) {
		return this;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//If you need to read initial parameters attached to your objet such as: mynewtype toto(i,j);
	//Then initialconstructor should return true
	bool initialconstructor() {
		return true;
	}

	KifElement* loading() {

		SF_INFO sfinfo;
		//For sampling sound files other than MP3		

		if (path.find(".mp3") != string::npos || path.find(".MP3") != string::npos) {
			int channels, encoding, err;
			long rate;

			//we test if it is a MP3 file...
			mh = mpg123_new(NULL, &err);
			mp3_buffer_size = mpg123_outblock(mh);
			mp3_buffer = (unsigned char*)malloc(mp3_buffer_size * sizeof(unsigned char));

			if (mpg123_open(mh, STR(path)) != MPG123_OK)
				return kifcode->Returnerror("SND(003): Loading error");

			if (mpg123_getformat(mh, &rate, &channels, &encoding) != MPG123_OK)
				return kifcode->Returnerror("SND(013): Unknown format");

			/* set the output format and open the output device */
			format.bits = mpg123_encsize(encoding) * BITS;
			format.rate = rate;
			format.channels = channels;
			format.byte_format = AO_FMT_NATIVE;
			format.matrix = 0;
			mp3 = true;

			device = ao_open_live(driver, &format, NULL);

			//In this case, we need to do it live from the buffer initialized with the mp3 decoder
			if (device == NULL)
				return kifcode->Returnerror("SND(004): Cannot open mp3 audio");

			return kifTRUE;
		}

		//For non MP3 files (wav or others)
		//Sampling with libsndfile
		if (!(soundfile = sf_open(STR(path), SFM_READ, &sfinfo))) {
			sf_perror(NULL);
			return kifcode->Returnerror("SND(003): Loading error");
		}

		//Basically, we have now all the right information about this file
		switch (sfinfo.format & SF_FORMAT_SUBMASK) {
		case SF_FORMAT_PCM_16:
			format.bits = 16;
			break;
		case SF_FORMAT_PCM_24:
			format.bits = 24;
			break;
		case SF_FORMAT_PCM_32:
			format.bits = 32;
			break;
		case SF_FORMAT_PCM_S8:
			format.bits = 8;
			break;
		case SF_FORMAT_PCM_U8:
			format.bits = 8;
			break;
		default:
			format.bits = 16;
			break;
		}

		format.channels = sfinfo.channels;
		format.rate = sfinfo.samplerate;
		format.byte_format = AO_FMT_NATIVE;
		format.matrix = 0;

		buffer = (short*)calloc(BUFFER_SIZE, sizeof(short));
		device = ao_open_live(driver, &format, NULL);
		//In this case, we need to do it live from the buffer initialized with the mp3 decoder
		if (device == NULL)
			return kifcode->Returnerror("SND(004): Cannot open audio");

		return kifTRUE;
	}

	//If initialconstructor returns true, then you might want to benefit from these parameters
	KifElement* constructor(KifDomain* domloc, KifElement* params, int idthread, KifCallFunction* callfunc) {
		//If you do not have any parameters, then your function might return whatever you want... Here itself
		if (params->InstructionSize() == 0)
			return this;

		path = params->Instruction(0)->Exec(kifNULL, domloc, kifNULL, idthread, callfunc)->String();

		KifElement* res = loading();
		if (res != kifTRUE)
			return res;

		loaded = true;
		return this;
	}

	//-----------------------------------------------------------------------------------------------------------------------
	//Declaration
	//All our methods must have been declared in kifexportedmethods... See MethodInitialization below
	KifElement* Declaration(short n) {
		if (kifexportedmethods.find(n) != kifexportedmethods.end())
			return &kifexportedmethods[n];
		//It might be a predefined function: string, int, float, boolean, size, list, type or isa
		return Kifpredefined(n);
	}

	//List of accessible functions
	//we iterate among the functions we have declared
	void Methods(KifVectorString* fcts) {
		map<short, KifCallMethod>::iterator it;
		for (it = kifexportedmethods.begin(); it != kifexportedmethods.end(); it++)
			fcts->values.push_back(KifIdString(it->first));
	}

	KifElement* Information(KifElement* kstr) {
		string s = kstr->String();
		if (infomethods.find(s) == infomethods.end())
			return kifNULL;
		return kifcode->Providestringraw(infomethods[s]);
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	//This is an example of a function that could be implemented for your needs.
	KifElement* MethodLoad(KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		ThreadLockElement _lock(_locker);

		if (loaded)
			return kifcode->Returnerror("SND(001): Already loaded");

		//In our example, we have only two parameters
		//0 is the first parameter and so on...
		path = callfunc->Evaluatethread(0, domain, idthread)->String();

		KifElement* res = loading();
		if (res != kifTRUE)
			return res;

		loaded = true;
		return kifTRUE;
	}

	KifElement* MethodPlay(KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		ThreadLockElement _lock(_locker);

		bool rset = false;
		if (callfunc->Size() == 1) {
			KifElement* k = callfunc->Evaluatethread(0, domain, idthread);
			if (k->type == kifVectorInteger)
				return MethodEncode(domain, idthread, callfunc);
			rset = k->Boolean();
		}

		if (!loaded)
			return kifcode->Returnerror("SND(002): Load your file first");

		stop = false;
		if (mp3) {
			//In case, the user has requested the stream to be reset to 0
			if (rset)
				mpg123_seek(mh, 0, SEEK_SET);

			size_t done = 1;

			/* decode and play */
			while (!stop && mpg123_read(mh, mp3_buffer, mp3_buffer_size, &done) == MPG123_OK && !Errorid(idthread) && done != 0)
				ao_play(device, (char*)mp3_buffer, done);

			return kifTRUE;
		}


		bool error = false;
		int read;
		if (rset)
			sf_seek(soundfile, 0, SEEK_SET); //We reset the stream to the beginning

		read = sf_read_short(soundfile, buffer, BUFFER_SIZE);
		while (read != 0 && !stop && !Errorid(idthread)) {
			if (ao_play(device, (char *)buffer, (uint_32)(read * sizeof(short))) == 0) {
				error = true;
				break;
			}
			read = sf_read_short(soundfile, buffer, BUFFER_SIZE);
		}

		if (error) {
			//We reset the device
			ao_close(device);
			device = ao_open_live(driver, &format, NULL);
			return kifcode->Returnerror("SND(005): Erreur while playing sound");
		}

		return kifTRUE;
	}

	KifElement* MethodEncode(KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		ThreadLockElement _lock(_locker);

		if (device == NULL)
			return kifcode->Returnerror("SND(011): Please open the audio first");

		KifElement* k = callfunc->Evaluatethread(0, domain, idthread);

		if (k->type != kifVectorInteger)
			return kifcode->Returnerror("SND(010): The argument should be an ivector");


		KifVectorInteger* kvect = (KifVectorInteger*)k;
		stop = false;
		int read = kvect->Size();
		int i;
		if (mp3) {

			{
				KifContainerLock _lockvect(kvect);
				read = kvect->values.size();
				for (i = 0; i < read; i++)
					mp3_buffer[i] = (char)kvect->values[i];
			}

			if (!read)
				return kifFALSE;

			if (ao_play(device, (char*)mp3_buffer, read) == 0)
				return kifcode->Returnerror("SND(005): Erreur while playing sound");

			return kifTRUE;
		}

		{
			KifContainerLock _lockvect(kvect);
			read = kvect->values.size();
			for (i = 0; i < read; i++)
				buffer[i] = (short)kvect->values[i];
		}

		if (!read)
			return kifFALSE;

		if (ao_play(device, (char *)buffer, (uint_32)(read * sizeof(short))) == 0)
			return kifcode->Returnerror("SND(005): Erreur while playing sound");

		return kifTRUE;
	}

	KifElement* MethodDecode(KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		ThreadLockElement _lock(_locker);

		if (!loaded)
			return kifcode->Returnerror("SND(002): Load your file first");

		KifElement* k = callfunc->Evaluatethread(0, domain, idthread);

		if (k->type != kifVectorInteger)
			return kifcode->Returnerror("SND(010): The argument should be an ivector");

		KifVectorInteger* kvect = (KifVectorInteger*)k;

		size_t read, i;

		if (mp3) {
			/* decode only*/
			if (mpg123_read(mh, mp3_buffer, mp3_buffer_size, &read) == MPG123_OK) {

				KifContainerLock _lockvect(kvect);
				kvect->values.clear();
				for (i = 0; i < read; i++)
					kvect->values.push_back(mp3_buffer[i]);

				if (!read)
					return kifFALSE;

				return kifTRUE;
			}

			return kifFALSE;
		}

		read = sf_read_short(soundfile, buffer, BUFFER_SIZE);

		KifContainerLock _lockvect(kvect);
		kvect->values.clear();
		for (i = 0; i < read; i++)
			kvect->values.push_back(buffer[i]);

		if (!read)
			return kifFALSE;

		return kifTRUE;
	}

	KifElement* MethodStop(KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//Of course to be able to stop a sound, you need to play your SOUND file IN A THREAD...
		stop = true;
		return kifTRUE;
	}

	KifElement* MethodInfos(KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		ThreadLockElement _lock(_locker);
		if (!loaded)
			return kifcode->Returnerror("SND(002): Load your file first");

		string k;
		long v;

		if (callfunc->Size() == 0) {
			KifMapStringInteger* kimap = new KifMapStringInteger(kifcode, NULL);

			k = "channels"; v = format.channels;
			kimap->storekey(k, v);
			k = "rate"; v = format.rate;
			kimap->storekey(k, v);
			k = "byte_format"; v = format.byte_format;
			kimap->storekey(k, v);
			k = "mp3"; v = mp3;
			kimap->storekey(k, v);
			k = "bits"; v = format.bits;
			kimap->storekey(k, v);
			k = "size";
			if (mp3)
				v = mp3_buffer_size;
			else
				v = BUFFER_SIZE;
			kimap->storekey(k, v);

			return kimap;
		}

		KifElement* kmap = callfunc->Evaluatethread(0, domain, idthread);


		if (!kmap->aMapContainer())
			return kifcode->Returnerror("SND(008): The argument shoud be a map");

		KifIteration* itrloc = (KifIteration*)kmap->Newiterator(false);
		for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
			k = itrloc->IteratorKeyString();
			v = itrloc->IteratorValueInteger();
			if (k == "channels")
				format.channels = v;
			else
			if (k == "rate")
				format.rate = v;
		}
		itrloc->Release();

		//We reset the device
		ao_close(device);
		device = ao_open_live(driver, &format, NULL);

		return kifTRUE;
	}

	KifElement* MethodOpen(KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		ThreadLockElement _lock(_locker);

		string k;
		long v;

		KifElement* kmap = callfunc->Evaluatethread(0, domain, idthread);

		if (!kmap->aMapContainer())
			return kifcode->Returnerror("SND(008): The argument shoud be a map");

		int sz = BUFFER_SIZE;
		KifIteration* itrloc = (KifIteration*)kmap->Newiterator(false);
		for (itrloc->Begin(); itrloc->End() != kifTRUE; itrloc->Next()) {
			k = itrloc->IteratorKeyString();
			v = itrloc->IteratorValueInteger();
			if (k == "channels")
				format.channels = v;
			else if (k == "rate")
				format.rate = v;
			else if (k == "byte_format")
				format.byte_format = v;
			else if (k == "bits")
				format.bits = v;
			else if (k == "mp3")
				mp3 = v;
			else if (k == "size")
				sz = v;
		}
		itrloc->Release();

		if (mp3) {
			mp3_buffer_size = sz;
			if (mp3_buffer != NULL)
				free(mp3_buffer);
			mp3_buffer = (unsigned char*)malloc(mp3_buffer_size * sizeof(unsigned char));
		}
		else {
			if (buffer == NULL)
				buffer = (short*)calloc(BUFFER_SIZE, sizeof(short));
		}

		format.matrix = 0;

		//We reset the device
		if (device != NULL)
			ao_close(device);
		//We open it...
		device = ao_open_live(driver, &format, NULL);

		return kifTRUE;
	}

	KifElement* MethodClose(KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		ThreadLockElement _lock(_locker);

		if (device != NULL) {
			ao_close(device);
			device = NULL;
			return kifTRUE;
		}
		return kifFALSE;
	}

	KifElement* MethodReset(KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		ThreadLockElement _lock(_locker);

		if (mp3) {
			if (mh != NULL)
				mpg123_seek(mh, 0, SEEK_SET);
		}
		else {
			if (soundfile != NULL)
				sf_seek(soundfile, 0, SEEK_SET); //We reset the stream to the beginning
		}
		return kifTRUE;
	}

	//---------------------------------------------------------------------------------------------------------------------

	//ExecuteMethod must be implemented in order to execute our new KiF methods. This method is called when a KifCallMethod object
	//is returned by the Declaration method.
	KifElement* ExecMethod(short name, KifElement* contextualpattern, KifDomain* domain, int idthread, KifCallFunction* callfunc) {
		//This call is a bit cryptic. It takes the method (function) pointer that has been associated in our map with KIFCURLSTRINGS[2]
		//and run it with the proper parameters. This is the right call which should be invoked from within a class definition
		KiFArityError(name)
			return (this->*linkedmethods[name])(domain, idthread, callfunc);
	}

	//In this case, the current element will be destroyed
	void Clean() {
		reference--;
		//Your cleaning here
	}

	void Clear() {
		//Your reset code here
	}

	//---------------------------------------------------------------------------------------------------------------------
	//This SECTION is for your specific implementation...
	// In this section, you can implement your own String, Integer etc... interpretation of your object
	//You can also derived your own implementation of plus, multiply, minus etc... together with the comparison methods

	//If you need to return a specific size for your object (the KIFCURLSTRINGS[3] method is already available in Predefined)
	long Size() { return 0; }

	//How to interprete your class according to the basic following types:
	string String() { return path; }
	long Integer() { return 0; }
	double Float() { return 0; }
	bool Boolean() { return true; }

};
//We need to declare once again our local definitions.
map<short, KifCallMethod> Kifsound::kifexportedmethods;
map<short, soundMethod>  Kifsound::linkedmethods;
map<string, string> Kifsound::infomethods;
hmap<short, bool> Kifsound::methodassigned;

//-----------------------------------------------------------------------------------------------------------------------

//Browsing function to associate a variable declaration with an object of our new type
//Thanks to this method, this new type is now known to the KiF compiler.
static KifElement* CreatekifsoundElement(KifCode* kifcode, x_node* xn, KifElement* base) {
	//Mandatory call to initialise our variable
	KifElement* variable = kifcode->KifInitVariable(xn, base);
	//which we associate with our object
	Kifsound* local = new Kifsound(kifcode, variable);
	//We then set the whole stuff right
	return kifcode->KifTerminateVariable(xn, local, variable, base);
}

//MethodInitialization will add the right references to "name", which is always a new method associated to the object we are creating
static void MethodInitialization(KifCode* kifcode, string name, soundMethod func, int arity, string infos) {
	//A KifCallMethod object is used by KiF to call the ExecuteMethod on our object
	short idname = KifNewId(name);
	KifCallMethod kcm(idname, arity);
	Kifsound::kifexportedmethods[idname] = kcm;
	kifGlobals->RecordExternalFunction(idname, arity);
	Kifsound::linkedmethods[idname] = func;
	Kifsound::methodassigned[idname] = true;
	Kifsound::infomethods[name] = infos;
}

//we export this function as a C one to avoid fumbling with C++ mangling
extern "C" {
	Exporting bool Init_kifsound(KifCode* kifcode, string version) {
		if (version != KIFVERSION) {
			cerr << "Incompatible version of KiF" << endl;
			return false;
		}

		kifcode->KifInitLib();

		if (kifGlobals->TestExternalType("sound") == true)
			return false;

		///The new type will be: sound. You will be able to declare any variable with that type, once this library has been loaded...
		//CreatekifsoundElement will be invoked by the KiF compiler to create the right sort of object for this type (in our example: Kifsound)
		//Whenever the keyword testage will be found by KiF it will then call CreatekifsoundElement, thanks to this association.
		//If the function returns kifVOID, it is an error. This type has already been declared.
		KifType res = KifAddNewType("sound", CreatekifsoundElement);
		if (res == kifVOID)
			return false;
		sound_type = res;

		return true;
	}

	Exporting bool kifsound_KifInitialisationModule(KifCode* kifcode, string version) {
		if (Init_kifsound(kifcode, version) == false)
			return false;

		//We need to create specific objects in order to have our right class method called
		//MethodInitialization, which is declared in this file (see above), implements a specific object that will be returned
		//by the Declaration method above. When a KifCallMethod object is detected during execution, then KiF calls
		//ExecuteMethod, which must have been implemented for our new object (see above again). We also link this new name
		//with a specific method from our new class.
		MethodInitialization(kifcode, "load", &Kifsound::MethodLoad, P_ONE, "load(string pathname): Load a sound file");
		MethodInitialization(kifcode, "play", &Kifsound::MethodPlay, P_NONE | P_ONE, "play(bool rst): play the sound. If rst is true, start back to beginning");
		MethodInitialization(kifcode, "stop", &Kifsound::MethodStop, P_NONE, "stop(): stop the sound");
		MethodInitialization(kifcode, "parameters", &Kifsound::MethodInfos, P_NONE | P_ONE, "parameters(): Return the sound configuration values as a simap. Parameters rate and channels can be modified");
		MethodInitialization(kifcode, "clear", &Kifsound::MethodReset, P_NONE, "clear(): Reset the audio channel");
		MethodInitialization(kifcode, "open", &Kifsound::MethodOpen, P_ONE, "open(simap p): Open a sound channel with parameters p (server side)");
		MethodInitialization(kifcode, "close", &Kifsound::MethodClose, P_NONE, "close(): close the sound channel (server side)");
		MethodInitialization(kifcode, "decode", &Kifsound::MethodDecode, P_ONE, "decode(ivector): decode a sound channel, fill in an ivector (client side)");
		MethodInitialization(kifcode, "encode", &Kifsound::MethodEncode, P_ONE, "encode(ivector): play a sound channel from a decoding sessions (server side)");

		kifGlobals->kifAllFunctions[sound_type] = Kifsound::methodassigned;
		kifGlobals->kifAllInformation[sound_type] = Kifsound::infomethods;

		return true;
	}

	Exporting bool kifsound_Terminate(KifCode* kifcode) {
		Kifsound::kifexportedmethods.clear();
		Kifsound::linkedmethods.clear();
		Kifsound::methodassigned.clear();
		Kifsound::infomethods.clear();

		mpg123_exit();
		ao_shutdown();

		return true;
	}
}
#endif









