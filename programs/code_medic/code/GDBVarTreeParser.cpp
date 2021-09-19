/******************************************************************************
 GDBVarTreeParser.cpp

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include "GDBVarTreeParser.h"
#include "CMVarNode.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBVarTreeParser::GDBVarTreeParser()
	:
	itsCurrentNode(nullptr),
	itsIsPointerFlag(false)
{
	itsScanner = jnew GDB::VarTree::Scanner;
	assert(itsScanner != nullptr);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBVarTreeParser::~GDBVarTreeParser()
{
	jdelete itsScanner;
	jdelete itsCurrentNode;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

int
GDBVarTreeParser::Parse
	(
	const JString& text
	)
{
	itsScanner->in(text.GetBytes());
	return yyparse();
}

/******************************************************************************
 yylex (private)

	Compensate for gdb stopping on error.

 *****************************************************************************/

int
GDBVarTreeParser::yylex
	(
	YYSTYPE* lvalp
	)
{
	int token = itsScanner->NextToken(lvalp);
	if (token == P_GROUP_OPEN)
	{
		itsGroupDepth++;
	}
	else if (token == P_GROUP_CLOSE && itsGroupDepth > 0)
	{
		itsGroupDepth--;
	}
	else if (token == P_EOF && itsGroupDepth > 0)
	{
		itsGDBErrorFlag = true;

		token          = P_GROUP_CLOSE;
		lvalp->pString = jnew JString("}");
		assert( lvalp->pString != nullptr );

		itsGroupDepth--;
	}

	return token;
}

/******************************************************************************
 yyerror (private)

 *****************************************************************************/

int
GDBVarTreeParser::yyerror
	(
	const char* message
	)
{
	std::cerr << "yyerror() called: " << message << std::endl;

	std::ostringstream log;
	log << "yyerror() called: " << message;
	CMGetLink()->Log(log);

	return 0;
}

/******************************************************************************
 AppendAsArrayElement (private)

 *****************************************************************************/

void
GDBVarTreeParser::AppendAsArrayElement
	(
	CMVarNode*				node,
	JPtrArray<CMVarNode>*	list
	)
	const
{
	const JString name = "[" + JString((JUInt64) list->GetElementCount()) + "]";
	node->SetName(name);
	list->Append(node);
}

/******************************************************************************
 AppendAsArrayElement (private)

 *****************************************************************************/

void
GDBVarTreeParser::AppendAsArrayElement
	(
	const JString&				groupLabel,
	const JPtrArray<CMVarNode>& data,
	JPtrArray<CMVarNode>*		list
	)
	const
{
	const JString name = "[" + JString((JUInt64) list->GetElementCount()) + "]";

	CMVarNode* node = CMGetLink()->CreateVarNode(nullptr, name, JString::empty, groupLabel);
	assert( node != nullptr );
	list->Append(node);

	const JSize count = data.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		node->Append(const_cast<CMVarNode*>(data.GetElement(i)));
		}
}

/******************************************************************************
 ReportRecoverableError

 *****************************************************************************/

void
GDBVarTreeParser::ReportRecoverableError()
{
	if (itsGDBErrorFlag)
		{
		CMVarNode* child;
		itsCurrentNode->GetLastChild(&child);
		child->SetValid(false);

		child = CMGetLink()->CreateVarNode(itsCurrentNode,
					JGetString("ErrorNodeName::GDBVarTreeParser"), JString::empty,
					JGetString("ErrorNodeValue::GDBVarTreeParser"));
		child->SetValid(false);

		itsGDBErrorFlag = false;
		}
}
