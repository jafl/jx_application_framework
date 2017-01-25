/******************************************************************************
 GLDLFitModule.cpp

	BASE CLASS = <NONE>

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <GLDLFitModule.h>

#include <JVector.h>

#include <ace/DLL.h>
#include <jAssert.h>

typedef const JCharacter** (GetParmsFn)();
typedef JSize (GetParmCountFn)();
typedef const JCharacter* (GetNameFn)();

const JCharacter* kInitValName		= "GetStartValues";
const JCharacter* kFNName			= "FunctionN";
const JCharacter* kFNPrimedName		= "FunctionNPrimed";
const JCharacter* kGetParmsName		= "GetParms";
const JCharacter* kParmCountName	= "GetParmCount";
const JCharacter* kFormName			= "GetFunctionForm";
const JCharacter* kFitName			= "GetFitName";

/******************************************************************************
 Create & Constructor

 *****************************************************************************/

JBoolean
GLDLFitModule::Create
	(
	const JCharacter*	moduleName,
	GLDLFitModule**		fit
	)
{
	ACE_DLL* module = jnew ACE_DLL(moduleName);
	assert(module != NULL);

	JBoolean ok	= kJTrue;

	EvalFn*	fn	= (EvalFn*)module->symbol(kFNName);
	if (fn == NULL)
		{
		ok	= kJFalse;
		}
	EvalFn*	fnprimed	= (EvalFn*)module->symbol(kFNPrimedName);
	GetParmsFn* pf		= (GetParmsFn*)module->symbol(kGetParmsName);
	if (pf == NULL)
		{
		ok	= kJFalse;
		}
	GetParmCountFn* pc	= (GetParmCountFn*)module->symbol(kParmCountName);
	if (pc == NULL)
		{
		ok	= kJFalse;
		}
	GetNameFn* ff		= (GetNameFn*)module->symbol(kFormName);
	if (ff == NULL)
		{
		ok	= kJFalse;
		}
	GetNameFn* fname	= (GetNameFn*)module->symbol(kFitName);
	if (fname == NULL)
		{
		ok	= kJFalse;
		}
	InitialValFn* ifn	= (InitialValFn*)module->symbol(kInitValName);
	if (!ok)
		{
		jdelete module;
		return kJFalse;
		}
	*fit	= jnew GLDLFitModule(module, fn, fnprimed, ifn, pc(), pf(), ff(), fname());
	assert(*fit != NULL);
	return kJTrue;
}

GLDLFitModule::GLDLFitModule
	(
	ACE_DLL* 			module, 
	EvalFn* 			function, 
	EvalFn* 			fprimed,
	InitialValFn* 		initFn,
	const JSize 		count, 
	const JCharacter** 	parms, 
	const JCharacter* 	form, 
	const JCharacter* 	name
	)
	:
	itsFunctionalForm(form),
	itsFitName(name),
	itsParmCount(count),
	itsFunction(function),
	itsFPrimed(fprimed),
	itsGetStartValFn(initFn),
	itsModule(module)
{
	itsParmNames	= jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsParmNames != NULL);
	for (JIndex i = 1; i <= count; i++)
		{
		JString* str	= jnew JString(parms[i - 1]);
		assert(str != NULL);
		itsParmNames->Append(str);
		}
	itsParameters	= jnew JVector(count);
	assert(itsParameters != NULL);
}


/******************************************************************************
 Destructor

 *****************************************************************************/

GLDLFitModule::~GLDLFitModule()
{
	itsParmNames->DeleteAll();
	jdelete itsParmNames;
	jdelete itsModule;
	jdelete itsParameters;
}

/******************************************************************************
 HasStartValues (public)

 ******************************************************************************/

JBoolean
GLDLFitModule::HasStartValues()
	const
{
	return JI2B(itsGetStartValFn != NULL);
}

/******************************************************************************
 GetStartValues (public)

	The arrays need to be pointers to allow for NULL.

 ******************************************************************************/

JBoolean
GLDLFitModule::GetStartValues
	(
	const JArray<JFloat>* x, 
	const JArray<JFloat>* y, 
	const JArray<JFloat>* xerr, 
	const JArray<JFloat>* yerr, 
	JVector* 			  p
	)
{
	if (itsGetStartValFn == NULL)
		{
		return kJFalse;
		}

	assert(x != NULL);
	assert(y != NULL);

	

//	itsGetStartValFn(x->GetElementCount(), 
	
	return kJTrue;
}


/******************************************************************************
 Function (public)

 ******************************************************************************/

JFloat
GLDLFitModule::Function
	(
	const JFloat 	x
	)
{
	return itsFunction(itsParameters->GetElements(), x);
}

/******************************************************************************
 HasFPrimed (public)

 ******************************************************************************/

JBoolean
GLDLFitModule::HasFPrimed()
	const
{
	return JI2B(itsFPrimed != NULL);
}

/******************************************************************************
 FPrimed (public)

 ******************************************************************************/

JFloat
GLDLFitModule::FPrimed
	(
	const JFloat 	x
	)
{
	assert(itsFPrimed != NULL);
	return itsFPrimed(itsParameters->GetElements(), x);
}

/******************************************************************************
 SetCurrentParameters (public)

 ******************************************************************************/

void
GLDLFitModule::SetCurrentParameters
	(
	const JVector& 	p
	)
{
	assert(p.GetDimensionCount() == itsParameters->GetDimensionCount());
	*itsParameters	= p;
}

/******************************************************************************
 GetParameterCount (public)

 ******************************************************************************/

JSize
GLDLFitModule::GetParameterCount()
	const
{
	return itsParmCount;
}

/******************************************************************************
 GetParameterName (public)

 ******************************************************************************/

const JString&
GLDLFitModule::GetParameterName
	(
	const JIndex index
	)
	const
{
	return *(itsParmNames->GetElement(index));
}

/******************************************************************************
 GetFunctionalForm (public)

 ******************************************************************************/

const JString&
GLDLFitModule::GetFunctionalForm()
	const
{
	return itsFunctionalForm;
}

/******************************************************************************
 GetFitName (public)

 ******************************************************************************/

const JString&
GLDLFitModule::GetFitName()
	const
{
	return itsFitName;
}
