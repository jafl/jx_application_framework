/******************************************************************************
 GLFitDescription.cpp

	BASE CLASS = <NONE>

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GLFitDescription.h>

#include "GLPolyFitDescription.h"
#include "GLNonLinearFitDescription.h"

#include <jGlobals.h>
#include <jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GLFitDescription::GLFitDescription
	(
	const FitType	type,
	const JString&	form,
	const JString&	name
	)
	:
	GLFitBase(1),
	itsType(type),
	itsFnForm(form),
	itsFnName(name.IsEmpty() ? JGetString("DefaultLabel::GLGlobal") : name),
	itsRequiresStartValues(false),
	itsCanUseStartValues(true)
{
	itsVarList	= jnew GLVarList();
	assert(itsVarList != nullptr);

	itsVarList->AddVariable(JGetString("DefaultVarName::GLGlobal"), 0);
}

bool
GLFitDescription::Create
	(
	std::istream& 			is,
	GLFitDescription** 	fd
	)
{
	JFileVersion version;
	is >> version;
	if (version > kCurrentSetupVersion)
		{
		return false;
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
		JString* var = jnew JString();
		assert(var != nullptr);
		is >> *var;
		vars.Append(var);
		}
	
	if (type == kPolynomial)
		{
		GLPolyFitDescription* pfd	= jnew GLPolyFitDescription(is);
		assert(pfd != nullptr);
		*fd	= pfd;
		}
	else if (type == kNonLinear)
		{
		GLNonLinearFitDescription* nfd	= jnew GLNonLinearFitDescription(is);
		assert(nfd != nullptr);
		*fd	= nfd;
		for (JIndex i = 1; i <= count; i++)
			{
			(*fd)->itsVarList->AddVariable(*(vars.GetElement(i)), 0);
			}
		(*fd)->SetParameterCount(count);		
		}
	else
		{
		return false;
		}

	vars.DeleteAll();
	(*fd)->itsFnName	= name;
	(*fd)->itsFnForm	= form;
		
	return true;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLFitDescription::~GLFitDescription()
{
	jdelete itsVarList;
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
	const JString& form
	)
{
	itsFnForm = form;
}

/******************************************************************************
 SetFnName (protected)

 ******************************************************************************/

void
GLFitDescription::SetFnName
	(
	const JString& name
	)
{
	itsFnName = name;
}

/******************************************************************************
 DoesRequireStartValues (protected)

 ******************************************************************************/

void
GLFitDescription::DoesRequireStartValues
	(
	const bool require
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
	const bool use
	)
{
	itsCanUseStartValues	= use;
}

/******************************************************************************
 GetParameterName (virtual public)

 ******************************************************************************/

bool
GLFitDescription::GetParameterName
	(
	const JIndex 	index, 
	JString*		name
	)
	const
{
	*name	= itsVarList->GetVariableName(index + 1);
	return true;
}

/******************************************************************************
 GetParameter (virtual public)

 ******************************************************************************/

bool
GLFitDescription::GetParameter
	(
	const JIndex 	index, 
	JFloat*			value
	)
	const
{
	return false;
}

/******************************************************************************
 WriteSetup (virtual public)

 ******************************************************************************/

void
GLFitDescription::WriteSetup
	(
	std::ostream& os
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

JListT::CompareResult
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
		return JListT::kFirstLessSecond;
		}
	else if (f1->GetType() == kModule)
		{
		return JListT::kFirstGreaterSecond;
		}
	else if (f1->GetType() == kPolynomial)
		{
		if (f2->GetType() >= kBLinear)
			{
			return JListT::kFirstGreaterSecond;
			}
		else
			{
			return JListT::kFirstLessSecond;
			}
		}
	else
		{
		if (f2->GetType() == kModule)
			{
			return JListT::kFirstLessSecond;
			}
		else
			{
			return JListT::kFirstGreaterSecond;
			}
		}
	
}
