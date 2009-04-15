/******************************************************************************
 JStringManager.h

	Copyright © 2000 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JStringManager
#define _H_JStringManager

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JStringPtrMap.h>

class JError;
class JString;
class JSubstitute;

class JStringManager : public JStringPtrMap<JString>
{
public:

	enum
	{
		kASCIIFormat = 0
	};

public:

	JStringManager();

	virtual ~JStringManager();

	void		Register(const JCharacter* signature, const JCharacter** defaultData);
	JBoolean	MergeFile(const JCharacter* fileName, const JBoolean debug = kJFalse);
	void		MergeFile(istream& input, const JBoolean debug = kJFalse);
	void		WriteFile(ostream& output) const;

	const JString&	Get(const JCharacter* id) const;

	JString			Get(const JCharacter* id, const JCharacter* map[],
						const JSize size) const;
	JSubstitute*	GetReplaceEngine() const;
	void			Replace(JString* str, const JCharacter* map[],
							const JSize size) const;

	void	ReportError(const JCharacter* id, const JError& err) const;
	void	ReportError(const JCharacter* id, const JCharacter* message) const;

	static JBoolean	CanOverride(const JCharacter* id);

private:

	JSubstitute*	itsReplaceEngine;

private:

	// not allowed

	JStringManager(const JStringManager& source);
	const JStringManager& operator=(const JStringManager& source);
};

#endif
