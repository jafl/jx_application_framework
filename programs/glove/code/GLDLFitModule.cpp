/******************************************************************************
 GLDLFitModule.cpp

	BASE CLASS = <NONE>

	Copyright (C) 2000 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GLDLFitModule.h>

#include <JVector.h>

#include <ace/DLL.h>
#include <jAssert.h>

typedef const JUtf8Byte** (GetParmsFn)();
typedef JSize (GetParmCountFn)();
typedef const JUtf8Byte* (GetNameFn)();

const JUtf8Byte* kInitValName	= "GetStartValues";
const JUtf8Byte* kFNName		= "FunctionN";
const JUtf8Byte* kFNPrimedName	= "FunctionNPrimed";
const JUtf8Byte* kGetParmsName	= "GetParms";
const JUtf8Byte* kParmCountName	= "GetParmCount";
const JUtf8Byte* kFormName		= "GetFunctionForm";
const JUtf8Byte* kFitName		= "GetFitName";

/******************************************************************************
 Create & Constructor

 *****************************************************************************/

bool
GLDLFitModule::Create
	(
	const JString&		moduleName,
	GLDLFitModule**		fit
	)
{
	ACE_DLL* module = jnew ACE_DLL(moduleName.GetBytes());
	assert(module != nullptr);

	bool ok	= true;

	EvalFn*	fn	= (EvalFn*)module->symbol(kFNName);
	if (fn == nullptr)
		{
		ok	= false;
		}
	EvalFn*	fnprimed	= (EvalFn*)module->symbol(kFNPrimedName);
	GetParmsFn* pf		= (GetParmsFn*)module->symbol(kGetParmsName);
	if (pf == nullptr)
		{
		ok	= false;
		}
	GetParmCountFn* pc	= (GetParmCountFn*)module->symbol(kParmCountName);
	if (pc == nullptr)
		{
		ok	= false;
		}
	GetNameFn* ff		= (GetNameFn*)module->symbol(kFormName);
	if (ff == nullptr)
		{
		ok	= false;
		}
	GetNameFn* fname	= (GetNameFn*)module->symbol(kFitName);
	if (fname == nullptr)
		{
		ok	= false;
		}
	InitialValFn* ifn	= (InitialValFn*)module->symbol(kInitValName);
	if (!ok)
		{
		jdelete module;
		return false;
		}
	*fit	= jnew GLDLFitModule(module, fn, fnprimed, ifn, pc(), pf(), ff(), fname());
	assert(*fit != nullptr);
	return true;
}

GLDLFitModule::GLDLFitModule
	(
	ACE_DLL* 			module, 
	EvalFn* 			function, 
	EvalFn* 			fprimed,
	InitialValFn* 		initFn,
	const JSize 		count, 
	const JUtf8Byte** 	parms, 
	const JUtf8Byte* 	form, 
	const JUtf8Byte* 	name
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
	assert(itsParmNames != nullptr);
	for (JIndex i = 1; i <= count; i++)
		{
		JString* str	= jnew JString(parms[i - 1]);
		assert(str != nullptr);
		itsParmNames->Append(str);
		}
	itsParameters	= jnew JVector(count);
	assert(itsParameters != nullptr);
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

bool
GLDLFitModule::HasStartValues()
	const
{
	return itsGetStartValFn != nullptr;
}

/******************************************************************************
 GetStartValues (public)

	The arrays need to be pointers to allow for nullptr.

 ******************************************************************************/

bool
GLDLFitModule::GetStartValues
	(
	const JArray<JFloat>* x, 
	const JArray<JFloat>* y, 
	const JArray<JFloat>* xerr, 
	const JArray<JFloat>* yerr, 
	JVector* 			  p
	)
{
	if (itsGetStartValFn == nullptr)
		{
		return false;
		}

	assert(x != nullptr);
	assert(y != nullptr);

	

//	itsGetStartValFn(x->GetElementCount(), 
	
	return true;
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

bool
GLDLFitModule::HasFPrimed()
	const
{
	return itsFPrimed != nullptr;
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
	assert(itsFPrimed != nullptr);
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
