/******************************************************************************
 GFGLink.cpp

	BASE CLASS = public JBroadcaster

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#include <GFGLink.h>
#include <GFGMemberFunction.h>
#include <GFGClass.h>
#include "gfgGlobals.h"

#include <JOutPipeStream.h>
#include <JProcess.h>
#include <JRegex.h>
#include <JStringIterator.h>
#include <JStringMatch.h>

#include <jDirUtil.h>
#include <jStreamUtil.h>
#include <jTime.h>
#include <jAssert.h>

const JString kCTagsCmd("ctags --filter=yes --filter-terminator=\a -n --fields=kzafimns --format=2 --c-types=p");
const JUtf8Byte kDelimiter = '\a';

// JBroadcaster messages

const JUtf8Byte* GFGLink::kFileParsed = "FileParsed::GFGLink";

/******************************************************************************
 Constructor

 *****************************************************************************/

GFGLink::GFGLink()
	:
	itsCTagsProcess(nullptr),
	itsClassList(nullptr)
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
	GFGClass*		list,
	const JString&	filename, 
	const JString&	classname
	)
{
	bool ok	= true;
	if (itsCTagsProcess == nullptr)
		{
		ok = StartCTags();
		}

	if (ok)
		{
		itsClassList	= list;
		itsCurrentClass	= classname;
		itsCurrentFile	= filename;

		JConvertToAbsolutePath(filename, JString::empty, &itsCurrentFile);

		itsCurrentFile.Print(*itsOutputLink);
		*itsOutputLink << std::endl;
		itsOutputLink->flush();

		bool found = false;
		JString result = JReadUntil(itsInputFD, kDelimiter, &found);

		if (found)
			{
			JPtrArray<JString> lines(JPtrArrayT::kDeleteAll);
			result.Split("\n", &lines);

			for (const auto* line : lines)
				{
				ParseLine(*line);
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
	itsCTagsProcess = nullptr;

	jdelete itsOutputLink;
	itsOutputLink	= nullptr;
	
	close(itsInputFD);
	itsInputFD = ACE_INVALID_HANDLE;
}

/******************************************************************************
 ParseLine (private)

 ******************************************************************************/

static const JRegex linePattern("([^[:space:]]+)[[:space:]]+[^[:space:]]+[[:space:]]+[^[:space:]]+[[:space:]]+[^[:space:]]+[[:space:]]+line:([[:digit:]]+)[[:space:]]+class:([^[:space:]]+)[[:space:]]+access:([^[:space:]]+)");
// index
// 0 = whole line
// 1 = class name
// 2 = line number
// 3 = base class
// 4 = access

void
GFGLink::ParseLine
	(
	const JString& data
	)
{
	// we only care about virtual functions

	bool required = false;
	if (data.Contains("implementation:pure virtual"))
		{
		required = true;
		}
	else if (!data.Contains("implementation:virtual"))
		{
		return;
		}

	JStringIterator iter(data);
	if (!iter.Next(linePattern))
		{
		return;
		}

	const JStringMatch& m = iter.GetLastMatch();
	if (m.GetSubstring(3) != itsCurrentClass)
		{
		return;
		}

	iter.Invalidate();

	JString name = m.GetSubstring(1);
	if (name.BeginsWith("~"))
		{
		return;
		}

	auto* fn = jnew GFGMemberFunction;
	assert( fn != nullptr );

	fn->SetFnName(name);
	fn->ShouldBeRequired(required);

	JIndex line;
	m.GetSubstring(2).ConvertToUInt(&line);

	if (m.GetSubstring(4) == "protected")
		{
		fn->ShouldBeProtected(true);
		}

	ParseInterface(fn, line);

	// Override entry from base class so function will only be
	// marked as pure virtual if nobody implemented it.

	bool found;
	const JIndex i = itsClassList->SearchSorted1(fn, JListT::kAnyMatch, &found);
	if (found)
		{
		itsClassList->DeleteElement(i);
		}
	itsClassList->InsertAtIndex(i, fn);
}

/******************************************************************************
 ParseInterface (private)

 ******************************************************************************/

static const JRegex commentPattern("//.*?\n");

void
GFGLink::ParseInterface
	(
	GFGMemberFunction* 	fn,
	const JIndex 		line
	)
{
	std::ifstream is(itsCurrentFile.GetBytes());
	if (!is.good())
		{
		return;
		}

	// skip to the function's line

	for (JIndex i=1; i<line; i++)
		{
		JIgnoreLine(is);
		}

	JSize p1 = JTellg(is);

	is >> std::ws;
	JString str = JReadUntilws(is);
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
	JUtf8Byte delim;
	do
		{
		if (!JReadUntil(is, 2, ",)", &str, &delim))
			{
			return;
			}

		JStringIterator iter(&str);
		while (iter.Next(commentPattern))
			{
			iter.RemoveLastMatch();
			}
		iter.Invalidate();

		str.TrimWhitespace();
		if (!str.IsEmpty())
			{
			fn->AddArg(str);
			}
		}
		while (delim == ',');

	is >> std::ws;

	// is it const;
	str = JReadUntil(is, ';');
	if (str.Contains("const"))
		{
		fn->ShouldBeConst(true);
		}

	JSize p2 = JTellg(is);
	JSeekg(is, p1);

	str = JRead(is, p2 - p1);
	fn->SetInterface(str);
}


/******************************************************************************
 StartCTags (private)

	We cannot send anything to CTags until it has successfully started.

 *****************************************************************************/

bool
GFGLink::StartCTags()
{
	assert( itsCTagsProcess == nullptr );

	int toFD, fromFD;
	const JError err = JProcess::Create(&itsCTagsProcess, kCTagsCmd,
										kJCreatePipe, &toFD,
										kJCreatePipe, &fromFD,
										kJAttachToFromFD, nullptr);
	if (err.OK())
		{
		itsOutputLink = jnew JOutPipeStream(toFD, true);
		assert( itsOutputLink != nullptr );

		itsInputFD = fromFD;
		assert( itsInputFD != ACE_INVALID_HANDLE );

		ListenTo(itsCTagsProcess);

		return true;
		}
	else
		{
		JGetStringManager()->ReportError("UnableToStartCTags::GFGLink", err);
		return false;
		}
}
