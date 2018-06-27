/******************************************************************************
 JXSpellChecker.h

	Copyright (C) 1997 by Glenn W. Bach.
	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#ifndef _H_JXSpellChecker
#define _H_JXSpellChecker

#include <JSpellChecker.h>
#include "JXSharedPrefObject.h"
#include <JRect.h>

class JXTEBase;

class JXSpellChecker : public JSpellChecker, JXSharedPrefObject
{
public:

	JXSpellChecker();

	virtual	~JXSpellChecker();

	JBoolean	WillReportNoErrors() const;
	void		ShouldReportNoErrors(const JBoolean report);

	void		Check(JXTEBase* te);
	void		CheckSelection(JXTEBase* te);

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output, const JFileVersion vers) const;

	// called by JXSpellCheckerDialog

	const JPoint&	GetWindowSize() const;
	void			SaveWindowSize(const JRect& bounds);

private:

	JBoolean		itsReportNoErrorsFlag;
	JPoint			itsDefaultWindowSize;

private:

	// not allowed

	JXSpellChecker(const JXSpellChecker& source);
	const JXSpellChecker& operator=(const JXSpellChecker& source);
};


/******************************************************************************
 WillReportNoErrors

 ******************************************************************************/

inline JBoolean
JXSpellChecker::WillReportNoErrors()
	const
{
	return itsReportNoErrorsFlag;
}

/******************************************************************************
 GetWindowSize

 ******************************************************************************/

inline const JPoint&
JXSpellChecker::GetWindowSize()
	const
{
	return itsDefaultWindowSize;
}

#endif
