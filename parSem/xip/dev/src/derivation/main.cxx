#include "parsbin.h"
#include "derivation.h"

int main(int argc, char* argv[]) {
    VECTA<char*> parameters;
    GlobalParseurTry* parser = new GlobalParseurTry("english.xip", "toyGram");
    parser->loading("toyGram", parameters);
    FILE* text = fopen("input.txt", "r");
    //This instruction set the value of fichierDonnees with text
    parser->FileAssociation(text);
    parser->Analyse(0, 1);
    return 0;
}

