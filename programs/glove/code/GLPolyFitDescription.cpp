/******************************************************************************
 GLPolyFitDescription.cpp

	<Description>

	BASE CLASS = public GLFitDescription

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <GLPolyFitDescription.h>

#include <jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GLPolyFitDescription::GLPolyFitDescription
	(
	const JCharacter* 		name,
	const JArray<JIndex>& 	powers
	)
	:
	GLFitDescription(kPolynomial, "", name)
{
	itsPowers	= new JArray<JIndex>(powers);
	assert(itsPowers != NULL);

	GLPolyFitDescriptionX();
}

GLPolyFitDescription::GLPolyFitDescription
	(
	istream& is
	)
	:
	GLFitDescription(kPolynomial, "", "")
{
	itsPowers	= new JArray<JIndex>;
	assert(itsPowers != NULL);

	JFileVersion version;
	is >> version;
	if (version > kCurrentSetupVersion)
		{
		return;
		}

	JSize count;
	is >> count;
	for (JIndex i = 1; i <= count; i++)
		{
		JIndex power;
		is >> power;
		itsPowers->AppendElement(power);
		}

	GLPolyFitDescriptionX();
}

void
GLPolyFitDescription::GLPolyFitDescriptionX()
{
	const JSize count	= itsPowers->GetElementCount();
	SetParameterCount(count);
	
	JString form;
	for (JIndex i = 1; i <= count; i++)
		{
		JString parm	= "a" + JString(i - 1, JString::kBase10);
		GetVarList()->AddVariable(parm, 0);
		JString xTerm;
		JIndex power	= itsPowers->GetElement(i);
		if (power > 0)
			{
			xTerm = " * x";
			}
		if (power > 1)
			{
			xTerm += "^" + JString(power);
			}
		form += parm + xTerm;
		if (i != count)
			{
			form += " + ";
			}
		}
	SetFitFunctionString(form);
}


/******************************************************************************
 Destructor

 *****************************************************************************/

GLPolyFitDescription::~GLPolyFitDescription()
{
	delete itsPowers;
}

/******************************************************************************
 GetPowers (public)

 ******************************************************************************/

void
GLPolyFitDescription::GetPowers
	(
	JArray<JIndex>* powers
	)
	const
{
	*powers	= *itsPowers;
}

/******************************************************************************
 WriteSetup (public)

 ******************************************************************************/

void
GLPolyFitDescription::WriteSetup
	(
	ostream& os
	)
{
	GLFitDescription::WriteSetup(os);
	
	os << ' ' << kCurrentSetupVersion << ' ' ;
	
	const JSize count	= itsPowers->GetElementCount();
	os << ' ' << count << ' ';
	for (JIndex i = 1; i <= count; i++)
		{
		os << ' ' << itsPowers->GetElement(i) << ' ';
		}
	
}
