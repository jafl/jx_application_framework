/******************************************************************************
 GFGClass.cpp

	<Description>

	BASE CLASS = public JPtrArray<GFGMemberFunction>

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GFGClass.h>
#include "GFGLink.h"
#include "gfgGlobals.h"
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

GFGClass::GFGClass()
	:
	JPtrArray<GFGMemberFunction>(JPtrArrayT::kDeleteAll),
	itsBaseClasses(nullptr),
	itsBaseClassFiles(nullptr),
	itsAncestors(nullptr),
	itsAncestorFiles(nullptr)
{
	itsLink	= jnew GFGLink();
	assert(itsLink != nullptr);
	ListenTo(itsLink);

	SetCompareFunction(GFGMemberFunction::CompareFunction);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GFGClass::~GFGClass()
{
	jdelete itsBaseClasses;
	jdelete itsBaseClassFiles;
	jdelete itsAncestors;
	jdelete itsAncestorFiles;
}

/******************************************************************************
 GetBaseClassCount (public)

 ******************************************************************************/

JSize
GFGClass::GetBaseClassCount()
	const
{
	if (itsBaseClasses == nullptr)
		{
		return 0;
		}

	assert(itsBaseClasses->GetElementCount() == itsBaseClassFiles->GetElementCount());

	return itsBaseClasses->GetElementCount();
}

/******************************************************************************
 GetBaseClass (public)

 ******************************************************************************/

void
GFGClass::GetBaseClass
	(
	const JIndex 	index,
	JString* 		classname, 
	JString* 		filename
	)
	const
{
	assert(itsBaseClasses != nullptr);
	assert(itsBaseClasses->IndexValid(index));
	assert(itsBaseClassFiles != nullptr);
	assert(itsBaseClassFiles->IndexValid(index));
	
	*classname	= *(itsBaseClasses->GetElement(index));
	*filename	= *(itsBaseClassFiles->GetElement(index));
}

/******************************************************************************
 AddBaseClass (public)

 ******************************************************************************/

void
GFGClass::AddBaseClass
	(
	const JString& classname, 
	const JString& filename
	)
{
	if (itsBaseClasses == nullptr)
		{
		itsBaseClasses = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsBaseClasses != nullptr);
		}
	itsBaseClasses->Append(classname);

	if (itsBaseClassFiles == nullptr)
		{
		itsBaseClassFiles = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsBaseClassFiles != nullptr);
		}
	itsBaseClassFiles->Append(filename);
}

/******************************************************************************
 GetAncestorCount (public)

 ******************************************************************************/

JSize
GFGClass::GetAncestorCount()
	const
{
	if (itsAncestors == nullptr)
		{
		return 0;
		}

	assert(itsAncestors->GetElementCount() == itsAncestorFiles->GetElementCount());

	return itsAncestors->GetElementCount();
}

/******************************************************************************
 GetAncestor (public)

 ******************************************************************************/

void
GFGClass::GetAncestor
	(
	const JIndex 	index,
	JString* 		classname, 
	JString*		filename
	)
	const
{
	assert(itsAncestors != nullptr);
	assert(itsAncestors->IndexValid(index));
	assert(itsAncestorFiles != nullptr);
	assert(itsAncestorFiles->IndexValid(index));
	
	*classname	= *(itsAncestors->GetElement(index));
	*filename	= *(itsAncestorFiles->GetElement(index));
}

/******************************************************************************
 AddAncestor (public)

 ******************************************************************************/

void
GFGClass::AddAncestor
	(
	const JString& classname, 
	const JString& filename
	)
{
	if (itsAncestors == nullptr)
		{
		itsAncestors = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsAncestors != nullptr);
		}
	itsAncestors->Append(classname);

	if (itsAncestorFiles == nullptr)
		{
		itsAncestorFiles = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsAncestorFiles != nullptr);
		}
	itsAncestorFiles->Append(filename);
}

/******************************************************************************
 Populate (public)

 ******************************************************************************/

void
GFGClass::Populate()
{
	const JSize bcount = GetBaseClassCount();
	if (bcount == 0)
		{
		return;
		}

	JString classname, filename;

	// Parse these in reverse order so newer declarations override
	// older ones.

	const JSize count = GetAncestorCount();
	for (JIndex i=count; i>=1; i--)
		{
		GetAncestor(i, &classname, &filename);
		itsLink->ParseClass(this, filename, classname);
		}

	// Parse these last so their function declarations override
	// those of their ancestors.

	for (JIndex i = 1; i <= bcount; i++)
		{
		GetBaseClass(i, &classname, &filename);
		itsLink->ParseClass(this, filename, classname);
		}
}

/******************************************************************************
 WritePublic (public)

 ******************************************************************************/

void
GFGClass::WritePublic
	(
	std::ostream& 		os,
	const bool	interface
	)
{
	const JSize count	= GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GFGMemberFunction* fn	= GetElement(i);
		if (fn->IsUsed() &&
			!fn->IsProtected())
			{
			WriteFunction(os, fn, interface);
			}
		}
}

/******************************************************************************
 WriteProtected (public)

 ******************************************************************************/

void
GFGClass::WriteProtected
	(
	std::ostream& 		os,
	const bool	interface
	)
{
	const JSize count	= GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GFGMemberFunction* fn	= GetElement(i);
		if (fn->IsUsed() &&
			fn->IsProtected())
			{
			WriteFunction(os, fn, interface);
			}
		}
}

/******************************************************************************
 WriteFunction (private)

 ******************************************************************************/

void
GFGClass::WriteFunction
	(
	std::ostream& 			os,
	GFGMemberFunction*	fn,
	const bool		interface
	)
{
	if (interface)
		{
		fn->GetInterface().Print(os);
		os << std::endl;
		return;
		}

	JString access("public");
	if (fn->IsProtected())
		{
		access = "protected";
		}

	const JString s = GFGGetPrefsManager()->GetFunctionComment(fn->GetFnName(), access);
	s.Print(os);

	fn->GetReturnType().Print(os);
	os << std::endl;
	
	itsClassName.Print(os);
	os << "::";
	fn->GetFnName().Print(os);
	const JSize count	= fn->GetArgCount();
	if (count == 0)
		{
		os << "()" << std::endl;
		}
	else
		{
		os << "\n\t(\n";
		for (JIndex i = 1; i <= count; i++)
			{
			os << "\t";
			fn->GetArg(i).Print(os);
			if (i != count)
				{
				os << ",";
				}
			os << std::endl;
			}
		os << "\t)" << std::endl;
		}
	if (fn->IsConst())
		{
		os << "\tconst" << std::endl;
		}
	os << "{\n\n}" << std::endl << std::endl;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GFGClass::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsLink && message.Is(GFGLink::kFileParsed))
		{
/*		const JSize count	= GetElementCount();
		for (JIndex i = 1; i <= count; i++)
			{
			GFGMemberFunction* fn	= GetElement(i);
			std::cout << "#########################" << std::endl;
			std::cout << fn->GetFnName() << '\t' << fn->IsProtected() << '\t' << fn->IsRequired() << '\t' << fn->IsConst() << std::endl;
			std::cout << "-------------------------" << std::endl;
			std::cout << fn->GetInterface() << std::endl;
			std::cout << "-------------------------" << std::endl;
			const JSize acount	= fn->GetArgCount();
			for (JIndex i = 1; i <= acount; i++)
				{
				std::cout << fn->GetArg(i) << std::endl;
				}
			}
*/		
		}
	else
		{
		JPtrArray<GFGMemberFunction>::Receive(sender, message);
		}
}
