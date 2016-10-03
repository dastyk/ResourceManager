%skeleton "lalr1.cc" 
%defines
%define api.value.type variant
%define api.token.constructor
%code requires{
	#include "Object.h"
	#include <vector>
	#include <stdint.h>
}
%code{
  #define YY_DECL yy::parser::symbol_type yylex()
   Object* o;
  YY_DECL;
}

%type <void*> line
%type <void*> statement
%type <std::vector<std::vector<uint64_t>>> createface
%type <float> optreal
%type <float> optreal2
%type <std::vector<uint64_t>> indices

%token <float> REAL
%token <uint64_t> INTEGER
%token <std::string> POSITION
%token <std::string> TEXCOORD
%token <std::string> NORMAL
%token <std::string> POINT
%token <std::string> FACE
%token <std::string> OBJECT
%token <std::string> S
%token <std::string> MTLLIB
%token <std::string> USEMTL
%token <std::string> NAME
%token <bool> BOOLEAN
%token <std::string> SEP
%token END 0 "end of file"

%%


line			: statement									{ }
				| line statement							{ }
				;

statement		: OBJECT NAME 								{ o->AddSubMesh($2); }		
				| MTLLIB NAME								{ }
				| POSITION REAL REAL REAL optreal			{ o->AddPosition(Position($2,$3,$4)); }
				| TEXCOORD REAL REAL optreal2				{ o->AddTexCoord(TexCoord($2,$3));}
				| NORMAL REAL REAL REAL						{ o->AddNormal(Normal($2,$3,$4)); }
				| POINT REAL REAL optreal					{ }
				| USEMTL NAME								{}					
				| S BOOLEAN									{ }
				| S INTEGER 								{ }
				| FACE createface							{ o->AddFace(Face($2)); }
				;	
	
optreal			: REAL										{ $$ = $1; }
				| /*empty*/									{ $$ = 1.0f; }
				;
				
optreal2		: REAL										{ $$ = $1;}
				| /*empty*/									{ $$ = 0.0f;}
				;
				
createface		: indices 									{ $$.push_back($1); }
				| createface indices						{ $$ = $1; $$.push_back($2);}
				;
				
indices			: INTEGER									{ $$.push_back($1); }
				| indices SEP INTEGER						{ $$ = $1; $$.push_back($3);}		
				;	