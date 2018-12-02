/******************************************************************************
 GLExprDirector.h

	Interface for the GLExprDirector class

	Copyright (C) 1997 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_GLExprDirector
#define _H_GLExprDirector

#include <JXDialogDirector.h>

class GLVarList;
class JXExprEditor;

class GLExprDirector : public JXDialogDirector
{
public:

	GLExprDirector(JXDirector* supervisor, const GLVarList* list, const JString& function);
	virtual ~GLExprDirector();

	JString GetString();

private:

	JXExprEditor*	itsEditor;

private:

	// not allowed

	GLExprDirector(const GLExprDirector& source);
	const GLExprDirector& operator=(const GLExprDirector& source);
};

#endif
