#ifndef _H_GDBVarTreeParser
#define _H_GDBVarTreeParser

/******************************************************************************
 GDBVarTreeParser.h

	Copyright (C) 1998 by Glenn W. Bach.

 *****************************************************************************/

#include "GDBVarTreeScanner.h"

class CMVarNode;

class GDBVarTreeParser
{
public:

	GDBVarTreeParser(const JString& text);

	virtual	~GDBVarTreeParser();

	int		yyparse();
	void	ReportRecoverableError();

	CMVarNode*	GetRootNode() const;

private:

	GDBVarTreeScanner*	itsScanner;
	CMVarNode*			itsCurrentNode;
	JBoolean			itsIsPointerFlag;

	// compensate for gdb stopping on error

	JSize				itsGroupDepth;
	JBoolean			itsGDBErrorFlag;

private:

	void	AppendAsArrayElement(CMVarNode* node, JPtrArray<CMVarNode>* list) const;
	void	AppendAsArrayElement(const JString& groupLabel,
								 const JPtrArray<CMVarNode>& data,
								 JPtrArray<CMVarNode>* list) const;

	int yylex(YYSTYPE* lvalp);
	int yyerror(const char* message);

	// not allowed

	GDBVarTreeParser(const GDBVarTreeParser& source);
	const GDBVarTreeParser& operator=(const GDBVarTreeParser& source);
};


/******************************************************************************
 GetRootNode

 *****************************************************************************/

inline CMVarNode*
GDBVarTreeParser::GetRootNode()
	const
{
	return itsCurrentNode;
}

#endif
