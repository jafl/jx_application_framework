/******************************************************************************
 GLNonLinearFitDescription.cpp

	<Description>

	BASE CLASS = public GLFitDescription

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <GLNonLinearFitDescription.h>
#include <jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GLNonLinearFitDescription::GLNonLinearFitDescription
	(
	const JCharacter* 			name, 
	const JCharacter* 			function,
	const JCharacter* 			fPrimed,
	const JPtrArray<JString>&	vars
	)
	:
	GLFitDescription(GLFitDescription::kNonLinear, function, name),
	itsFunction(function),
	itsFPrimed(fPrimed)
{
	DoesRequireStartValues(kJTrue);

	JSize count		= vars.GetElementCount();
	JIndex offset	= 0;
	if (count > 1 && *(vars.NthElement(1)) == "x")
		{
		offset = 1;
		count--;
		}
	SetParameterCount(count);
	for (JIndex i = 1; i <= count; i++)
		{
		JString var	= *(vars.NthElement(i + offset));
		GetVarList()->AddVariable(var, 0);
		}
}

GLNonLinearFitDescription::GLNonLinearFitDescription
	(
	istream& is
	)
	:
	GLFitDescription(GLFitDescription::kNonLinear, "", "")
{
	JFileVersion version;
	is >> version;
	if (version > kCurrentSetupVersion)
		{
		return;
		}
	is >> itsFunction;
	is >> itsFPrimed;

	DoesRequireStartValues(kJTrue);
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
	ostream& os
	)
{
	GLFitDescription::WriteSetup(os);

	os << ' ' << kCurrentSetupVersion << ' ' ;
	os << ' ' << itsFunction << ' ';
	os << ' ' << itsFPrimed << ' ';
}
