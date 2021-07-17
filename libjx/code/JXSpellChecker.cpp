/******************************************************************************
 JXSpellChecker.cpp

	BASE CLASS = JSpellChecker, JXSharedPrefObject

	Copyright (C) 1997 by Glenn W. Bach.
	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#include "JXSpellChecker.h"
#include "JXSpellCheckerDialog.h"
#include "JXSharedPrefsManager.h"
#include "JXTEBase.h"
#include "jXGlobals.h"
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
	itsReportNoErrorsFlag(true)
{
	JXSharedPrefObject::ReadPrefs();
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXSpellChecker::~JXSpellChecker()
{
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
	if (!IsAvailable())
		{
		return;
		}

	JXSpellCheckerDialog* dlog =
		jnew JXSpellCheckerDialog(this, editor, editor->GetText()->SelectAll());
	assert( dlog != nullptr );
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
	if (!IsAvailable() ||
		!editor->GetSelection(&range))
		{
		return;
		}

	JXSpellCheckerDialog* dlog =
		jnew JXSpellCheckerDialog(this, editor, editor->GetText()->CharToTextRange(nullptr, range));
	assert( dlog != nullptr );
	dlog->Check();
}

/******************************************************************************
 ShouldReportNoErrors

 ******************************************************************************/

void
JXSpellChecker::ShouldReportNoErrors
	(
	const bool report
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
	const JRect& bounds
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

	input >> JBoolFromString(itsReportNoErrorsFlag);
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
		output << ' ' << JBoolToString(itsReportNoErrorsFlag);
		output << ' ' << itsDefaultWindowSize;
		}
	else
		{
		assert( 0 );
		}
}
