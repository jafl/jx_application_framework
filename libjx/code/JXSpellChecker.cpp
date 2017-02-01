/******************************************************************************
 JXSpellChecker.cc

	BASE CLASS = JXSharedPrefObject

	Copyright (C) 1997 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <JXSpellChecker.h>
#include <JXSpellCheckerDialog.h>
#include <JXSharedPrefsManager.h>
#include <JXWindowDirector.h>
#include <JXWindow.h>
#include <JXTEBase.h>
#include <jXGlobals.h>
#include <JProcess.h>
#include <JOutPipeStream.h>
#include <JStringIterator.h>
#include <JStringMatch.h>
#include <jStreamUtil.h>
#include <jProcessUtil.h>
#include <jDirUtil.h>
#include <sstream>
#include <jAssert.h>

// setup information

const JFileVersion kCurrentPrefsVersion	= 0;

static const JXSharedPrefObject::VersionInfo kVersList[] =
{
	JXSharedPrefObject::VersionInfo(0, JXSharedPrefsManager::kSpellCheckerV0)
};

const JSize kVersCount = sizeof(kVersList) / sizeof(JXSharedPrefObject::VersionInfo);

/******************************************************************************
 Constructor

 *****************************************************************************/

JXSpellChecker::JXSpellChecker()
	:
	JXSharedPrefObject(kCurrentPrefsVersion,
					   JXSharedPrefsManager::kLatestSpellCheckerVersionID,
					   kVersList, kVersCount),
	itsProcess(NULL),
	itsInFD(ACE_INVALID_HANDLE),
	itsOutPipe(NULL),
	itsReportNoErrorsFlag(kJTrue)
{
	const JString cwd = JGetCurrentDirectory();
	JString home;
	if (JGetHomeDirectory(&home))
		{
		JChangeDirectory(home);
		}

	int toFD, fromFD;
	JError err = JProcess::Create(&itsProcess, JString("aspell -a", 0, kJFalse),
								  kJCreatePipe, &toFD,
								  kJCreatePipe, &fromFD,
								  kJTossOutput, NULL);
	if (!err.OK())
		{
		err = JProcess::Create(&itsProcess, JString("ispell -a", 0, kJFalse),
							   kJCreatePipe, &toFD,
							   kJCreatePipe, &fromFD,
							   kJTossOutput, NULL);
		}

	if (err.OK())
		{
		itsInFD = fromFD;
		assert(itsInFD != ACE_INVALID_HANDLE);

		itsOutPipe = jnew JOutPipeStream(toFD, kJTrue);
		assert(itsOutPipe != NULL);

		JIgnoreUntil(itsInFD, '\n');
		}

	JChangeDirectory(cwd);

	JXSharedPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXSpellChecker::~JXSpellChecker()
{
	jdelete itsOutPipe;
	close(itsInFD);
	jdelete itsProcess;
}

/******************************************************************************
 Check

 *****************************************************************************/

void
JXSpellChecker::Check
	(
	JXTEBase* editor
	)
{
	if (itsProcess == NULL)
		{
		return;
		}

	JIndexRange range(1, editor->GetTextLength());

	JXSpellCheckerDialog* dlog = jnew JXSpellCheckerDialog(this, editor, range);
	assert( dlog != NULL );
	dlog->Check();
}

/******************************************************************************
 CheckSelection

 *****************************************************************************/

void
JXSpellChecker::CheckSelection
	(
	JXTEBase* editor
	)
{
	JCharacterRange range;
	if (itsProcess == NULL ||
		!editor->GetSelection(&range))
		{
		return;
		}

	JXSpellCheckerDialog* dlog = jnew JXSpellCheckerDialog(this, editor, range);
	assert( dlog != NULL );
	dlog->Check();
}

/******************************************************************************
 CheckWord

 *****************************************************************************/

JBoolean
JXSpellChecker::CheckWord
	(
	const JString&		word,
	JPtrArray<JString>*	suggestionList,
	JBoolean*			goodFirstSuggestion
	)
{
	suggestionList->DeleteAll();

	JStringIterator iter1(word);
	JUtf8Character c;
	while (iter1.Next(&c))
		{
		if (!c.IsAlpha())	// ispell splits on non-alpha characters
			{
			return kJTrue;
			}
		}

	word.Print(*itsOutPipe);
	*itsOutPipe << std::endl;

	JString test = JReadUntil(itsInFD, '\n');
	if (test.IsEmpty())
		{
		test = JReadUntil(itsInFD, '\n');
		}
	if (test.IsEmpty())
		{
		return kJTrue;
		}

	c = test.GetFirstCharacter();
	if (c == '*' || c == '+' || c == '-')
		{
		return kJTrue;
		}

	JStringIterator iter2(&test);
	if (iter2.Next(":"))
		{
		iter2.RemoveAllPrev();
		test.TrimWhitespace();	// invalidates iter2
		test += ",";			// so every suggestion is caught in loop

		JStringIterator iter3(&test);
		iter3.BeginMatch();
		while (iter3.Next(","))
			{
			JString* str = jnew JString(iter3.FinishMatch().GetString());
			assert(str != NULL);
			str->TrimWhitespace();
			suggestionList->Append(str);
			iter3.BeginMatch();
			}
		}

	*goodFirstSuggestion = JI2B(c == '&');
	return kJFalse;
}

/******************************************************************************
 Learn

 ******************************************************************************/

void
JXSpellChecker::Learn
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
JXSpellChecker::LearnCaps
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
JXSpellChecker::Ignore
	(
	const JString& word
	)
{
	*itsOutPipe << '@';
	word.Print(*itsOutPipe);
	*itsOutPipe << std::endl;
}

/******************************************************************************
 ShouldReportNoErrors

 ******************************************************************************/

void
JXSpellChecker::ShouldReportNoErrors
	(
	const JBoolean report
	)
{
	itsReportNoErrorsFlag = report;
	JXSharedPrefObject::WritePrefs();
}

/******************************************************************************
 SaveWindowSize

 ******************************************************************************/

void
JXSpellChecker::SaveWindowSize
	(
	const JRect bounds
	)
{
	itsDefaultWindowSize.x = bounds.width();
	itsDefaultWindowSize.y = bounds.height();
	JXSharedPrefObject::WritePrefs();
}

/******************************************************************************
 ReadPrefs (virtual)

 ******************************************************************************/

void
JXSpellChecker::ReadPrefs
	(
	std::istream& input
	)
{
	JFileVersion vers;
	input >> vers;
	if (vers > kCurrentPrefsVersion)
		{
		return;
		}

	input >> itsReportNoErrorsFlag;
	input >> itsDefaultWindowSize;
}

/******************************************************************************
 WritePrefs (virtual)

 ******************************************************************************/

void
JXSpellChecker::WritePrefs
	(
	std::ostream&			output,
	const JFileVersion	vers
	)
	const
{
	if (vers == 0)
		{
		output << vers;
		output << ' ' << itsReportNoErrorsFlag;
		output << ' ' << itsDefaultWindowSize;
		}
	else
		{
		assert( 0 );
		}
}
