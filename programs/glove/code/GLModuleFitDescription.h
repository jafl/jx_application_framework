/******************************************************************************
 GLModuleFitDescription.h

	Interface for the GLModuleFitDescription class

	Copyright © 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GLModuleFitDescription
#define _H_GLModuleFitDescription

// Superclass Header
#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <GLFitDescription.h>

class GLDLFitModule;

class GLModuleFitDescription : public GLFitDescription
{
public:

	GLModuleFitDescription(GLDLFitModule* fit);
	virtual ~GLModuleFitDescription();

	GLDLFitModule*		GetFitModule();

protected:

private:

	GLDLFitModule*		itsModule;

private:

	// not allowed

	GLModuleFitDescription(const GLModuleFitDescription& source);
	const GLModuleFitDescription& operator=(const GLModuleFitDescription& source);

};

/******************************************************************************
 GetFitModule (public)

 ******************************************************************************/

inline GLDLFitModule*
GLModuleFitDescription::GetFitModule()
{
	return itsModule;
}

#endif
