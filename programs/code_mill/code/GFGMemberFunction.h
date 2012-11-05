/******************************************************************************
 GFGMemberFunction.h

	Interface for the GFGMemberFunction class

	Copyright © 2002 by Glenn W. Bach.  All rights reserved.
	
 *****************************************************************************/

#ifndef _H_GFGMemberFunction
#define _H_GFGMemberFunction

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray-JString.h>

class GFGMemberFunction
{
public:

	GFGMemberFunction();
	virtual ~GFGMemberFunction();

	const JString&	GetFnName() const;
	void			SetFnName(const JCharacter* str);

	JBoolean		IsProtected() const;
	void			ShouldBeProtected(const JBoolean protect);

	JBoolean		IsRequired() const;
	void			ShouldBeRequired(const JBoolean require);

	JBoolean		IsUsed() const;
	void			ShouldBeUsed(const JBoolean use);

	const JString&	GetInterface() const;
	void			SetInterface(const JCharacter* str);

	const JString&	GetReturnType() const;
	void			SetReturnType(const JCharacter* type);

	JBoolean		IsConst() const;
	void			ShouldBeConst(const JBoolean isConst);

	JSize			GetArgCount() const;
	const JString&	GetArg(const JIndex index) const;
	const JString&	GetArgString() const;
	void			AddArg(const JCharacter* arg);

	static JOrderedSetT::CompareResult
		CompareFunction(GFGMemberFunction* const & f1, GFGMemberFunction* const & f2);

protected:

private:

	JString		itsFnName;
	JBoolean	itsIsProtected;
	JBoolean	itsIsRequired;	// if base class has pure virtual function
	JBoolean	itsIsUsed;
	JString		itsInterface;
	JString		itsReturnType;
	JBoolean	itsIsConst;
	
	JPtrArray<JString>*	itsArgs;
	JString				itsArgString;
	
private:

	// not allowed

	GFGMemberFunction(const GFGMemberFunction& source);
	const GFGMemberFunction& operator=(const GFGMemberFunction& source);

};


/******************************************************************************
 GetFnName (public)

 ******************************************************************************/

inline const JString&
GFGMemberFunction::GetFnName()
	const
{
	return itsFnName;
}

/******************************************************************************
 SetFnName (public)

 ******************************************************************************/

inline void
GFGMemberFunction::SetFnName
	(
	const JCharacter* name
	)
{
	itsFnName = name;
}

/******************************************************************************
 IsProtected (public)

 ******************************************************************************/

inline JBoolean
GFGMemberFunction::IsProtected()
	const
{
	return itsIsProtected;
}

/******************************************************************************
 ShouldBeProtected (public)

 ******************************************************************************/

inline void
GFGMemberFunction::ShouldBeProtected
	(
	const JBoolean protect
	)
{
	itsIsProtected	= protect;
}

/******************************************************************************
 IsRequired (public)

 ******************************************************************************/

inline JBoolean
GFGMemberFunction::IsRequired()
	const
{
	return itsIsRequired;
}

/******************************************************************************
 ShouldBeRequired (public)

 ******************************************************************************/

inline void
GFGMemberFunction::ShouldBeRequired
	(
	const JBoolean require
	)
{
	itsIsRequired	= require;
}

/******************************************************************************
 IsUsed (public)

 ******************************************************************************/

inline JBoolean
GFGMemberFunction::IsUsed()
	const
{
	return itsIsUsed;
}

/******************************************************************************
 ShouldBeUsed (public)

 ******************************************************************************/

inline void
GFGMemberFunction::ShouldBeUsed
	(
	const JBoolean use
	)
{
	itsIsUsed	= use;
}

/******************************************************************************
 GetInterface (public)

 ******************************************************************************/

inline const JString&
GFGMemberFunction::GetInterface()
	const
{
	return itsInterface;
}

/******************************************************************************
 GetReturnType

 ******************************************************************************/

inline const JString&
GFGMemberFunction::GetReturnType()
	const
{
	return itsReturnType;
}

/******************************************************************************
 SetReturnType

 ******************************************************************************/

inline void
GFGMemberFunction::SetReturnType
	(
	const JCharacter* type
	)
{
	itsReturnType	= type;
}

/******************************************************************************
 IsConst

 ******************************************************************************/

inline JBoolean
GFGMemberFunction::IsConst()
	const
{
	return itsIsConst;
}

/******************************************************************************
 ShouldBeConst

 ******************************************************************************/

inline void
GFGMemberFunction::ShouldBeConst
	(
	const JBoolean isConst
	)
{
	itsIsConst = isConst;
}

/******************************************************************************
 GetArgCount (public)

 ******************************************************************************/

inline JSize
GFGMemberFunction::GetArgCount()
	const
{
	if (itsArgs == NULL)
		{
		return 0;
		}

	return itsArgs->GetElementCount();
}

/******************************************************************************
 GetArgString (public)

 ******************************************************************************/

inline const JString&
GFGMemberFunction::GetArgString()
	const
{
	return itsArgString;
}

#endif
