/******************************************************************************
 GLBuiltinFitDescription.h

	Interface for the GLBuiltinFitDescription class

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GLBuiltinFitDescription
#define _H_GLBuiltinFitDescription

// Superclass Header
#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

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
