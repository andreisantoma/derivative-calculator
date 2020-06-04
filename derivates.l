%{
    #include <string.h>
    #include "derivates.tab.h"
%}

%%

[0-9]+  {yylval.ival = strtol(yytext, NULL, 10); return num;}
sin     {return sin;}
cos     {return cos;}
tan     {return tan;}
cotan   {return cotan;}
log     {return log;}
sqrt    {return sqrt;}
sinh    {return sinh;}
cosh    {return cosh;}
tanh    {return tanh;}
cotanh  {return cotanh;}
verbose {return verbose;}
[\n\r]  {return nl;}
.       {return yytext[0];}

%%