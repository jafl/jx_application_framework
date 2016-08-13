/******************************************************************************
 GDBVarTreeParser.cpp

	Copyright © 1998 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include "GDBVarTreeParser.h"
#include "CMVarNode.h"
#include "cmGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GDBVarTreeParser::GDBVarTreeParser
	(
	const JCharacter* text
	)
	:
	itsCurrentNode(NULL),
	itsIsPointerFlag(kJFalse)
{
	itsScanner = new GDBVarTreeScanner(text);
	assert(itsScanner != NULL);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GDBVarTreeParser::~GDBVarTreeParser()
{
	delete itsScanner;
	delete itsCurrentNode;
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
	int token = itsScanner->yylex(lvalp);
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
		itsGDBErrorFlag = kJTrue;

		token          = P_GROUP_CLOSE;
		lvalp->pString = new JString("}");
		assert( lvalp->pString != NULL );

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
	cerr << "yyerror() called: " << message << endl;

	std::ostringstream log;
	log << "yyerror() called: " << message;
	(CMGetLink())->Log(log);

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
	const JString name = "[" + JString(list->GetElementCount(), JString::kBase10) + "]";
	node->SetName(name);
	list->Append(node);
}

/******************************************************************************
 AppendAsArrayElement (private)

 *****************************************************************************/

void
GDBVarTreeParser::AppendAsArrayElement
	(
	const JCharacter*			groupLabel,
	const JPtrArray<CMVarNode>& data,
	JPtrArray<CMVarNode>*		list
	)
	const
{
	const JString name = "[" + JString(list->GetElementCount(), JString::kBase10) + "]";

	CMVarNode* node = (CMGetLink())->CreateVarNode(NULL, name, NULL, groupLabel);
	assert( node != NULL );
	list->Append(node);

	const JSize count = data.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		node->Append(const_cast<CMVarNode*>(data.NthElement(i)));
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
		child->SetValid(kJFalse);

		child = (CMGetLink())->CreateVarNode(itsCurrentNode,
					JGetString("ErrorNodeName::GDBVarTreeParser"), NULL,
					JGetString("ErrorNodeValue::GDBVarTreeParser"));
		child->SetValid(kJFalse);

		itsGDBErrorFlag = kJFalse;
		}
}
