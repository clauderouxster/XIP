#include<locale.h>
#include<ctype.h>
#include<stdio.h>


 
 
int main() {

    int c;

  if (!setlocale(LC_CTYPE, "iso_8859_1")) {
        fprintf(stderr, "guesser_lib::init_guesser(): Can't set locale to ISO Latin I");
        exit(1);
    }
  

while ((c = getchar()) != EOF)
	putchar(tolower(c));
	
   return 0;
}
