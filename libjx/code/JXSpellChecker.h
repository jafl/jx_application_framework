/******************************************************************************
 JXSpellChecker.h

	Copyright (C) 1997 by Glenn W. Bach.

 *****************************************************************************/

#ifndef _H_JXSpellChecker
#define _H_JXSpellChecker

#include <JXSharedPrefObject.h>
#include <JPtrArray-JString.h>
#include <JRect.h>

class JProcess;
class JOutPipeStream;
class JXTEBase;

class JXSpellChecker : public JXSharedPrefObject
{
public:

	JXSpellChecker();

	virtual	~JXSpellChecker();

	JBoolean	IsAvailable() const;

	JBoolean	WillReportNoErrors() const;
	void		ShouldReportNoErrors(const JBoolean report);

	void		Check(JXTEBase* te);
	void		CheckSelection(JXTEBase* te);
	JBoolean	CheckWord(const JString& word, JPtrArray<JString>* suggestionList,
						  JBoolean* goodFirstSuggestion);

	void	Learn(const JString& word);
	void	LearnCaps(const JString& word);
	void	Ignore(const JString& word);

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output, const JFileVersion vers) const;

	// called by JXSpellCheckerDialog

	const JPoint&	GetWindowSize() const;
	void			SaveWindowSize(const JRect bounds);

private:

	JProcess*		itsProcess;
	int				itsInFD;
	JOutPipeStream*	itsOutPipe;

	JBoolean		itsReportNoErrorsFlag;
	JPoint			itsDefaultWindowSize;

private:

	void		CleanUpOnFinished();

	// not allowed

	JXSpellChecker(const JXSpellChecker& source);
	const JXSpellChecker& operator=(const JXSpellChecker& source);
};


/******************************************************************************
 IsAvailable

 ******************************************************************************/

inline JBoolean
JXSpellChecker::IsAvailable()
	const
{
	return JI2B(itsProcess != NULL);
}

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
