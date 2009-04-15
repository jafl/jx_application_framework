/******************************************************************************
 ExprDirector.h

	Interface for the ExprDirector class

	Copyright © 1997 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_ExprDirector
#define _H_ExprDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXDialogDirector.h>

#include <JString.h>

class GVarList;
class JFunction;
class JXExprEditor;

class ExprDirector : public JXDialogDirector
{
public:

	ExprDirector(JXDirector* supervisor, const GVarList* list, const JCharacter* function);
	virtual ~ExprDirector();

	JString GetString();

private:

	JFunction* 		itsFunction;
	JString			itsFnString;
	JXExprEditor*	itsEditor;

private:

	// not allowed

	ExprDirector(const ExprDirector& source);
	const ExprDirector& operator=(const ExprDirector& source);
};

#endif
