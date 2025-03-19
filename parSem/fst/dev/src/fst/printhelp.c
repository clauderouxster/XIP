#include <string.h>

#include <stdlib.h>
#include <stdio.h>

#include "fst/comm-def.c"

#ifdef Darwin
#include <unistd.h>      /* to get the header for getopt */
#endif

static void iy_print_usage(struct command *comm)
{
  char *p, buf[1000];
  if (comm)
    {
      if (!strncmp(comm->command,"variable",8))
	{
	  printf("\\item {\\em %s} == %s \\\\\n",comm->command,comm->usage);
	}
      else if (!strncmp(comm->command,"regex:",6))
	{
	  printf("\\item {\\em %s} (%s) \\\\\n",comm->command,comm->usage);
	}
      else
	{
	  printf("\\item \\begin{tabbing}\n");

	  printf("usage: \\= {\\em DUMMY} \\kill\n");

	  printf("usage: \\> ");
	  strcpy(buf,comm->usage);
	  p=strtok(buf,"|");
	  do {
	    printf("{\\em %s} %s",comm->command,(p ? p : ""));
	    p=strtok(NULL,"|");
	    if (p) printf(" \\\\\n\\> ");
	    else printf("\n");
	  } while (p);
	  printf("\\end{tabbing}\n");
	}
    }
}


static void iy_print_usage_html(struct command *comm)
{
  char *p, buf[1000];
  if (comm)
    {
      if (!strncmp(comm->command,"variable",8))
	{
	  printf("<DT><TT>%s</TT> (<EM>%s</EM>) <DD>\n",comm->command,comm->usage);
	}
      else if (!strncmp(comm->command,"regex:",6))
	{
	  printf("<DT>%s</TT> (%s) \n<DD>",comm->command,comm->usage);
	}
      else
	{
	  printf("<DT>\n");
	  strcpy(buf,comm->usage);
	  p=strtok(buf,"|");
	  do {
	    printf("\t<TT>%s</TT> &lt;%s&gt;",comm->command,(p ? p : ""));
	    p=strtok(NULL,"|");
	    if (p) printf(" <BR>\n");
	    else printf("<P>\n\n<DD>");
	  } while (p);
	}
    }
}

#if (defined (MAC_OS) || defined (_WIN32))
static int getopt(int argc, char**argv, char *opt)
{
	/* Must write this because it is not in the Metrowerks library.
		98-09-25 */
  int i;
  char c;
  
  for (i = 0; i < argc; i++)
  	while (*opt) {
  	  if (*opt == *(argv[i]) && *(argv[i]+1) == '\0')
  	  {
  	     /* Save the option and eliminate it from the string
  	     	before returning it as a value so that we will
  	     	not find it anymore next time around. */
  	     c = *opt;
  	     
  	     do {
  	     *opt = *(opt+1);
  	     } while (*opt);
  	       
  	     return((int) c);
  	   }
  	   *opt++;
  	}

	return EOF;
}
#endif  /* MAC_OS */

int main (int argc, char **argv)
{
  struct command *c;
  int a;
   void (* func) (struct command *comm);
 
   if (argc && argc <= 1)
     func = iy_print_usage;
   else
     while ((a = getopt(argc, argv, "twh")) != EOF)
       switch (a)
	 {
	 case 't':
	   func = iy_print_usage;
	   break;
	 case 'w':
	   func = iy_print_usage_html;
	   break;
	 case 'h':
	 default:
	   {
	     printf("usage: -t LaTeX style output\n\
  -w HTML style output\n");
	     exit(2);
	   }
	 }

 c=commands;
  while (c->command)
    {
      func(c);
      if (c->help)
	printf("%s\n\n",c->help);
      c++;
    }

  return 0;
}
