%top{
#include "parser.tab.h"
#define YY_DECL yy::parser::symbol_type yylex()
}
%option noyywrap nounput batch noinput
%x comment
ws      [ \t\n]+

%%

"#"            { BEGIN(comment); }
<comment>"\n" { BEGIN(INITIAL); }
<comment>.    { }


{ws}									{}


[0-9]+												{ return yy::parser::make_INTEGER(std::stol(yytext));}
[-+]?[0-9]*\.?[0-9]+([eE][-+]?[0-9]+)?				{ return yy::parser::make_REAL(std::stof(yytext));}
v													{ return yy::parser::make_POSITION(yytext);}
vt													{ return yy::parser::make_TEXCOORD(yytext);}
vn													{ return yy::parser::make_NORMAL(yytext);}
vp													{ return yy::parser::make_POINT(yytext);}
o													{ return yy::parser::make_OBJECT(yytext);}
g													{ return yy::parser::make_OBJECT(yytext);}
s													{ return yy::parser::make_S(yytext);}
f													{ return yy::parser::make_FACE(yytext);}

mtllib												{ return yy::parser::make_MTLLIB(yytext);}
usemtl												{ return yy::parser::make_USEMTL(yytext);}

off													{ return yy::parser::make_BOOLEAN(false);}
on													{ return yy::parser::make_BOOLEAN(true);}

[a-zA-Z0-9._]+										{ return yy::parser::make_NAME(yytext);}

\/													{ return yy::parser::make_SEP("true");}
<<EOF>>      										{ return yy::parser::make_END();}
%%