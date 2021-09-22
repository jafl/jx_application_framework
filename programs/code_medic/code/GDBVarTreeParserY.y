%{
#include "CMVarNode.h"
#include "GDBVarGroupInfo.h"
#include "cmGlobals.h"
#include <JStringIterator.h>

// also uncomment yydebug=1; below
//#define YYERROR_VERBOSE
//#define YYDEBUG 1

inline bool
cmIsOpenablePointer
	(
	const JString& s
	)
{
	return s != "0x0";
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

%define api.pure

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
		fprintf(file, "string:  %s", (value.pString)->GetBytes());
		}
}

%}

%{
#include <jAssert.h>	// must be last
%}

%initial-action
{
	itsGroupDepth   = 0;
	itsGDBErrorFlag = false;
//	yydebug         = 1;
}

%%
top_group :

	group P_EOF
		{
		itsCurrentNode = $$ = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, $1->GetName());
		for (JIndex i=1; i<=$1->list->GetElementCount(); i++)
			{
			itsCurrentNode->Append($1->list->GetElement(i));
			}
		itsIsPointerFlag = false;

		jdelete $1;

		YYACCEPT;
		}

	| reference_value group P_EOF
		{
		itsCurrentNode = $$ = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *$1);
		for (JIndex i=1; i<=$2->list->GetElementCount(); i++)
			{
			itsCurrentNode->Append($2->list->GetElement(i));
			}
		itsIsPointerFlag = false;

		jdelete $1;
		jdelete $2;

		YYACCEPT;
		}

	/* These are needed for a simple value, like a = 3 */

	| value P_EOF
		{
		itsCurrentNode = $$ = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *$1);
		itsCurrentNode->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = false;

		jdelete $1;

		YYACCEPT;
		}

	| reference_value value P_EOF
		{
		itsCurrentNode = $$ = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *$1);
		CMVarNode* child = CMGetLink()->CreateVarNode(itsCurrentNode, JString::empty, JString::empty, *$2);
		child->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = false;

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
		AppendAsArrayElement(JString::empty, *($2->list), list);
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

		CMVarNode* node = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *$3);
		if ((($$->GetFirstElement())->GetName()).BeginsWith(JString("[", JString::kNoCopy)))
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
		if ((($1->GetFirstElement())->GetName()).BeginsWith(JString("[", JString::kNoCopy)))
			{
			$$ = $1;
			}
		else
			{
			JPtrArray<CMVarNode>* list = $$ = jnew JPtrArray<CMVarNode>(JPtrArrayT::kForgetAll);
			assert( list != nullptr );
			AppendAsArrayElement(JString::empty, *$1, list);

			jdelete $1;
			}

		AppendAsArrayElement($3->GetName(), *($3->list), $$);

		jdelete $3;
		}

	| node_list '.' '.' '.'
		{
		$$ = $1;
		CMVarNode* child = CMGetLink()->CreateVarNode(nullptr, JString("...", JString::kNoCopy), JString::empty, JString::empty);
		$$->Append(child);
		}
	;

node :

	name_eq value
		{
		if ($1->EndsWith("="))
			{
			JStringIterator iter($1, kJIteratorStartAtEnd);
			iter.RemovePrev();
			$1->TrimWhitespace();
			}
		itsCurrentNode = $$ = CMGetLink()->CreateVarNode(nullptr, *$1, JString::empty, *$2);
		if (!$1->BeginsWith("_vptr.") && !$1->BeginsWith("_vb."))
			{
			itsCurrentNode->MakePointer(itsIsPointerFlag);
			}
		itsIsPointerFlag = false;

		jdelete $1;
		jdelete $2;
		}

	| name_eq group
		{
		if ($1->EndsWith("="))
			{
			JStringIterator iter($1, kJIteratorStartAtEnd);
			iter.RemovePrev();
			$1->TrimWhitespace();
			}
		itsCurrentNode = $$ = CMGetLink()->CreateVarNode(nullptr, *$1, JString::empty, $2->GetName());
		itsIsPointerFlag = false;
		for (JIndex i=1; i<=$2->list->GetElementCount(); i++)
			{
			itsCurrentNode->Append($2->list->GetElement(i));
			}

		jdelete $1;
		jdelete $2;
		}

	| P_NO_DATA_FIELDS
		{
		itsCurrentNode = $$ = CMGetLink()->CreateVarNode(nullptr, *$1, JString::empty, JString::empty);
		itsIsPointerFlag = false;

		jdelete $1;
		}

	| name_eq
		{
		if ($1->EndsWith("="))
			{
			JStringIterator iter($1, kJIteratorStartAtEnd);
			iter.RemovePrev();
			$1->TrimWhitespace();
			}
		itsCurrentNode = $$ = CMGetLink()->CreateVarNode(nullptr, *$1, JString::empty, JString("<nothing>", JString::kNoCopy));
		itsIsPointerFlag = false;

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
		CMVarNode* child = CMGetLink()->CreateVarNode(nullptr, JString("...", JString::kNoCopy), JString::empty, JString::empty);
		$$->Append(child);
		}
	;

	/* necessary intermediate: use itsIsPointerFlag before next value changes it */

value_node :

	value
		{
		itsCurrentNode = $$ = CMGetLink()->CreateVarNode(nullptr, JString::empty, JString::empty, *$1);
		itsCurrentNode->MakePointer(itsIsPointerFlag);
		itsIsPointerFlag = false;

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
		$$->Prepend("@");
		}

	| P_PAREN_EXPR P_HEX
		{
		itsIsPointerFlag = cmIsOpenablePointer(*$2);
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_EMPTY_SUMMARY

	| P_STRING

	| P_HEX P_STRING
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_PAREN_EXPR P_HEX P_STRING
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);
		$$->Append(" ");
		$$->Append(*$3);

		jdelete $2;
		jdelete $3;
		}

	| P_HEX P_BRACKET
		{
		itsIsPointerFlag = cmIsOpenablePointer(*$1);
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_NAME P_BRACKET
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_INTEGER P_BRACKET
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_FLOAT P_BRACKET
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_CHAR P_BRACKET
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_BRACKET

	| P_HEX name P_PAREN_EXPR
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);
		$$->Append(*$3);

		jdelete $2;
		jdelete $3;
		}

	| P_NAME P_PAREN_EXPR
		{
		$$ = $1;
		$$->Append(" ");
		$$->Append(*$2);

		jdelete $2;
		}

	| P_INTEGER P_CHAR
		{
		$$ = $1;
		$$->Append(" ");
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
		$$->Append(" ");
		$$->Append("@");
		$$->Append(*$3);

		jdelete $3;
		}

	| '@' P_HEX ':'
		{
		$$ = $2;
		$$->Prepend("@");
		}
	;

%%
