/******************************************************************************
 GLBuiltinFitDescription.h

	Interface for the GLBuiltinFitDescription class

	Copyright (C) 1999 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_GLBuiltinFitDescription
#define _H_GLBuiltinFitDescription

// Superclass Header
#include <GLFitDescription.h>



class GLBuiltinFitDescription : public GLFitDescription
{
public:

	GLBuiltinFitDescription(const FitType type);
	virtual ~GLBuiltinFitDescription();

protected:

private:

private:

	// not allowed

	GLBuiltinFitDescription(const GLBuiltinFitDescription& source);
	const GLBuiltinFitDescription& operator=(const GLBuiltinFitDescription& source);

};
#endif
