//
// module for extended plain result
//
#include <vector>

#include "parsbin.h"


#include "ProcessorFactory.hxx"
#include "Processor.hxx"
#include "Analyzer.hxx"
#include "InputBuilder.hxx"
#include "OutputBuilder.hxx"
#include "OutputWriterFactory.hxx"
#include "OutputWriter.hxx"
#include "MemoryWriter.hxx"
#include "OutputIterator.hxx"

#include "xipjmax.h"
#define YES 1


/*
This file comprises the modification of the JMAX interface to analyse and store as 
XIP result the output of JMAX.
The different methods that are called are:

  registerEntries: this method splits a JMAX strings in token and stores them as a ListeVoc object
                  that is used by the parser
 
  ParseEntreeLexique: this is the method that is called to split the JMAX string in tokens. Each part
                      of the string is stored as a separate token in an internal vector of strings: 
                      enregListeTags. For example the string: dogs\tdog\t+Noun+Pl is split in 4 tokens:
                      enregListeTags[0]="dogs"  surface form
                      enregListeTags[1]="dog"   lemma form
                      enregListeTags[2]="Noun"  first tag
                      enregListeTags[3]="Pl"    last tag

  Parse: this method simply parse the input string transformed into a vector of ListeVoc objects
  AffichageResultat: this method creates the XipResult object that is the result of the parse
  Detruit (delete) : this method cleans the internal buffer to start a new parse
  excuteAnalyse : this variable toggles between pure JMAX display and parsing
*/



//This function counts the number of tabs in a line.
int TabCounter(char* line) {
    int v=0;
    char* pt=line;

    while ( (pt=strchr(pt+1,'\t'))!=NULL) v++;
    return v;
}

const string nul = "(null)";

void splitString(vector<string> &array, string str, string delim);	// add argument "delim" (delimiter)
bool matching(string buf, string str, vector<string>& output_array);
string removeNull(string symbol);	// remove string from buffered result
vector<string> getFormatArray(vector<string> output_array);	// format array for output



void GlobalParseurJMAX::epr() {
    //ofstream ofs = ofstream(filename, ios::out | ios::binary);
    
    OutputWriterFactory writerFactory;
    MemoryWriter* memory = writerFactory.createMemoryWriter();
    
    azr->getOutputBuilder()->setResultWriter(memory);
    azr->getOutputBuilder()->setExtensionFlag(true);
	azr->getOutputBuilder()->setReferFlag(true);	//set flag of lsc reference
    azr->run();
    static char buffer[10000];
    
    // start...
    string buf("");
    string str("");
    vector<string> output_array;
    bool matchFlag = false;
    const char* stringBuffer;

    //The class ListeVoc is a list of vocabulary entry readings
    //which is defined in XIP.
    ListeVoc* voc=NULL;    
    
    int startIndex = -1, endIndex = -1;
    OutputIterator* iter = memory->createOutputIterator();
    
    iter->first();
    while (!iter->isDone()) {
    
        str = iter->getString();
        
        //cout << str << endl;
        
        //バッファリングしていた文字列とオフセット値が同じ、文節の分割数が等しいもの
        //についてャbチング、表示調整を行う
        if ( startIndex ==  iter->getStartIndex() &&
            endIndex == iter->getEndIndex() &&
            matching(buf, str, output_array) ) {
            //cout << "matching" << endl;
            matchFlag = true;
            
        } else {
            //cout << "output start" << endl;
            //cout << iter->getStartIndex() << ", " << iter->getEndIndex() << "\n";
             voc=NULL;
            //オフセット値が同じでないならバッファリングしていた文字列を出力する
             
//--------------------------------------------------------------------------------------------             
             //XIP
             //This is certainly the most tricky part of the transformation
             //I guess that if there are some problems in the translation, they certainly
             //take their origin in my misunderstanding of the following lines.
			 output_array = getFormatArray(output_array);	// format output_array (including remove "(null)" string)
			 for (vector<string>::iterator i = output_array.begin(); i != output_array.end(); i++) {
                //if ((signed) ((*i).find("\t", 0)) < 0) 
                  //  ofs << "\t\t";
                //cout << "\t\t";
                
                //We use this string to feed XIP with a string that comes from JMAX
                stringBuffer= i->c_str();
                //We copy this entry in a buffer
                strcpy(buffer,(char*)stringBuffer);
                
                if (appliqueGrammaire==YES)
                    //registerEntries split an entry and translates it as XIP entry
                    //voc records the result of that operation
                    registerEntries(voc,buffer);                
                else
                    //If JMAXONLY is called, then the entry is simply recorded as it is
                    AppendLemmaXipResult(buffer,startIndex,endIndex);
            }
            //ofs.flush();
            //cout << "output end." << endl;
            output_array.clear();
            //cout << "output_array.clear()" << endl;
            matchFlag = false;
        }
        
        buf = str;
        //cout << "buf = str;" << endl;
        if (matchFlag == false) splitString(output_array, buf, "\n");
        //cout << "splitString" << endl;
        
        startIndex = iter->getStartIndex(); 
        endIndex = iter->getEndIndex();
        
        iter->next();        
    }
    
    //最後の出力
    voc=NULL;
	output_array = getFormatArray(output_array);
    for (vector<string>::iterator i = output_array.begin(); i != output_array.end(); i++) {
        //if ((signed) ((*i).find("\t", 0)) < 0) 
          //  ofs << "\t\t";
        //cout << "\t\t";

        //We treat the last entry the same way
        stringBuffer= i->c_str();
        strcpy(buffer,(char*)stringBuffer);
        if (appliqueGrammaire==YES)
            registerEntries(voc,buffer);                
        else
            AppendLemmaXipResult(buffer,startIndex,endIndex);
        //cout << (*i) << "\n\n";
    }

    //Last but not least, of there is still something to parse...
    Parse(YES);
    //AffichageResultat means: Display Result
    //It creates the XipResult object
    AffichageResultat();
    //Detruit means Delete current XIP entries
    Detruit();

    //後片付け
    buf = ""; 
    str = "";
    output_array.clear();
    
    
    delete iter;
    delete memory;
}


//指定した文字列をデリミタにして、指定した文字列を分割する
void
splitString(vector<string>& array, string str, string delim) {
	int num = 0;
	int i = 0;
	int pos = 0;
	string sub("");

	//cout << "string: " << str << endl;

	int len = str.length();
	//cout << "string length: " << len << endl;

	while (i < len && 0 <= pos ) {
		//cout << "looping" << endl;
	  pos = str.find(delim, i);
	  
	  //cout << "string pos: " << pos << endl;
	  
	  sub = str.substr(i, pos - i);
	  //cout << "sub string: " << sub << endl;
	  array.push_back(sub);
	  i = pos + 1;
	}
	//cout << "splitString end." << endl;
}


bool
matching(string buf, string str, vector<string>& output_array) {
    vector<string> buf_array;
    vector<string> array;
    
    //cout << "matching start..." << "\n";
    
    //文字列を改行で切り出す
    splitString(buf_array, buf, "\n");
    splitString(array, str, "\n");
    
    //	cout << "buf: " << buf << endl;
    //	cout << "str: " << str << endl;
    
    //サイズが異なっていたら偽を返す
    if (array.size() != buf_array.size()) return false;
    
    //バッファリングしていた文字列とマッチングをかける
    //	cout << "array.size(): " << array.size() << endl;
    
    for(int i = 0;  i < array.size(); i++) {
        //マッチしなければ空行なしでバッファに追加する
        //		cout << "buf_array[i]: " << buf_array[i] << endl;
        //		cout << "array[i]: " << array[i] << endl;
        
        if (buf_array[i] != array[i]) {
            //			cout << "output_array[i]: " << output_array[i] << endl;
            output_array[i] += ("\n" + array[i]);
        }
        //		cout << "test" << endl;
    }
    
    buf_array.clear();
    array.clear();
    //	cout << "matching end." << endl;
    return true;
}


//シンボル文字列から"(null)"を取り除いて返す
string removeNull(string symbol) {
	vector<string> split;
	int pos = 0;
	int len = 0;

	//cout << "symbol: "  << symbol << endl;
	len = nul.length();
	pos = symbol.find(nul,0);
	//cout << "len: " << len << endl;
	while(0 <= pos) {
		//symbol = symbol.substr(0, pos - 1) + symbol.substr(pos + len);
		symbol.replace(pos, len, "");
		//cout << "editing symbol: " << symbol << endl;
		pos = symbol.find(nul, 0);
		//cout << "pos: " << pos << endl;
	}
	return symbol;
}


// "(null)"シンボルを削除し、整形する処理を行う
vector<string> getFormatArray(vector<string> output_array) {
	string tmp_str;
	string lex, sur, sym, symbol;
	vector<string> tmp_array, split_array, ret_array;

	//cout << "size: " << output_array.size() << endl;

	for(int i = 0; i < output_array.size(); i++) {
		tmp_str = output_array[i];

        if (tmp_str.length()==0)
           break;

		//cout << "tmp_str:" << tmp_str << endl;

		//重複の場合、改行までの文字列を取得する
		if (0 <= tmp_str.find("\n")) {
			tmp_str = tmp_str.substr(0, tmp_str.find("\n"));
		}

		splitString(split_array, tmp_str, "\t");

		if (split_array.size() > 1) { // && !split_array[0].compare(nul)) {

			symbol = removeNull(split_array[2]);
			//cout << "removeNull return: " << symbol << endl;

			if (symbol.length() == 0) {
				//cout << "lex: " << split_array[0] << endl;
				//cout << "sur: " << split_array[1] << endl;
				lex += split_array[0];	
				sur += split_array[1];
				sym += symbol;
			
			} else {
				tmp_str = (lex + split_array[0] + "\t" + sur + split_array[1] + "\t" + sym + symbol);
				//cout << "push_back tmp_str: " << tmp_str << endl;
				ret_array.push_back(tmp_str);
				lex = "";
				sur = "";
				sym = "";
			}
		
		} else {
			symbol = removeNull(split_array[0]);
			if (symbol != "" && ret_array.size() != 0) {
				ret_array[ret_array.size() - 1] += symbol;
			}
		}
		split_array.clear();
	}
	return ret_array;
}



//This method simply calls the spliting of the string which is then recorded in 
//a enregListeTags (TagsRecordedList)
//We store the result in a XipResult.
void GlobalParseurJMAX::AppendLemmaXipResult(char* buffer,int start,int end) {
    char* stringBufferCopy;
    XipNode* node = NULL;
    
    if (xipresult->sentences.size()==0) {
        xipresult->sentences.push_back(new XipUnit);
        xipresult->sentences[0]->root=new XipNode((char*)"TOP",-1,-1);
    }
    if (TabCounter(buffer)<2)
        node = new XipNode(buffer,start,end);
    else {
        node = new XipNode(buffer,start,end);

        //This method splits a string in list of lemma, surface form and syntactic tags 
        ParseEntreeLexique(buffer,&stringBufferCopy);
        
        if (stringBufferCopy!=NULL)
            free(stringBufferCopy);

        //node->lemma.push_back(strdup(enregListeTags[0]));
        //node->lemma.push_back(strdup(enregListeTags[1]));
        for (int i=2;i<enregListeTags.size();i++) {
            XipFeature* feature=new XipFeature(enregListeTags[i],(char*)"+");
            node->features.push_back(feature);
        }
    }
    xipresult->sentences[0]->root->daughters.push_back(node);
}


