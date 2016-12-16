/******************************************************************************
 GFGLink.cpp

	BASE CLASS = public JBroadcaster

	Copyright (C) 2002 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#include <GFGLink.h>
#include <GFGMemberFunction.h>
#include <GFGClass.h>
#include "gfgGlobals.h"

#include <JOutPipeStream.h>
#include <JProcess.h>
#include <JRegex.h>
#include <JTextEditor.h>

#include <jDirUtil.h>
#include <jStreamUtil.h>
#include <jTime.h>
#include <jAssert.h>

const JCharacter* kUnableToStartCTagsID	= "UnableToStartCTags::GFGLink";
const JCharacter* kCTagsCmd				= "ctags --filter=yes --filter-terminator=\\\f -n --fields=kzafimns --format=2 --c-types=p ";
const JCharacter kDelimiter 			= '\f';
const JCharacter* GFGLink::kFileParsed	= "kFileParsed::GFGLink";

static const JRegex memberLine("([^[:space:]]+)[[:space:]]+[^[:space:]]+[[:space:]]+[^[:space:]]+[[:space:]]+[^[:space:]]+[[:space:]]+line:([[:digit:]]+)[[:space:]]+class:([^[:space:]]+)[[:space:]]+access:([^[:space:]]+)");
// index
// 1	= whole line
// 2	= class name
// 3	= line number
// 4	= base class
// 5	= access

/******************************************************************************
 Constructor

 *****************************************************************************/

GFGLink::GFGLink()
	:
	itsCTagsProcess(NULL),
	itsClassList(NULL)
{
	StartCTags();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GFGLink::~GFGLink()
{
	StopCTags();
}

/******************************************************************************
 ParseClass (public)

 ******************************************************************************/

void
GFGLink::ParseClass
	(
	GFGClass* 		  list,
	const JCharacter* filename, 
	const JCharacter* classname
	)
{
	JBoolean ok	= kJTrue;
	if (itsCTagsProcess == NULL)
		{
		ok = StartCTags();
		}

	if (ok)
		{
		itsClassList	= list;
		itsCurrentClass	= classname;
		itsCurrentFile	= filename;

		JConvertToAbsolutePath(filename, "", &itsCurrentFile);

		itsCurrentFile.Print(*itsOutputLink);
		*itsOutputLink << std::endl;
		itsOutputLink->flush();

		JBoolean found = kJFalse;
		JString result = JReadUntil(itsInputFD, kDelimiter, &found);

		if (found)
			{
			JIndex findex;
			while (	result.LocateSubstring("\n", &findex) &&
					findex > 1)
				{
				JString line	= result.GetSubstring(1, findex - 1);
				result.RemoveSubstring(1, findex);
				ParseLine(line);
				}
			Broadcast(FileParsed());
			}
		}
}

/******************************************************************************
 StopCTags (private)

 *****************************************************************************/

void
GFGLink::StopCTags()
{
	jdelete itsCTagsProcess;
	itsCTagsProcess = NULL;

	jdelete itsOutputLink;
	itsOutputLink	= NULL;
	
	close(itsInputFD);
	itsInputFD = ACE_INVALID_HANDLE;
}

/******************************************************************************
 ParseLine (private)

 ******************************************************************************/

void
GFGLink::ParseLine
	(
	const JString& data
	)
{
	// we only care about virtual functions
	JBoolean required	= kJFalse;
	if (data.Contains("implementation:pure virtual"))
		{
		required	= kJTrue;
		}
	else if (!data.Contains("implementation:virtual"))
		{
		return;
		}

	JArray<JIndexRange> subList;
	if (memberLine.Match(data, &subList))
		{
		JIndexRange sRange	= subList.GetElement(2);
		JString name		= data.GetSubstring(sRange);
		if (name.BeginsWith("~"))
			{
			return;
			}

		GFGMemberFunction* fn	= jnew GFGMemberFunction();
		assert(fn != NULL);

		fn->SetFnName(name);
		fn->ShouldBeRequired(required);

		sRange	= subList.GetElement(3);
		JIndex line;
		JString lineStr		= data.GetSubstring(sRange);
		lineStr.ConvertToUInt(&line);

		sRange	= subList.GetElement(4);
		JString base		= data.GetSubstring(sRange);
		if (base != itsCurrentClass)
			{
			jdelete fn;
			return;
			}

		sRange	= subList.GetElement(5);
		JString access		= data.GetSubstring(sRange);
		if (access == "protected")
			{
			fn->ShouldBeProtected(kJTrue);
			}

		ParseInterface(fn, line);

		// Override entry from base class so function will only be
		// marked as pure virtual if nobody implemented it.

		JBoolean found;
		const JIndex i =
			itsClassList->SearchSorted1(fn, JOrderedSetT::kAnyMatch, &found);
		if (found)
			{
			itsClassList->DeleteElement(i);
			}
		itsClassList->InsertAtIndex(i, fn);
		}
}

/******************************************************************************
 ParseInterface (private)

 ******************************************************************************/

void
GFGLink::ParseInterface
	(
	GFGMemberFunction* 	fn,
	const JIndex 		line
	)
{
	std::ifstream is(itsCurrentFile);
	if (!is.good())
		{
		return;
		}

	// skip to the function's line
	JString str;
	for (JIndex i = 1; i < line; i++)
		{
		str	= JReadLine(is);
		}

	JSize p1	= JTellg(is);

	is >> std::ws;
	str	= JReadUntilws(is);
	if (str != "virtual")
		{
		return;
		}

	is >> std::ws;

	// return type
	str	= JReadUntilws(is);
	if (str	== "const")
		{
		str	+= " " + JReadUntilws(is);
		}
	fn->SetReturnType(str);

	is >> std::ws;

	// this should be the function name
	str	= JReadUntil(is, '(');
	str.TrimWhitespace();
	if (str != fn->GetFnName())
		{
		return;
		}

	// get arguments
	JCharacter delim	= ',';
	while (delim == ',')
		{
		JBoolean ok	= JReadUntil(is, 2, ",)", &str, &delim);
		if (!ok)
			{
			return;
			}
		JIndex findex;
		if (str.LocateSubstring("//", &findex))
			{
			JIndex eindex;
			if (str.LocateSubstring("\n", &eindex) &&
				eindex >= findex)
				{
				str.RemoveSubstring(findex, eindex);
				}
			}
		str.TrimWhitespace();
		if (!str.IsEmpty())
			{
			fn->AddArg(str);
			}		
		}

	is >> std::ws;

	// is it const;
	str	= JReadUntil(is, ';');
	if (str.Contains("const"))
		{
		fn->ShouldBeConst(kJTrue);
		}

	JSize p2	= JTellg(is);
	JSeekg(is, p1);

	str	= JRead(is, p2 - p1);
	fn->SetInterface(str);
}


/******************************************************************************
 StartCTags (private)

	We cannot send anything to CTags until it has successfully started.

 *****************************************************************************/

JBoolean
GFGLink::StartCTags()
{
	assert( itsCTagsProcess == NULL );

	JString cmd = kCTagsCmd;

	int toFD, fromFD;
	const JError err = JProcess::Create(&itsCTagsProcess, cmd,
										kJCreatePipe, &toFD,
										kJCreatePipe, &fromFD,
										kJAttachToFromFD, NULL);
	if (err.OK())
		{
		itsOutputLink = jnew JOutPipeStream(toFD, kJTrue);
		assert( itsOutputLink != NULL );

		itsInputFD = fromFD;
		assert( itsInputFD != ACE_INVALID_HANDLE );

		ListenTo(itsCTagsProcess);

		return kJTrue;
		}
	else
		{
		(JGetStringManager())->ReportError(kUnableToStartCTagsID, err);
		return kJFalse;
		}
}
