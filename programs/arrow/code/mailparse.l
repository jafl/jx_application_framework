
%{
#include <stdio.h>
%}


%x SUBJECT_STATE HEADER_STATE TOSS_STATE FROM_STATE

FROMLINE ^From[[:space:]]+.*.{3}[[:space:]]+.{3}[[:space:]]+[[:digit:]]+[[:space:]]+[[:digit:]]+:[[:digit:]]+:[[:digit:]]+[[:space:]]+[[:digit:]]{4}

%%

	int hitsub = 0;

{FROMLINE} {
	hitsub = 0;
	BEGIN(HEADER_STATE);
}

.|\n ;

<HEADER_STATE>{

^Subject: {
	hitsub = 1;
	BEGIN(SUBJECT_STATE);
	}

^From: {
	BEGIN(FROM_STATE);
	}

^[^:\n]*: BEGIN(TOSS_STATE);

^\n {
	if (hitsub == 0)
		{
		printf("(No subject)");
		}
	printf("\n");
	BEGIN(INITIAL);
	}

.|\n ;

}

<SUBJECT_STATE>{

[^[:space:]\n].*	{

	printf("%-32.30s ", yytext);
	BEGIN(HEADER_STATE);
	}

\n BEGIN(HEADER_STATE);

. ;

}

<FROM_STATE>{

[^[:space:]\n].*	{
	printf("%-22.20s	", yytext);
	}

\n BEGIN(HEADER_STATE);

. ;

}

<TOSS_STATE>{

\n BEGIN(HEADER_STATE);

. ;

}

%%

main
	(
	int argc,
	char** argv
	)
{
	if (argc > 1)
		{
		yyin = fopen(argv[1], "r");
		}
	else
		{
		yyin = stdin;
		}
	yylex();
}
