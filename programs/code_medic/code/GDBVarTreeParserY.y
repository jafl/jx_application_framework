%{
#include "CMVarNode.h"
#include "GDBVarGroupInfo.h"
#include "cmGlobals.h"

// also uncomment yydebug=1; below
//#define YYERROR_VERBOSE
//#define YYDEBUG 1

inline JBoolean
cmIsOpenablePointer
	(
	const JString& s
	)
{
	return JNegate( s == "0x0" );
}

%}

%union {
	JString*				pString;
	CMVarNode*				pNode;
	JPtrArray<CMVarNode>*	pList;
	GDBVarGroupInfo*		pGroup;
}

%{
#define NOTAB
#include "GDBVarTreeParser.h"

#define yyparse GDBVarTreeParser::yyparse
%}

%pure-parser

%token<pString>	P_NAME P_NAME_EQ
%token<pString>	P_EMPTY_BRACKET P_EMPTY_BRACKET_EQ
%token<pString> P_NO_DATA_FIELDS
%token<pString>	P_INTEGER P_HEX P_FLOAT P_CHAR P_STRING P_EMPTY_SUMMARY
%token<pString>	P_STATIC
%token<pString>	P_BRACKET P_BRACKET_EQ
%token<pString> P_GROUP_OPEN P_GROUP_CLOSE
%token<pString>	P_PAREN_EXPR P_SUMMARY
%token			P_EOF

%type<pNode>	value_node node top_group
%type<pString>	name name_eq value reference_value
%type<pList>	value_list node_list
%type<pGroup>	group

%{

// debugging output

#define YYPRINT(file, type, value)	yyprint(file, type, value)
#include <stdio.h>

inline void
yyprint
	(
	FILE*	file,
	int		type,
	YYSTYPE	value
	)
{
	if (value.pString != nullptr)
		{
		fprintf(file, "string:  %s", (value.pString)->GetCString());
		}
}

%}

%{
#include <jAssert.h>	// must be last
%}

%initial-action
{
	itsGroupDepth   = 0;
	itsGDBErrorFlag = kJFalse;
//	yydebug         = 1;
}

%%
top_group :

	group P_EOF
		{
		itsCurrentNode = $$ = (CMGetLink())->CreateVarNode(nullptr, "", nullptr, $1->GetName());
		for (JIndex i=1; i<=$1->list->GetElementCount(); i++)
			{
			itsCurrentNode->Append($1->list->GetElement(i));
			}
		itsIsPointerFlag = kJFalse;

		jdelete $1;

		YYACCEPT;
		}

	| reference_value group P_EOF
		{
		itsCurrentNode = $$ = (CMGetLink())->CreateVarNode(nullptr, "", nullptr, *$1);
		for (JIndex i=1; i<=$2->list->GetElementCount(); i++)
			{
			itsCurrentNode->Append($2->list->GetElement(i));
			}
		itsIsPointerFlag = kJFalse;

		jdelete $1;
		jdelete $2;

		YYACCEPT;
		}

	/* These are needed for a simple value, like a = 3 */

	| value P_EOF
		{
		itsCurrentNode = $$ = (CMGetLink())->CreateVarNode(nullptr, "", nullptr, *$1);
		itsCurrentNode->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = kJFalse;

		jdelete $1;

		YYACCEPT;
		}

	| reference_value value P_EOF
		{
		itsCurrentNode = $$ = (CMGetLink())->CreateVarNode(nullptr, "", nullptr, *$1);
		CMVarNode* child = (CMGetLink())->CreateVarNode(itsCurrentNode, "", nullptr, *$2);
		child->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = kJFalse;

		jdelete $1;
		jdelete $2;

		YYACCEPT;
		}
	;

group :

	P_GROUP_OPEN node_list P_GROUP_CLOSE
		{
		$$ = jnew GDBVarGroupInfo(nullptr, $2);
		assert( $$ != nullptr );
		}

	| P_GROUP_OPEN value_list P_GROUP_CLOSE
		{
		$$ = jnew GDBVarGroupInfo(nullptr, $2);
		assert( $$ != nullptr );
		}

	| P_GROUP_OPEN group P_GROUP_CLOSE
		{
		JPtrArray<CMVarNode>* list = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		AppendAsArrayElement("", *($2->list), list);
		$$ = jnew GDBVarGroupInfo(nullptr, list);
		assert( $$ != nullptr );

		jdelete $2;
		}

	| P_SUMMARY P_GROUP_OPEN node_list P_GROUP_CLOSE
		{
		$$ = jnew GDBVarGroupInfo($1, $3);
		assert( $$ != nullptr );
		}

	| P_SUMMARY P_GROUP_OPEN value_list P_GROUP_CLOSE
		{
		$$ = jnew GDBVarGroupInfo($1, $3);
		assert( $$ != nullptr );
		}

	| P_SUMMARY P_GROUP_OPEN group P_GROUP_CLOSE
		{
		JPtrArray<CMVarNode>* list = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		AppendAsArrayElement($3->GetName(), *($3->list), list);
		$$ = jnew GDBVarGroupInfo($1, list);
		assert( $$ != nullptr );

		jdelete $3;
		}
	;

node_list :

	node
		{
		JPtrArray<CMVarNode>* list = $$ = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		list->Append($1);
		}

	| node_list ',' node
		{
		$$ = $1;
		$$->Append($3);
		}

	| node_list ',' P_EMPTY_SUMMARY
		{
		$$ = $1;

		CMVarNode* node = (CMGetLink())->CreateVarNode(nullptr, "", nullptr, *$3);
		if ((($$->FirstElement())->GetName()).BeginsWith("["))
			{
			AppendAsArrayElement(node, $$);
			}
		else
			{
			$$->Append(node);
			}

		jdelete $3;
		}

	| group ',' group
		{
		JPtrArray<CMVarNode>* list = $$ = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		AppendAsArrayElement($1->GetName(), *($1->list), list);
		AppendAsArrayElement($3->GetName(), *($3->list), list);

		jdelete $1;
		jdelete $3;
		}

	| node_list ',' group
		{
		if ((($1->FirstElement())->GetName()).BeginsWith("["))
			{
			$$ = $1;
			}
		else
			{
			JPtrArray<CMVarNode>* list = $$ = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
			assert( list != nullptr );
			AppendAsArrayElement("", *$1, list);

			jdelete $1;
			}

		AppendAsArrayElement($3->GetName(), *($3->list), $$);

		jdelete $3;
		}

	| node_list '.' '.' '.'
		{
		$$ = $1;
		CMVarNode* child = (CMGetLink())->CreateVarNode(nullptr, "...", nullptr, "");
		$$->Append(child);
		}
	;

node :

	name_eq value
		{
		if ($1->EndsWith("="))
			{
			$1->RemoveSubstring($1->GetLength(), $1->GetLength());
			$1->TrimWhitespace();
			}
		itsCurrentNode = $$ = (CMGetLink())->CreateVarNode(nullptr, *$1, nullptr, *$2);
		if (!$1->BeginsWith("_vptr.") && !$1->BeginsWith("_vb."))
			{
			itsCurrentNode->MakePointer(itsIsPointerFlag);
			}
		itsIsPointerFlag = kJFalse;

		jdelete $1;
		jdelete $2;
		}

	| name_eq group
		{
		if ($1->EndsWith("="))
			{
			$1->RemoveSubstring($1->GetLength(), $1->GetLength());
			$1->TrimWhitespace();
			}
		itsCurrentNode = $$ = (CMGetLink())->CreateVarNode(nullptr, *$1, nullptr, $2->GetName());
		itsIsPointerFlag = kJFalse;
		for (JIndex i=1; i<=$2->list->GetElementCount(); i++)
			{
			itsCurrentNode->Append($2->list->GetElement(i));
			}

		jdelete $1;
		jdelete $2;
		}

	| P_NO_DATA_FIELDS
		{
		itsCurrentNode = $$ = (CMGetLink())->CreateVarNode(nullptr, *$1, nullptr, "");
		itsIsPointerFlag = kJFalse;

		jdelete $1;
		}

	| name_eq
		{
		if ($1->EndsWith("="))
			{
			$1->RemoveSubstring($1->GetLength(), $1->GetLength());
			$1->TrimWhitespace();
			}
		itsCurrentNode = $$ = (CMGetLink())->CreateVarNode(nullptr, *$1, nullptr, "<nothing>");
		itsIsPointerFlag = kJFalse;

		jdelete $1;
		}
	;

name_eq :

	P_NAME_EQ

	| P_STATIC P_NAME_EQ
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_BRACKET_EQ

	| P_STATIC P_BRACKET_EQ
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_NAME P_BRACKET_EQ
		{
		$$ = $1;
		$$->Append(*$2);

		jdelete $2;
		}

	| P_STATIC P_NAME P_BRACKET_EQ
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);
		$$->Append(*$3);

		jdelete $2;
		jdelete $3;
		}

	| P_EMPTY_BRACKET_EQ
	;

value_list :

	value_node
		{
		JPtrArray<CMVarNode>* list = $$ = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
		assert( list != nullptr );
		AppendAsArrayElement($1, $$);
		}

	| value_list ',' value_node
		{
		$$ = $1;
		AppendAsArrayElement($3, $$);
		}

	| value_list '.' '.' '.'
		{
		$$ = $1;
		CMVarNode* child = (CMGetLink())->CreateVarNode(nullptr, "...", nullptr, "");
		$$->Append(child);
		}
	;

	/* necessary intermediate: use itsIsPointerFlag before next value changes it */

value_node :

	value
		{
		itsCurrentNode = $$ = (CMGetLink())->CreateVarNode(nullptr, "", nullptr, *$1);
		itsCurrentNode->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = kJFalse;

		jdelete $1;
		}
	;

value :

	P_NAME

	| P_EMPTY_BRACKET

	| P_INTEGER

	| P_FLOAT

	| P_HEX
		{
		itsIsPointerFlag = cmIsOpenablePointer(*$1);
		$$ = $1;
		}

	| '@' P_HEX
		{
		itsIsPointerFlag = cmIsOpenablePointer(*$2);
		$$ = $2;
		$$->PrependCharacter('@');
		}

	| P_PAREN_EXPR P_HEX
		{
		itsIsPointerFlag = cmIsOpenablePointer(*$2);
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);

		jdelete $2;
		}

	| P_EMPTY_SUMMARY

	| P_STRING

	| P_HEX P_STRING
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);

		jdelete $2;
		}

	| P_PAREN_EXPR P_HEX P_STRING
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);
		$$->AppendCharacter(' ');
		$$->Append(*$3);

		jdelete $2;
		jdelete $3;
		}

	| P_HEX P_BRACKET
		{
		itsIsPointerFlag = cmIsOpenablePointer(*$1);
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);

		jdelete $2;
		}

	| P_NAME P_BRACKET
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);

		jdelete $2;
		}

	| P_INTEGER P_BRACKET
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);

		jdelete $2;
		}

	| P_FLOAT P_BRACKET
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);

		jdelete $2;
		}

	| P_CHAR P_BRACKET
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);

		jdelete $2;
		}

	| P_BRACKET

	| P_HEX name P_PAREN_EXPR
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);
		$$->Append(*$3);

		jdelete $2;
		jdelete $3;
		}

	| P_NAME P_PAREN_EXPR
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);

		jdelete $2;
		}

	| P_INTEGER P_CHAR
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->Append(*$2);

		jdelete $2;
		}

	| P_GROUP_OPEN P_GROUP_CLOSE
		{
		$$ = jnew JString("{}");
		assert( $$ != nullptr );
		}
	;

name :

	P_NAME

	| P_STATIC P_NAME
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_BRACKET

	| P_STATIC P_BRACKET
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_EMPTY_BRACKET
	;

reference_value :

	P_PAREN_EXPR '@' P_HEX ':'
		{
		$$ = $1;
		$$->AppendCharacter(' ');
		$$->AppendCharacter('@');
		$$->Append(*$3);

		jdelete $3;
		}
	;

%%
