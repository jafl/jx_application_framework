/******************************************************************************
 GLFitDescription.cpp

	BASE CLASS = <NONE>

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <glStdInc.h>
#include <GLFitDescription.h>

#include "GLPolyFitDescription.h"
#include "GLNonLinearFitDescription.h"

#include <JPtrArray-JString.h>

#include <jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GLFitDescription::GLFitDescription
	(
	const FitType 		type, 
	const JCharacter* 	form,
	const JCharacter* 	name
	)
	:
	JFitBase(1),
	itsType(type),
	itsFnForm(form),
	itsFnName(name),
	itsRequiresStartValues(kJFalse),
	itsCanUseStartValues(kJTrue)
{
	itsVarList	= new GVarList();
	assert(itsVarList != NULL);

	itsVarList->AddVariable("x", 0);
}

JBoolean
GLFitDescription::Create
	(
	istream& 			is,
	GLFitDescription** 	fd
	)
{
	JFileVersion version;
	is >> version;
	if (version > kCurrentSetupVersion)
		{
		return kJFalse;
		}
		
	int type;
	is >> type;
	JString form;
	is >> form;
	JString name;
	is >> name;
	JSize count;
	is >> count;
	JPtrArray<JString> vars(JPtrArrayT::kDeleteAll);
	for (JIndex i = 1; i <= count; i++)
		{
		JString* var = new JString();
		assert(var != NULL);
		is >> *var;
		vars.Append(var);
		}
	
	if (type == kPolynomial)
		{
		GLPolyFitDescription* pfd	= new GLPolyFitDescription(is);
		assert(pfd != NULL);
		*fd	= pfd;
		}
	else if (type == kNonLinear)
		{
		GLNonLinearFitDescription* nfd	= new GLNonLinearFitDescription(is);
		assert(nfd != NULL);
		*fd	= nfd;
		for (JIndex i = 1; i <= count; i++)
			{
			(*fd)->itsVarList->AddVariable(*(vars.NthElement(i)), 0);
			}
		(*fd)->SetParameterCount(count);		
		}
	else
		{
		return kJFalse;
		}

	vars.DeleteAll();
	(*fd)->itsFnName	= name;
	(*fd)->itsFnForm	= form;
		
	return kJTrue;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLFitDescription::~GLFitDescription()
{
	delete itsVarList;
}

/******************************************************************************
 SetType (public)

 ******************************************************************************/

void
GLFitDescription::SetType
	(
	const FitType type
	)
{
	itsType	= type;
}

/******************************************************************************
 SetFnForm (public)

 ******************************************************************************/

void
GLFitDescription::SetFitFunctionString
	(
	const JCharacter* form
	)
{
	itsFnForm	= form;
}

/******************************************************************************
 SetFnName (protected)

 ******************************************************************************/

void
GLFitDescription::SetFnName
	(
	const JCharacter* name
	)
{
	itsFnName	= name;
}

/******************************************************************************
 DoesRequireStartValues (protected)

 ******************************************************************************/

void
GLFitDescription::DoesRequireStartValues
	(
	const JBoolean require
	)
{
	itsRequiresStartValues	= require;
}

/******************************************************************************
 SetCanUseStartValues (protected)

 ******************************************************************************/

void
GLFitDescription::SetCanUseStartValues
	(
	const JBoolean use
	)
{
	itsCanUseStartValues	= use;
}

/******************************************************************************
 GetParameterName (virtual public)

 ******************************************************************************/

JBoolean
GLFitDescription::GetParameterName
	(
	const JIndex 	index, 
	JString*		name
	)
	const
{
	*name	= itsVarList->GetVariableName(index + 1);
	return kJTrue;
}

/******************************************************************************
 GetParameter (virtual public)

 ******************************************************************************/

JBoolean
GLFitDescription::GetParameter
	(
	const JIndex 	index, 
	JFloat*			value
	)
	const
{
	return kJFalse;
}

/******************************************************************************
 WriteSetup (virtual public)

 ******************************************************************************/

void
GLFitDescription::WriteSetup
	(
	ostream& os
	)
{
	os << ' ' << kCurrentSetupVersion << ' ';
	os << ' ' << (int)itsType << ' ';
	os << ' ' << itsFnForm << ' ';
	os << ' ' << itsFnName << ' ';

	const JSize count	= itsVarList->GetVariableCount() - 1;
	os << ' ' << count << ' ';
	for (JIndex i = 1; i <= count; i++)
		{
		os << ' ' << itsVarList->GetVariableName(i + 1) << ' ';
		}
}

/******************************************************************************
 CompareFontNames (static private)

	We sort by the first number, then by the second number: #x##

 ******************************************************************************/

JOrderedSetT::CompareResult
GLFitDescription::CompareFits
	(
	GLFitDescription* const & f1,
	GLFitDescription* const & f2
	)
{
	if (f1->GetType() == f2->GetType())
		{
		return JCompareStringsCaseInsensitive(const_cast<JString*>(&(f1->GetFnName())), const_cast<JString*>(&(f2->GetFnName())));
		}
	else if (f1->GetType() >= kBLinear)
		{
		return JOrderedSetT::kFirstLessSecond;
		}
	else if (f1->GetType() == kModule)
		{
		return JOrderedSetT::kFirstGreaterSecond;
		}
	else if (f1->GetType() == kPolynomial)
		{
		if (f2->GetType() >= kBLinear)
			{
			return JOrderedSetT::kFirstGreaterSecond;
			}
		else
			{
			return JOrderedSetT::kFirstLessSecond;
			}
		}
	else
		{
		if (f2->GetType() == kModule)
			{
			return JOrderedSetT::kFirstLessSecond;
			}
		else
			{
			return JOrderedSetT::kFirstGreaterSecond;
			}
		}
	
}
