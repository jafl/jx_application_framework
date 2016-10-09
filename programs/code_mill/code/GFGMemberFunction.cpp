/******************************************************************************
 GFGMemberFunction.cpp

	BASE CLASS = none

	Copyright (C) 2002 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <GFGMemberFunction.h>
#include <JPtrArray-JString.h>
#include <JRegex.h>
#include <jAssert.h>

static const JRegex pureVirtualPattern = "[[:space:]]*=[[:space:]]*0;$";

/******************************************************************************
 Constructor

 *****************************************************************************/

GFGMemberFunction::GFGMemberFunction()
	:	
	itsArgs(NULL)
{
	itsIsProtected	= kJFalse;
	itsIsRequired	= kJFalse;
	itsIsUsed		= kJFalse;
	itsIsConst		= kJFalse;
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GFGMemberFunction::~GFGMemberFunction()
{
	jdelete itsArgs;
}

/******************************************************************************
 SetInterface (public)

 ******************************************************************************/

void
GFGMemberFunction::SetInterface
	(
	const JCharacter* interface
	)
{
	itsInterface = interface;

	JIndexRange r;
	if (pureVirtualPattern.Match(itsInterface, &r))
		{
		itsInterface.ReplaceSubstring(r.first, r.last, ";");
		}
}

/******************************************************************************
 GetArg (public)

 ******************************************************************************/

const JString&
GFGMemberFunction::GetArg
	(
	const JIndex index
	)
	const
{
	assert(itsArgs != NULL);
	assert(itsArgs->IndexValid(index));
	return *(itsArgs->NthElement(index));
}

/******************************************************************************
 AddArg (public)

 ******************************************************************************/

void
GFGMemberFunction::AddArg
	(
	const JCharacter* arg
	)
{
	if (itsArgs == NULL)
		{
		itsArgs = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsArgs != NULL);
		}

	itsArgs->Append(arg);

	if (!itsArgString.IsEmpty())
		{
		itsArgString += ", ";
		}
	itsArgString += arg;
}

/******************************************************************************
 CompareFunction (static private)

 ******************************************************************************/

JOrderedSetT::CompareResult
GFGMemberFunction::CompareFunction
	(
	GFGMemberFunction* const & f1,
	GFGMemberFunction* const & f2
	)
{
	const JString& name1 = f1->GetFnName();
	const JString& name2 = f2->GetFnName();
	JOrderedSetT::CompareResult r =
		JCompareStringsCaseInsensitive(
			const_cast<JString*>(&name1), const_cast<JString*>(&name2));
	if (r != JOrderedSetT::kFirstEqualSecond)
		{
		return r;
		}

	if (f1->IsConst() != f2->IsConst())
		{
		if (f1->IsConst())
			{
			return JOrderedSetT::kFirstLessSecond;
			}
		return JOrderedSetT::kFirstGreaterSecond;
		}

	const JString& type1 = f1->GetReturnType();
	const JString& type2 = f2->GetReturnType();
	r = JCompareStringsCaseInsensitive(const_cast<JString*>(&type1),
									   const_cast<JString*>(&type2));
	if (r != JOrderedSetT::kFirstEqualSecond)
		{
		return r;
		}

	return JCompareStringsCaseInsensitive(const_cast<JString*>(&(f1->GetArgString())),
										  const_cast<JString*>(&(f2->GetArgString())));
}
