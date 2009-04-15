%{
#define GASOK
#include <GAddressScanner.h>
#include <jAssert.h>
#include <stdio.h>
%}

%option nodefault 8bit stack c++ full align
%option yyclass = "GAddressScanner"
%option prefix="GAS"
%option noyywrap

%x quote comment

%%

\" {
	*itsString += "\"";
	yy_push_state(quote);
	}

\( {
	*itsString += "(";
	yy_push_state(comment);
	}

\, {
	itsString = new JString();
	assert(itsString != NULL);
/*	itsList->InsertSorted(itsString, kJFalse);*/
	itsList->Append(itsString);
	}

[^\"(,]+ {
	*itsString += YYText();
	}

<quote>{

\\. *itsString += YYText();

\\ *itsString += YYText();

[^\\\"]+ *itsString += YYText();

\" {
	*itsString += "\"";
	yy_pop_state();
	}
}

<comment>{

\( {
	*itsString += "(";
	yy_push_state(comment);
	}

\\. *itsString += YYText();

\\ *itsString += YYText();

\" {
	*itsString += "\"";
	yy_push_state(quote);
	}

[^()\"\\]+ *itsString += YYText();

\) {
	*itsString += ")";
	yy_pop_state();
	}
}

%%
