/******************************************************************************
 JSpellChecker.h

	Copyright (C) 1997 by Glenn W. Bach.
	Copyright (C) 2018 by John Lindal.

 *****************************************************************************/

#ifndef _H_JSpellChecker
#define _H_JSpellChecker

#include "jx-af/jcore/JPtrArray-JString.h"

class JProcess;
class JOutPipeStream;

class JSpellChecker
{
public:

	JSpellChecker();

	virtual ~JSpellChecker();

	bool	IsAvailable() const;

	bool	CheckWord(const JString& word, JPtrArray<JString>* suggestionList,
						  bool* goodFirstSuggestion);

	void	Learn(const JString& word);
	void	LearnCaps(const JString& word);
	void	Ignore(const JString& word);

private:

	JProcess*		itsProcess;
	int				itsInFD;
	JOutPipeStream*	itsOutPipe;

private:

	// not allowed

	JSpellChecker(const JSpellChecker&) = delete;
	JSpellChecker& operator=(const JSpellChecker&) = delete;
};


/******************************************************************************
 IsAvailable

 ******************************************************************************/

inline bool
JSpellChecker::IsAvailable()
	const
{
	return itsProcess != nullptr;
}

#endif
