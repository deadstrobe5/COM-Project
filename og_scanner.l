%option c++ prefix="og_scanner_" outfile="og_scanner.cpp"
%option stack noyywrap yylineno 8bit
%{ 
// make relevant includes before including the parser's tab file
#include <string>
#include <sstream>
#include <cdk/ast/sequence_node.h>
#include <cdk/ast/expression_node.h>
#include <cdk/ast/lvalue_node.h>
#include "og_parser.tab.h"

static std::ostringstream buffer;

// don't change this
#define yyerror LexerError
%}
%x X_STRING X_BACKSLASH X_IGNORE
%x X_COMMENT
%%

  /* ====================================================================== */
  /* ====[                  3.1 Caracteres Brancos                    ]==== */
  /* ====[                     3.2 Comentários                        ]==== */
  /* ====================================================================== */

[ \n\r\t]              ; /* ignore white characteres */

"//".*\n               ; /* ignore comments */

"/*"                   yy_push_state(X_COMMENT);
<X_COMMENT>"/*"        yy_push_state(X_COMMENT);
<X_COMMENT>"*/"        yy_pop_state();
<X_COMMENT>.|\n        ; /* ignore content */

  /* ====================================================================== */
  /* ====[                3.5 Operadores de expressões                ]==== */
  /* ====[              3.6 Delimitadores e terminadores              ]==== */
  /* ====[                 7.2.2 Indexação de tuplos                  ]==== */
  /* ====================================================================== */

[-?()+*/%~=<>;,@{}]     return *yytext;
"]"|"["                 return *yytext;

">="                    return tGE; 
"<="                    return tLE; 
"=="                    return tEQ; 
"!="                    return tNE; 
"&&"                    return tAND;
"||"                    return tOR;

  /* ====================================================================== */
  /* ====[                         3.4 Tipos                          ]==== */
  /* ====[                      3.8.4 Ponteiros                       ]==== */
  /* ====[                    4.5 Símbolos globais                    ]==== */
  /* ====================================================================== */

"int"				           return tINT_TYPE;
"real"				         return tREAL_TYPE;
"string"			         return tSTR_TYPE;
"auto"				         return tAUTO;
"ptr"				           return tPTR;

"nullptr"			         return tNULLPTR;

"public"			         return tPUBLIC;
"require"			         return tREQUIRE;

  /* ====================================================================== */
  /* ====[                 5.1 Declaração(procedure)                  ]==== */
  /* ====[                        6 Instruções                        ]==== */
  /* ====[                       7.1.2 Leitura                        ]==== */
  /* ====[                7.2.6 Expressão de dimensão                 ]==== */
  /* ====================================================================== */

"sizeof"			         return tSIZEOF;

"procedure"			       return tPROCEDURE;

"break"				         return tBREAK;
"continue"			       return tCONTINUE;
"return"			         return tRETURN;


"input"				         return tINPUT;
"write"				         return tWRITE;
"writeln"			         return tWRITELN;

"for"                  return tFOR; 
"if"                   return tIF; 
"then"                 return tTHEN;
"else"                 return tELSE; 
"elif"                 return tELSIF;
"do"                   return tDO;

  /* ====================================================================== */
  /* ====[                    3.7 Identificadores                     ]==== */
  /* ====================================================================== */

[A-Za-z][A-Za-z0-9_]*     yylval.s = new std::string(yytext); return tID;

  /* ====================================================================== */
  /* ====[                      3.4.1 Inteiros                        ]==== */
  /* ====================================================================== */

[0-9]+                 yylval.i = std::stoi(yytext, NULL, 10); return tINTEGER;

"0x"[0-9a-fA-F]+       yylval.i = std::stoi(yytext, NULL, 16); return tINTEGER;
"0x"[0-9a-zA-Z]+       yyerror("Invalid hexadecimal number.");

  /* ====================================================================== */
  /* ====[              3.4.2 Reais em vírgula flutuante              ]==== */
  /* ====================================================================== */

(([0-9]+(\.[0-9]+)?)|(\.[0-9]+))([Ee][-+]?[0-9]+)?   yylval.d = strtod(yytext, NULL); return tREAL;

  /* ====================================================================== */
  /* ====[                3.4.3 Cadeias de caracteres                 ]==== */
  /* ====================================================================== */

\"                                 yy_push_state(X_STRING);
<X_STRING>\"                       {
                                    yylval.s = new std::string(buffer.str());
                                    buffer.str("");
                                    yy_pop_state();
                                    return tSTRING;
                                   }
<X_STRING>\\                       yy_push_state(X_BACKSLASH);
<X_STRING>.                        buffer << *yytext;

<X_BACKSLASH>00?                   yy_push_state(X_IGNORE);
<X_BACKSLASH>\"                    buffer << '\"'; yy_pop_state();
<X_BACKSLASH>\\                    buffer << '\\'; yy_pop_state();
<X_BACKSLASH>n                     buffer << '\n'; yy_pop_state();
<X_BACKSLASH>r                     buffer << '\r'; yy_pop_state();
<X_BACKSLASH>t                     buffer << '\t'; yy_pop_state();
<X_BACKSLASH>[0-9a-fA-F]{1,2}      buffer << (char)(unsigned char) strtoul(yytext, NULL, 16); yy_pop_state();
<X_BACKSLASH>.                     buffer << '\\' << *yytext; yy_pop_state();

<X_IGNORE>\"                       yyless(0); yy_pop_state(); yy_pop_state(); /* Volta para X_STRING e termina */
<X_IGNORE>\\\"                     ; /* Ignore */
<X_IGNORE>\\\\                     ; /* Ignore */
<X_IGNORE>.                        ; /* Ignore */

.                                  yyerror("Unknown character");

%%
