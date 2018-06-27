/******************************************************************************
 JSpellChecker.h

	Copyright (C) 1997 by Glenn W. Bach.
	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#ifndef _H_JSpellChecker
#define _H_JSpellChecker

#include <JPtrArray-JString.h>

class JProcess;
class JOutPipeStream;

class JSpellChecker
{
public:

	JSpellChecker();

	virtual	~JSpellChecker();

	JBoolean	IsAvailable() const;

	JBoolean	CheckWord(const JString& word, JPtrArray<JString>* suggestionList,
						  JBoolean* goodFirstSuggestion);

	void	Learn(const JString& word);
	void	LearnCaps(const JString& word);
	void	Ignore(const JString& word);

private:

	JProcess*		itsProcess;
	int				itsInFD;
	JOutPipeStream*	itsOutPipe;

private:

	// not allowed

	JSpellChecker(const JSpellChecker& source);
	const JSpellChecker& operator=(const JSpellChecker& source);
};


/******************************************************************************
 IsAvailable

 ******************************************************************************/

inline JBoolean
JSpellChecker::IsAvailable()
	const
{
	return JI2B(itsProcess != nullptr);
}

#endif
