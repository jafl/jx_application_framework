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

	void		Register(const JUtf8Byte* signature, const JUtf8Byte** defaultData);
	JBoolean	MergeFile(const JString& fileName, const JBoolean debug = kJFalse);
	void		MergeFile(istream& input, const JBoolean debug = kJFalse);
	void		WriteFile(ostream& output) const;

	const JString&	Get(const JUtf8Byte* id) const;

	JString			Get(const JUtf8Byte* id, const JUtf8Byte* map[],
						const JSize size) const;
	JSubstitute*	GetReplaceEngine() const;
	void			Replace(JString* str, const JUtf8Byte* map[],
							const JSize size) const;

	void	ReportError(const JUtf8Byte* id, const JError& err) const;
	void	ReportError(const JUtf8Byte* id, const JString& message) const;

	static JBoolean	CanOverride(const JString& id);

private:

	JSubstitute*	itsReplaceEngine;

private:

	// not allowed

	JStringManager(const JStringManager& source);
	const JStringManager& operator=(const JStringManager& source);
};

#endif
