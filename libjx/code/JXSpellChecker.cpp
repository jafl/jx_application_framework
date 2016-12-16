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
	JError err = JProcess::Create(&itsProcess, "aspell -a",
								  kJCreatePipe, &toFD,
								  kJCreatePipe, &fromFD,
								  kJTossOutput, NULL);
	if (!err.OK())
		{
		err = JProcess::Create(&itsProcess, "ispell -a",
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
	JIndexRange range;
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

	const JSize wordLength = word.GetLength();
	for (JIndex i=1; i<=wordLength; i++)
		{
		if (!JIsAlpha(word.GetCharacter(i)))	// ispell splits on non-alpha characters
			{
			return kJTrue;
			}
		}

	word.Print(*itsOutPipe);
	*itsOutPipe << std::endl;

//word.Print(std::cerr); std::cerr << std::endl;

	JString test = JReadUntil(itsInFD, '\n');
	if (test.IsEmpty())
		{
		test = JReadUntil(itsInFD, '\n');
		}
	if (test.IsEmpty())
		{
		return kJTrue;
		}

//test.Print(std::cerr); std::cerr << std::endl;

	JCharacter c = test.GetFirstCharacter();
	if (c == '*' || c == '+' || c == '-')
		{
		return kJTrue;
		}

	JIndex findex;
	if (test.LocateSubstring(":", &findex))
		{
		test.RemoveSubstring(1, findex);
		test.TrimWhitespace();
		test += ",";	// so every suggestion is caught in loop

		while (test.LocateSubstring(",", &findex))
			{
			JString* str = jnew JString(test.GetSubstring(1, findex - 1));
			assert(str != NULL);
			str->TrimWhitespace();
			suggestionList->Append(str);
			test.RemoveSubstring(1, findex);
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
	const JCharacter* word
	)
{
	*itsOutPipe << '&' << word << std::endl;
	*itsOutPipe << '#' << std::endl;
}

/******************************************************************************
 LearnCaps

 ******************************************************************************/

void
JXSpellChecker::LearnCaps
	(
	const JCharacter* word
	)
{
	*itsOutPipe << '*' << word << std::endl;
	*itsOutPipe << '#' << std::endl;
}

/******************************************************************************
 Ignore

 ******************************************************************************/

void
JXSpellChecker::Ignore
	(
	const JCharacter* word
	)
{
	*itsOutPipe << '@' << word << std::endl;
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
