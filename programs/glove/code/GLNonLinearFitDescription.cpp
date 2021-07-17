/******************************************************************************
 GLNonLinearFitDescription.cpp

	<Description>

	BASE CLASS = public GLFitDescription

	Copyright (C) 1999 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GLNonLinearFitDescription.h>
#include <jGlobals.h>
#include <jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GLNonLinearFitDescription::GLNonLinearFitDescription
	(
	const JString& 				name, 
	const JString& 				function,
	const JString& 				fPrimed,
	const JPtrArray<JString>&	vars
	)
	:
	GLFitDescription(GLFitDescription::kNonLinear, function, name),
	itsFunction(function),
	itsFPrimed(fPrimed)
{
	DoesRequireStartValues(true);

	JSize count		= vars.GetElementCount();
	JIndex offset	= 0;
	if (count > 1 && *(vars.GetElement(1)) == JGetString("DefaultVarName::GLGlobal"))
		{
		offset = 1;
		count--;
		}
	SetParameterCount(count);
	for (JIndex i = 1; i <= count; i++)
		{
		JString var	= *(vars.GetElement(i + offset));
		GetVarList()->AddVariable(var, 0);
		}
}

GLNonLinearFitDescription::GLNonLinearFitDescription
	(
	std::istream& is
	)
	:
	GLFitDescription(GLFitDescription::kNonLinear, JString::empty, JString::empty)
{
	JFileVersion version;
	is >> version;
	if (version > kCurrentSetupVersion)
		{
		return;
		}
	is >> itsFunction;
	is >> itsFPrimed;

	DoesRequireStartValues(true);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLNonLinearFitDescription::~GLNonLinearFitDescription()
{
}

/******************************************************************************
 WriteSetup (virtual public)

 ******************************************************************************/

void
GLNonLinearFitDescription::WriteSetup
	(
	std::ostream& os
	)
{
	GLFitDescription::WriteSetup(os);

	os << ' ' << kCurrentSetupVersion << ' ' ;
	os << ' ' << itsFunction << ' ';
	os << ' ' << itsFPrimed << ' ';
}
