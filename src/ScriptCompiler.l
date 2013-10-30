/* Code section */
%{

int lastleng=0;
#include <stdlib.h>
#include <string.h>

#include "src/common.h" //Since flex always outputs at cwd, the final file will be located at root, so this is the route to common.h relative to there

#ifdef SYNTACTIC

#include "ScriptCompiler.tab.c"

extern int line, column;
extern void push_context();

#define outputid(x)     return (x)				//Lexical value (identifiers)
#define outputc(x)      return (x)				//Lexical value (characters)
#define value(type, x)  ({yylval.type = (x);})  //Semantic value

//Semantic value (char[])						
#define valuechrs(type, x) ({strcpy(yylval.type, (x));})

//Semantic value (char*)
#define valuestr(type, x)  ({\
    char* __x = x; \
	char* __str;\
	__str = malloc2(strlen(__x)+1);\
	strcpy(__str, (__x));\
	yylval.type = __str;\
	})
    
#define if_lex_free(x) //Do nothing

#else
int line = 1, column = 1;
void push_context() {}
bool verbose_mode = false;

#  ifndef NOTSYNTACTIC
#  warning "SYNTACTIC undefined - lexical analysis will be compiled separatedly"
#  endif

language lang;

#define outputid(x) printf("%s", #x)            //Lexical value (identifier)
#define outputc(x)  printf("%c", x)             //Lexical value (characers)
#define value(type, x)                          //Semantic value
#define valuechrs(type, x)                      //Semantic value (char[])
#define valuestr(type, x)                       //Semantic value (char*)
#define if_lex_free(x) free(x)
#endif

#define output(x) outputid(x)

%}

/* Lexical rules */
id              [a-zA-Z][a-zA-Z_0-9]*

lit_char        '[^\"'\\]'|'\\[abfnrtv'\"\\\?]'

err_lit_char    '[\"'\\]'|''
err_sqmark      '

string          \"[^\"]*\"
err_dqmark      \"



width           :[1-4]

delimiter       [\{\}:;]

dec             [0-9]{1,10}|-[0-9]{1,10}
hex             0x[0-9a-fA-F]{1,8}|-0x[0-9a-fA-F]{1,8}
oct             0[0-7]{1,11}|-0[0-7]{1,11}
bin             0b[01]{1,32}
/*NOTE
If a number starts with 0 but contains 8 or 9, it will be read as decimal*/

/*ids are also atoms, but this is checked in the syntax
This makes things like "if(" two atoms, allowing automatically variants like "if ("*/
atom1           "¬"|[()=!,&%\?\.\|\<\>\[\]\*\^\$\~\-\+\\\/]           
atom            {atom1}+

err_keyword     #{id}?

err_label       @
err_value       _

comment         ##.*\n
white           [ \t\r]+

%%

\n              {column=lastleng=1; line++;
#               ifndef SYNTACTIC
                printf("\n");
#               endif
                }
{comment}       {column=lastleng=1; line++;}


{white}         {column+=lastleng; lastleng=yyleng; 
#               ifndef SYNTACTIC
                printf(" ");
#               endif
                }
                
{bin}           {column+=lastleng; lastleng=yyleng; value(int_t, strtol(yytext,NULL, 2));     output(LITERAL);}
{oct}           {column+=lastleng; lastleng=yyleng; value(int_t, strtol(yytext,NULL, 8));     output(LITERAL);}
{dec}           {column+=lastleng; lastleng=yyleng; value(int_t, strtol(yytext,NULL, 10));    output(LITERAL);}
{hex}           {column+=lastleng; lastleng=yyleng; value(int_t, strtol(yytext,NULL, 16));    output(LITERAL);}
{lit_char}      {column+=lastleng; lastleng=yyleng; value(int_t, read_lit_char(yytext));      output(LITERAL);}

{string}        {column+=lastleng; lastleng=yyleng; char* a = malloc2(yyleng-1);
                 strncpy(a, &yytext[1], yyleng-2);
                 a[yyleng-2] = '\0'; 
                 remove_escape_sequences(a);
                 a = realloc2(a, strlen(a)+1);
                 valuestr(str, a); output(STRING); if_lex_free(a);
                }

{err_lit_char}  {column+=lastleng; lastleng=yyleng; err_lex("Illegal character \"%s\".", yytext);}

_{id}           {column+=lastleng; lastleng=yyleng; valuestr(str, yytext); output(VALUE);}
@{id}           {column+=lastleng; lastleng=yyleng; valuestr(str, yytext); output(LABEL);}

{id}            {column+=lastleng; lastleng=yyleng; valuestr(str, yytext); output(ID);}

\{              {column+=lastleng; lastleng=yyleng; push_context(); outputc('{');}

"#let"          {column+=lastleng; lastleng=yyleng; output(DECLARE);}
"#export"       {column+=lastleng; lastleng=yyleng; output(EXPORT);}
"#import"       {column+=lastleng; lastleng=yyleng; output(IMPORT);}
"#lang"         {column+=lastleng; lastleng=yyleng; output(LANGUAGE);}
"#target"       {column+=lastleng; lastleng=yyleng; output(TARGET);}
"#resource"     {column+=lastleng; lastleng=yyleng; output(RESOURCE);}
"#raw"          {column+=lastleng; lastleng=yyleng; output(RAW);}
"#string"       {column+=lastleng; lastleng=yyleng; output(RAW_STRING);}

{width}         {column+=lastleng; lastleng=yyleng; int val = yytext[1] - '0'; value(int_t, val); output(WIDTH);}

{delimiter}     {column+=lastleng; lastleng=yyleng; outputc(yytext[0]);}

{atom}          {column+=lastleng; lastleng=yyleng; valuestr(str, yytext); output(ATOM);}

{err_label}     {column+=lastleng; lastleng=yyleng; err_lex("Unvalid label.");}
{err_keyword}   {column+=lastleng; lastleng=yyleng; err_lex("Keyword \"%s\" unknown.", yytext);}
{err_value}     {column+=lastleng; lastleng=yyleng; err_lex("Value not valid.");}

{err_sqmark}    {column+=lastleng; lastleng=yyleng; err_lex("Missing closing \'.");}
{err_dqmark}    {column+=lastleng; lastleng=yyleng; err_lex("Missing closing \".");}

.               {column+=lastleng; lastleng=yyleng; err_lex("Illegal token.");}

%%
