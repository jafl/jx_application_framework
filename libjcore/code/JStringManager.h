/******************************************************************************
 JStringManager.h

	Copyright (C) 2000 by John Lindal.  All rights reserved.

 *****************************************************************************/

#ifndef _H_JStringManager
#define _H_JStringManager

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
	JBoolean	MergeFile(std::istream& input, const JBoolean debug = kJFalse);
	void		WriteFile(std::ostream& output) const;

	const JString&	Get(const JCharacter* id) const;

	JString			Get(const JCharacter* id, const JCharacter* map[],
						const JSize size) const;
	JSubstitute*	GetReplaceEngine() const;
	void			Replace(JString* str, const JCharacter* map[],
							const JSize size) const;

	void	ReportError(const JCharacter* id, const JError& err) const;
	void	ReportError(const JCharacter* id, const JCharacter* message) const;

	static JBoolean	CanOverride(const JCharacter* id);

	static void	EnablePseudotranslation();

private:

	JSubstitute*	itsReplaceEngine;
	static JBoolean	thePseudotranslationFlag;

private:

	static void	Pseudotranslate(JString* s);

	// not allowed

	JStringManager(const JStringManager& source);
	const JStringManager& operator=(const JStringManager& source);
};


/******************************************************************************
 EnablePseudotranslation

	This must be called before initialization.

 *****************************************************************************/

inline void
JStringManager::EnablePseudotranslation()
{
	thePseudotranslationFlag = kJTrue;
}

#endif
