%{
//-- don't change *any* of these: if you do, you'll break the compiler.
#include <cdk/compiler.h>
#include "ast/all.h"
#define LINE               compiler->scanner()->lineno()
#define yylex()            compiler->scanner()->scan()
#define yyerror(s)         compiler->scanner()->error(s)
#define YYPARSE_PARAM_TYPE std::shared_ptr<cdk::compiler>
#define YYPARSE_PARAM      compiler
//-- don't change *any* of these --- END!
%}

%union {
  int                               i;	/* integer value */
  double                            d;    /* double value */
  std::string                      *s;	/* symbol name or string literal */
  std::vector<std::string>   *strings;  /* vector of strings */
  cdk::basic_node               *node;	/* node pointer */
  cdk::typed_node              *typed; 
  cdk::sequence_node        *sequence;
  cdk::expression_node    *expression; /* expression nodes */
  cdk::lvalue_node            *lvalue;

  og::tuple_node               *tuple;

  og::block_node               *block;
  og::for_node                  *cond;

  cdk::basic_type               *type;
  cdk::reference_type      *reference;
}

%token <i> tINTEGER
%token <d> tREAL
%token <s> tID tSTRING
%token <expression> tNULLPTR 

%token tINT_TYPE tREAL_TYPE tSTR_TYPE tAUTO tPTR
%token tPUBLIC tREQUIRE 

%token tPROCEDURE tBREAK tCONTINUE tSIZEOF tRETURN
%token tINPUT tWRITE tWRITELN 
%token tFOR tIF tTHEN tELSE tELSIF
%token tGE tLE tEQ tNE tAND tOR tDO

%type<node> declaration instruction conditional elsif
%type<typed> function_def function_decl variable ivariable iivariable fvariable
%type<sequence> file declarations idecls for_variables iivariables fvariables expressions instructions
%type<expression> expr literal
%type<lvalue> lval
%type<tuple> tuple

%type<block> block
%type<cond> iteration

%type<strings> identifiers
%type<s> string

%type<type> type
%type<reference> pointer

%nonassoc tTHEN
%nonassoc tELSE tELSIF

%nonassoc '='
%left tOR
%left tAND
%nonassoc '~'
%left tEQ tNE
%left tGE tLE '>' '<'
%left '+' '-'
%left '*' '/' '%'
%nonassoc tUNARY
%nonassoc '['
%nonassoc '@'

%{
//-- The rules below will be included in yyparse, the main parsing function.
%}
%%


file           : declarations { compiler->ast($$ = $1);  }
               ;


declarations   : declarations declaration { $$ = new cdk::sequence_node(LINE, $2, $1); }
               | declaration              { $$ = new cdk::sequence_node(LINE, $1); }
               ;


declaration    : function_decl  { $$ = $1; }
               | function_def   { $$ = $1; }
               | variable  ';'  { $$ = $1; }
               ;


variable       : tPUBLIC  type tID '=' expr                  { $$ = new og::variable_declaration_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3, $5); delete $3; }
               | tREQUIRE type tID '=' expr                  { $$ = new og::variable_declaration_node(LINE, 2, std::shared_ptr<cdk::basic_type>($2), *$3, $5); delete $3; }
               |          type tID '=' expr                  { $$ = new og::variable_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, $4); delete $2; }
               | tPUBLIC  type tID                           { $$ = new og::variable_declaration_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3, nullptr); delete $3; }
               | tREQUIRE type tID                           { $$ = new og::variable_declaration_node(LINE, 2, std::shared_ptr<cdk::basic_type>($2), *$3, nullptr); delete $3; }
               |          type tID                           { $$ = new og::variable_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, nullptr); delete $2; }
               /* Multiple variables or tuples */
               | tPUBLIC  tAUTO identifiers '=' tuple  { $$ = new og::tuple_declaration_node(LINE, 1, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), $3, $5); }
               |          tAUTO identifiers '=' tuple  { $$ = new og::tuple_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), $2, $4); }
               ;


function_decl  : tPUBLIC  type  tID '(' fvariables ')'         { $$ = new og::function_declaration_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3, $5); delete $3; }
               | tREQUIRE type  tID '(' fvariables ')'         { $$ = new og::function_declaration_node(LINE, 2, std::shared_ptr<cdk::basic_type>($2), *$3, $5); delete $3; }
               | tPUBLIC  type  tID '('            ')'         { $$ = new og::function_declaration_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3, nullptr); delete $3; } 
               | tREQUIRE type  tID '('            ')'         { $$ = new og::function_declaration_node(LINE, 2, std::shared_ptr<cdk::basic_type>($2), *$3, nullptr); delete $3; } 
               /* Com auto */
               | tPUBLIC  tAUTO tID '(' fvariables ')'        { $$ = new og::function_declaration_node(LINE, 1, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$3, $5); delete $3; }
               | tREQUIRE tAUTO tID '(' fvariables ')'        { $$ = new og::function_declaration_node(LINE, 2, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$3, $5); delete $3; }
               | tPUBLIC  tAUTO tID '('            ')'        { $$ = new og::function_declaration_node(LINE, 1, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$3, nullptr); delete $3; }
               | tREQUIRE tAUTO tID '('            ')'        { $$ = new og::function_declaration_node(LINE, 2, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$3, nullptr); delete $3; }
               /* Sem qualifier */
               |          type  tID '(' fvariables ')'        { $$ = new og::function_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, $4); delete $2; }
               |          type  tID '('            ')'        { $$ = new og::function_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, nullptr); delete $2; }
               |          tAUTO tID '(' fvariables ')'        { $$ = new og::function_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$2, $4); delete $2; }
               |          tAUTO tID '('            ')'        { $$ = new og::function_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$2, nullptr); delete $2; }
               /* Procedures */
               | tPUBLIC  tPROCEDURE tID '(' fvariables ')'       { $$ = new og::function_declaration_node(LINE, 1, *$3, $5); delete $3; }
               | tREQUIRE tPROCEDURE tID '(' fvariables ')'       { $$ = new og::function_declaration_node(LINE, 2, *$3, $5); delete $3; }
               | tPUBLIC  tPROCEDURE tID '('            ')'       { $$ = new og::function_declaration_node(LINE, 1, *$3, nullptr); delete $3; }
               | tREQUIRE tPROCEDURE tID '('            ')'       { $$ = new og::function_declaration_node(LINE, 2, *$3, nullptr); delete $3; }
               |          tPROCEDURE tID '(' fvariables ')'       { $$ = new og::function_declaration_node(LINE, 0, *$2, $4); delete $2; }
               |          tPROCEDURE tID '('            ')'       { $$ = new og::function_declaration_node(LINE, 0, *$2, nullptr); delete $2; }
               ;

function_def   : tPUBLIC  type  tID '(' fvariables ')' block   { $$ = new og::function_definition_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3, $5, $7); delete $3; }
               | tPUBLIC  type  tID '('            ')' block   { $$ = new og::function_definition_node(LINE, 1, std::shared_ptr<cdk::basic_type>($2), *$3, nullptr, $6); delete $3; }
               /* Com auto */
               | tPUBLIC  tAUTO tID '(' fvariables ')' block  { $$ = new og::function_definition_node(LINE, 1, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$3, $5, $7); delete $3; }
               | tPUBLIC  tAUTO tID '('            ')' block  { $$ = new og::function_definition_node(LINE, 1, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$3, nullptr, $6); delete $3; }
               /* Sem public */
               |          type  tID '(' fvariables ')' block  { $$ = new og::function_definition_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, $4, $6); delete $2; }
               |          type  tID '('            ')' block  { $$ = new og::function_definition_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, nullptr, $5); delete $2; }
               |          tAUTO tID '(' fvariables ')' block  { $$ = new og::function_definition_node(LINE, 0, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$2, $4, $6); delete $2; }
               |          tAUTO tID '('            ')' block  { $$ = new og::function_definition_node(LINE, 0, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), *$2, nullptr, $5); delete $2; }
               /* Procedures */
               | tPUBLIC  tPROCEDURE tID '(' fvariables ')' block { $$ = new og::function_definition_node(LINE, 1, *$3, $5, $7); delete $3; }
               | tPUBLIC  tPROCEDURE tID '('            ')' block { $$ = new og::function_definition_node(LINE, 1, *$3, nullptr, $6); delete $3; }
               |          tPROCEDURE tID '(' fvariables ')' block { $$ = new og::function_definition_node(LINE, 0, *$2, $4, $6); delete $2; }
               |          tPROCEDURE tID '('            ')' block { $$ = new og::function_definition_node(LINE, 0, *$2, nullptr, $5); delete $2; }
               ;  

               

/* -----------------   fvariables : variáveis, mas com as restrições:        ----------------- */
/* -----------------   -> Não podem existir qualificadores (Public/Require)  ----------------- */
/* -----------------   -> Não podem ser do tipo auto                         ----------------- */    
/* -----------------   -> Não podem ser inicializadas                        ----------------- */           

fvariables     : fvariables ',' fvariable            { $$ = new cdk::sequence_node(LINE, $3, $1); }
               | fvariable                           { $$ = new cdk::sequence_node(LINE, $1); }
               ;

fvariable      : type tID       { $$ = new og::variable_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, nullptr); delete $2; }
               ;


identifiers    : identifiers ',' tID                    { $1->push_back(*$3); delete $3;
                                                          $$ = $1;
                                                        }
               | tID                                    { std::vector<std::string> *s = new std::vector<std::string>();
                                                          s->push_back(*$1); delete $1;
                                                          $$ = s;  
                                                        }
               ;

type           : tINT_TYPE                              { $$ = new cdk::primitive_type(4, cdk::typename_type::TYPE_INT); }
               | tREAL_TYPE                             { $$ = new cdk::primitive_type(8, cdk::typename_type::TYPE_DOUBLE); }
               | tSTR_TYPE                              { $$ = new cdk::primitive_type(4, cdk::typename_type::TYPE_STRING); }
               | pointer                                { $$ = $1; }
               ;

pointer        : tPTR '<' type '>'                      { $$ = new cdk::reference_type(4, std::shared_ptr<cdk::basic_type>($3)); }
               | tPTR '<' tAUTO '>'                     
               { $$ = new cdk::reference_type(4, std::shared_ptr<cdk::basic_type>(std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()))); }
               ;

expressions    : expressions ',' expr                   { $$ = new cdk::sequence_node(LINE, $3, $1); }
               | expr                                   { $$ = new cdk::sequence_node(LINE, $1); }
               ;

expr           : literal                                                              { $$ = $1; }
               /* Chamadas a funções */
               | tID '(' expressions ')'                                              { $$ = new og::function_call_node(LINE, *$1, $3); delete $1; }
               | tID '(' /* empty */ ')'                                              { $$ = new og::function_call_node(LINE, *$1, nullptr); delete $1; }
               /* Left values */
               | lval                                                                 { $$ = new cdk::rvalue_node(LINE, $1); }
               | lval '=' expr                                                        { $$ = new cdk::assignment_node(LINE, $1, $3); }
               | lval '?'                                                             { $$ = new og::position_indicator_node(LINE, $1); }
               /* Expressões com operadores */
               |      '~' expr                                                        { $$ = new cdk::not_node(LINE, $2); }
               |      '-' expr %prec tUNARY                                           { $$ = new cdk::neg_node(LINE, $2); }
               |      '+' expr %prec tUNARY                                           { $$ = new og::identity_node(LINE, $2); }
               | expr '+' expr	                                                      { $$ = new cdk::add_node(LINE, $1, $3); }
               | expr '-' expr	                                                      { $$ = new cdk::sub_node(LINE, $1, $3); }
               | expr '*' expr	                                                      { $$ = new cdk::mul_node(LINE, $1, $3); }
               | expr '/' expr	                                                      { $$ = new cdk::div_node(LINE, $1, $3); }
               | expr '%' expr	                                                      { $$ = new cdk::mod_node(LINE, $1, $3); }
               | expr '<' expr	                                                      { $$ = new cdk::lt_node(LINE, $1, $3); }
               | expr '>' expr	                                                      { $$ = new cdk::gt_node(LINE, $1, $3); }
               | expr tGE expr	                                                      { $$ = new cdk::ge_node(LINE, $1, $3); }
               | expr tLE expr                                                        { $$ = new cdk::le_node(LINE, $1, $3); }
               | expr tNE expr	                                                      { $$ = new cdk::ne_node(LINE, $1, $3); }
               | expr tEQ expr	                                                      { $$ = new cdk::eq_node(LINE, $1, $3); }
               | expr tOR expr	                                                      { $$ = new cdk::or_node(LINE, $1, $3); }
               | expr tAND expr	                                                      { $$ = new cdk::and_node(LINE, $1, $3); }
               /* Outros */
               | '(' expr ')'                                                         { $$ = $2; }
               | '[' expr ']'                                                         { $$ = new og::memalloc_node(LINE, $2); }
               | tINPUT                                                               { $$ = new og::input_node(LINE); }
               | tSIZEOF '(' tuple ')'                                                { $$ = new og::sizeof_node(LINE, $3); }
               ;


tuple          : expressions                                                          { $$ = new og::tuple_node(LINE, $1); }
               ;


literal        : tINTEGER                                                             { $$ = new cdk::integer_node(LINE, $1); }
               | string                                                               { $$ = new cdk::string_node(LINE, $1); }
               | tREAL                                                                { $$ = new cdk::double_node(LINE, $1); }
               | tNULLPTR                                                             { $$ = new og::nullptr_node(LINE); }
               ;


string         : string tSTRING                                                       { $$ = new std::string(*$1 + *$2); delete $1; delete $2; }
               | tSTRING                                                              { $$ = $1; }
               ;


lval           : tID                                                                  { $$ = new cdk::variable_node(LINE, *$1); delete $1; }
               /* Indexação de ponteiros */
               | expr '[' expr ']'                                                    { $$ = new og::pointer_index_node(LINE, $1, $3); }
               /* Indexação de tuplos */
               | expr '@' tINTEGER                                                    { $$ = new og::tuple_index_node(LINE, $1, new cdk::integer_node(LINE, $3)); }
               ;


block          : '{' idecls instructions '}'                                    { $$ = new og::block_node(LINE, $2, $3); }
               | '{' idecls '}'                                                 { $$ = new og::block_node(LINE, $2, nullptr); }
               | '{' instructions '}'                                                 { $$ = new og::block_node(LINE, nullptr, $2); }
               | '{' /* empty */  '}'                                                 { $$ = new og::block_node(LINE, nullptr, nullptr); }
               ;

/* Declaration without function definitions/declarations */
idecls         : idecls ivariable ';' { $$ = new cdk::sequence_node(LINE, $2, $1); }
               | ivariable ';'        { $$ = new cdk::sequence_node(LINE, $1); }
               ;


/* Variable without qualifiers */
ivariable      :          type tID '=' expr                  { $$ = new og::variable_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, $4); delete $2; }
               |          type tID                           { $$ = new og::variable_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, nullptr); delete $2; }
               /* Multiple variables or tuples */
               |          tAUTO identifiers '=' tuple  { $$ = new og::tuple_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), $2, $4); }
               ;



instructions   : instructions instruction                 { $$ = new cdk::sequence_node(LINE, $2, $1); }
               | instruction                              { $$ = new cdk::sequence_node(LINE, $1); }
               ;


instruction    : expr ';'                         { $$ = new og::evaluation_node(LINE, $1); }
               | tWRITE expressions ';'           { $$ = new og::write_node(LINE, $2, 0); }
               | tWRITELN expressions ';'         { $$ = new og::write_node(LINE, $2, 1); }
               | tBREAK                           { $$ = new og::break_node(LINE); }
               | tCONTINUE                        { $$ = new og::continue_node(LINE); }
               | tRETURN tuple ';'                { $$ = new og::return_node(LINE, $2); }
               | tRETURN /* empty */ ';'          { $$ = new og::return_node(LINE, nullptr); }  
               | conditional                      { $$ = $1; }
               | iteration                        { $$ = $1; }
               | block                            { $$ = $1; }
               ;


iteration      : tFOR  expressions   ';' expressions ';' expressions tDO instruction  { $$ = new og::for_node(LINE, $2, $4, $6, $8); }
               | tFOR                ';' expressions ';' expressions tDO instruction  { $$ = new og::for_node(LINE, nullptr, $3, $5, $7); }
               | tFOR  expressions   ';'             ';' expressions tDO instruction  { $$ = new og::for_node(LINE, $2, nullptr, $5, $7); }
               | tFOR  expressions   ';' expressions ';'             tDO instruction  { $$ = new og::for_node(LINE, $2, $4, nullptr, $7); }
               | tFOR                ';'             ';' expressions tDO instruction  { $$ = new og::for_node(LINE, nullptr, nullptr, $4, $6); }
               | tFOR  expressions   ';'             ';'             tDO instruction  { $$ = new og::for_node(LINE, $2, nullptr, nullptr, $6); }
               | tFOR                ';' expressions ';'             tDO instruction  { $$ = new og::for_node(LINE, nullptr, $3, nullptr, $6); }
               | tFOR                ';'             ';'             tDO instruction  { $$ = new og::for_node(LINE, nullptr, nullptr, nullptr, $5); }
               | tFOR  for_variables ';' expressions ';' expressions tDO instruction  { $$ = new og::for_node(LINE, $2, $4, $6, $8); }
               | tFOR  for_variables ';'             ';' expressions tDO instruction  { $$ = new og::for_node(LINE, $2, nullptr, $5, $7); }
               | tFOR  for_variables ';' expressions ';'             tDO instruction  { $$ = new og::for_node(LINE, $2, $4, nullptr, $7); }
               | tFOR  for_variables ';'             ';'             tDO instruction  { $$ = new og::for_node(LINE, $2, nullptr, nullptr, $6); }
               ;

/* -----------------   for_variables : variáveis, mas com as restrições:     ----------------- */
/* -----------------   -> Não podem existir qualificadores (Public/Require)  ----------------- */
/* -----------------   -> Só poder haver 1 declaração auto                   ----------------- */

for_variables  : iivariables                        { $$ = $1; }
               | tAUTO identifiers '=' tuple  { $$ = new cdk::sequence_node(LINE, new og::tuple_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>(new cdk::primitive_type()), $2, $4)); }
               ;

iivariables    : iivariables ',' iivariable         { $$ = new cdk::sequence_node(LINE, $3, $1); }  
               | iivariable                         { $$ = new cdk::sequence_node(LINE, $1); }
               ;

iivariable     : type tID '=' expr                  { $$ = new og::variable_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, $4); delete $2; }
               | type tID                           { $$ = new og::variable_declaration_node(LINE, 0, std::shared_ptr<cdk::basic_type>($1), *$2, nullptr); delete $2; }
               ; 


conditional    : tIF expr tTHEN instruction              { $$ = new og::if_node(LINE, $2, $4); }
               | tIF expr tTHEN instruction elsif        { $$ = new og::if_else_node(LINE, $2, $4, $5); }
               ;


elsif          : tELSE  instruction                         { $$ = $2; }
               | tELSIF expr tTHEN instruction              { $$ = new og::if_node(LINE, $2, $4); }
               | tELSIF expr tTHEN instruction elsif        { $$ = new og::if_else_node(LINE, $2, $4, $5); }
               ;

%%
