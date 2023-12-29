/******************************************************************************
 JSpellChecker.cpp

	Copyright (C) 1997 by Glenn W. Bach.
	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#include "JSpellChecker.h"
#include "JProcess.h"
#include "JOutPipeStream.h"
#include "JStringIterator.h"
#include "JStringMatch.h"
#include "JRegex.h"
#include "jStreamUtil.h"
#include "jDirUtil.h"
#include "sstream"
#include "jAssert.h"

/******************************************************************************
 Constructor

 *****************************************************************************/

JSpellChecker::JSpellChecker()
	:
	itsProcess(nullptr),
	itsInFD(ACE_INVALID_HANDLE),
	itsOutPipe(nullptr)
{
	const JString cwd = JGetCurrentDirectory();
	JString home;
	if (JGetHomeDirectory(&home))
	{
		JChangeDirectory(home);
	}

	int toFD, fromFD;
	JError err = JProcess::Create(&itsProcess, "aspell -a",
								  kJCreatePipe, &toFD,
								  kJCreatePipe, &fromFD,
								  kJTossOutput, nullptr);
	if (!err.OK())
	{
		err = JProcess::Create(&itsProcess, "ispell -a",
							   kJCreatePipe, &toFD,
							   kJCreatePipe, &fromFD,
							   kJTossOutput, nullptr);
	}

	if (err.OK())
	{
		itsInFD = fromFD;
		assert(itsInFD != ACE_INVALID_HANDLE);

		itsOutPipe = jnew JOutPipeStream(toFD, true);

		JIgnoreUntil(itsInFD, '\n');
	}

	JChangeDirectory(cwd);
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JSpellChecker::~JSpellChecker()
{
	jdelete itsOutPipe;
	close(itsInFD);
	jdelete itsProcess;
}

/******************************************************************************
 CheckWord

 *****************************************************************************/

static const JRegex resultSplitPattern("\\s*,\\s*");

bool
JSpellChecker::CheckWord
	(
	const JString&		word,
	JPtrArray<JString>*	suggestionList,
	bool*				goodFirstSuggestion
	)
{
	suggestionList->DeleteAll();

	if (!word.IsAscii())	// ispell *may* split on non-english characters
	{
		return true;
	}

	JStringIterator iter1(word);
	JUtf8Character c;
	while (iter1.Next(&c))
	{
		if (!c.IsAlpha())	// ispell splits on non-alpha characters
		{
			return true;
		}
	}
	iter1.Invalidate();

	word.Print(*itsOutPipe);
	*itsOutPipe << std::endl;

	JString test = JReadUntil(itsInFD, '\n');
	if (test.IsEmpty())
	{
		return true;
	}

	JReadUntil(itsInFD, '\n');	// flush extra newline

	c = test.GetFirstCharacter();
	if (c == '*' || c == '+' || c == '-')
	{
		return true;
	}

	JStringIterator iter2(&test);
	if (iter2.Next(":"))
	{
		iter2.RemoveAllPrev();
		test.TrimWhitespace();	// invalidates iter2
		test.Split(resultSplitPattern, suggestionList);
	}

	*goodFirstSuggestion = c == '&';
	return false;
}

/******************************************************************************
 Learn

 ******************************************************************************/

void
JSpellChecker::Learn
	(
	const JString& word
	)
{
	*itsOutPipe << '&';
	word.Print(*itsOutPipe);
	*itsOutPipe << std::endl << '#' << std::endl;
}

/******************************************************************************
 LearnCaps

 ******************************************************************************/

void
JSpellChecker::LearnCaps
	(
	const JString& word
	)
{
	*itsOutPipe << '*';
	word.Print(*itsOutPipe);
	*itsOutPipe << std::endl << '#' << std::endl;
}

/******************************************************************************
 Ignore

 ******************************************************************************/

void
JSpellChecker::Ignore
	(
	const JString& word
	)
{
	*itsOutPipe << '@';
	word.Print(*itsOutPipe);
	*itsOutPipe << std::endl;
}
