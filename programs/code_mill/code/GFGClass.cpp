/******************************************************************************
 GFGClass.cpp

	<Description>

	BASE CLASS = public JPtrArray<GFGMemberFunction>

	Copyright © 2002 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <gfgStdInc.h>
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
	itsBaseClasses(NULL),
	itsBaseClassFiles(NULL),
	itsAncestors(NULL),
	itsAncestorFiles(NULL)
{
	itsLink	= new GFGLink();
	assert(itsLink != NULL);
	ListenTo(itsLink);

	SetCompareFunction(GFGMemberFunction::CompareFunction);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GFGClass::~GFGClass()
{
	delete itsBaseClasses;
	delete itsBaseClassFiles;
	delete itsAncestors;
	delete itsAncestorFiles;
}

/******************************************************************************
 GetBaseClassCount (public)

 ******************************************************************************/

JSize
GFGClass::GetBaseClassCount()
	const
{
	if (itsBaseClasses == NULL)
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
	assert(itsBaseClasses != NULL);
	assert(itsBaseClasses->IndexValid(index));
	assert(itsBaseClassFiles != NULL);
	assert(itsBaseClassFiles->IndexValid(index));
	
	*classname	= *(itsBaseClasses->NthElement(index));
	*filename	= *(itsBaseClassFiles->NthElement(index));
}

/******************************************************************************
 AddBaseClass (public)

 ******************************************************************************/

void
GFGClass::AddBaseClass
	(
	const JCharacter* classname, 
	const JCharacter* filename
	)
{
	JString* cname	= new JString(classname);
	assert(cname != NULL);
	JString* fname	= new JString(filename);
	assert(fname != NULL);
	
	if (itsBaseClasses == NULL)
		{
		itsBaseClasses		= new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsBaseClasses != NULL);
		}
	if (itsBaseClassFiles == NULL)
		{
		itsBaseClassFiles	= new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsBaseClassFiles != NULL);
		}
	itsBaseClasses->Append(cname);
	itsBaseClassFiles->Append(fname);
}

/******************************************************************************
 GetAncestorCount (public)

 ******************************************************************************/

JSize
GFGClass::GetAncestorCount()
	const
{
	if (itsAncestors == NULL)
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
	assert(itsAncestors != NULL);
	assert(itsAncestors->IndexValid(index));
	assert(itsAncestorFiles != NULL);
	assert(itsAncestorFiles->IndexValid(index));
	
	*classname	= *(itsAncestors->NthElement(index));
	*filename	= *(itsAncestorFiles->NthElement(index));
}

/******************************************************************************
 AddAncestor (public)

 ******************************************************************************/

void
GFGClass::AddAncestor
	(
	const JCharacter* classname, 
	const JCharacter* filename
	)
{
	JString* cname	= new JString(classname);
	assert(cname != NULL);
	JString* fname	= new JString(filename);
	assert(fname != NULL);
	
	if (itsAncestors == NULL)
		{
		itsAncestors		= new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsAncestors != NULL);
		}
	if (itsAncestorFiles == NULL)
		{
		itsAncestorFiles	= new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
		assert(itsAncestorFiles != NULL);
		}
	itsAncestors->Append(cname);
	itsAncestorFiles->Append(fname);
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
	ostream& 		os,
	const JBoolean	interface
	)
{
	const JSize count	= GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GFGMemberFunction* fn	= NthElement(i);
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
	ostream& 		os,
	const JBoolean	interface
	)
{
	const JSize count	= GetElementCount();
	for (JIndex i = 1; i <= count; i++)
		{
		GFGMemberFunction* fn	= NthElement(i);
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
	ostream& 			os,
	GFGMemberFunction*	fn,
	const JBoolean		interface
	)
{
	if (interface)
		{
		fn->GetInterface().Print(os);
		os << endl;
		return;
		}

	JString access	= "public";
	if (fn->IsProtected())
		{
		access	= "protected";
		}

	JString s	= GFGGetPrefsManager()->GetFunctionComment(fn->GetFnName(), access);
	s.Print(os);

	fn->GetReturnType().Print(os);
	os << endl;
	
	itsClassName.Print(os);
	os << "::";
	fn->GetFnName().Print(os);
	const JSize count	= fn->GetArgCount();
	if (count == 0)
		{
		os << "()" << endl;
		}
	else
		{
		os << "\n\t(\n";
		for (JIndex i = 1; i <= count; i++)
			{
			JString arg	= fn->GetArg(i);
			os << "\t";
			arg.Print(os);
			if (i != count)
				{
				os << ",";
				}
			os << endl;
			}
		os << "\t)" << endl;
		}
	if (fn->IsConst())
		{
		os << "\tconst" << endl;
		}
	os << "{\n\n}" << endl << endl;
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
			GFGMemberFunction* fn	= NthElement(i);
			cout << "#########################" << endl;
			cout << fn->GetFnName() << '\t' << fn->IsProtected() << '\t' << fn->IsRequired() << '\t' << fn->IsConst() << endl;
			cout << "-------------------------" << endl;
			cout << fn->GetInterface() << endl;
			cout << "-------------------------" << endl;
			const JSize acount	= fn->GetArgCount();
			for (JIndex i = 1; i <= acount; i++)
				{
				cout << fn->GetArg(i) << endl;
				}
			}
*/		
		}
	else
		{
		JPtrArray<GFGMemberFunction>::Receive(sender, message);
		}
}

#include <JArray.tmpls>
#define JTemplateType GFGMemberFunction
#include <JPtrArray.tmpls>
