/******************************************************************************
 JStringManager.h

	Copyright (C) 2000 by John Lindal.

 *****************************************************************************/

#ifndef _H_JStringManager
#define _H_JStringManager

#include "jx-af/jcore/JStringPtrMap.h"

class JError;
class JString;
class JSubstitute;

class JStringManager : public JStringPtrMap<JString>
{
public:

	enum
	{
		kASCIIFormat = 0,
		kUTF8Format  = 1
	};

public:

	JStringManager();

	virtual ~JStringManager();

	void	Register(const JUtf8Byte* signature, const JUtf8Byte** defaultData);
	bool	MergeFile(const JString& fileName, const bool debug = false);
	bool	MergeFile(std::istream& input, const bool debug = false);
	void	WriteFile(std::ostream& output) const;

	const JString&	GetBCP47Locale() const;
	const JString&	Get(const JUtf8Byte* id) const;

	JString			Get(const JUtf8Byte* id, const JUtf8Byte* map[],
						const JSize size) const;
	JSubstitute*	GetReplaceEngine() const;
	void			Replace(JString* str, const JUtf8Byte* map[],
							const JSize size) const;

	void	ReportError(const JUtf8Byte* id, const JError& err) const;
	void	ReportError(const JUtf8Byte* id, const JString& message) const;

	static bool	CanOverride(const JString& id);

	static void	EnablePseudotranslation();

private:

	JString			itsBCP47Locale;
	JSubstitute*	itsReplaceEngine;
	static bool		thePseudotranslationFlag;

private:

	static void	Pseudotranslate(JString* s);
};


/******************************************************************************
 GetBCP47Locale

 *****************************************************************************/

inline const JString&
JStringManager::GetBCP47Locale()
	const
{
	return itsBCP47Locale;
}

/******************************************************************************
 EnablePseudotranslation

	This must be called before initialization.

 *****************************************************************************/

inline void
JStringManager::EnablePseudotranslation()
{
	thePseudotranslationFlag = true;
}

#endif
