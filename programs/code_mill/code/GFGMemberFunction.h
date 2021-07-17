/******************************************************************************
 GFGMemberFunction.h

	Interface for the GFGMemberFunction class

	Copyright (C) 2002 by Glenn W. Bach.
	
 *****************************************************************************/

#ifndef _H_GFGMemberFunction
#define _H_GFGMemberFunction

#include <JPtrArray-JString.h>

class GFGMemberFunction
{
public:

	GFGMemberFunction();
	virtual ~GFGMemberFunction();

	const JString&	GetFnName() const;
	void			SetFnName(const JString& str);

	bool		IsProtected() const;
	void			ShouldBeProtected(const bool protect);

	bool		IsRequired() const;
	void			ShouldBeRequired(const bool require);

	bool		IsUsed() const;
	void			ShouldBeUsed(const bool use);

	const JString&	GetInterface() const;
	void			SetInterface(const JString& str);

	const JString&	GetReturnType() const;
	void			SetReturnType(const JString& type);

	bool		IsConst() const;
	void			ShouldBeConst(const bool isConst);

	JSize			GetArgCount() const;
	const JString&	GetArg(const JIndex index) const;
	const JString&	GetArgString() const;
	void			AddArg(const JString& arg);

	static JListT::CompareResult
		CompareFunction(GFGMemberFunction* const & f1, GFGMemberFunction* const & f2);

protected:

private:

	JString		itsFnName;
	bool	itsIsProtected;
	bool	itsIsRequired;	// if base class has pure virtual function
	bool	itsIsUsed;
	JString		itsInterface;
	JString		itsReturnType;
	bool	itsIsConst;
	
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
	const JString& name
	)
{
	itsFnName = name;
}

/******************************************************************************
 IsProtected (public)

 ******************************************************************************/

inline bool
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
	const bool protect
	)
{
	itsIsProtected	= protect;
}

/******************************************************************************
 IsRequired (public)

 ******************************************************************************/

inline bool
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
	const bool require
	)
{
	itsIsRequired	= require;
}

/******************************************************************************
 IsUsed (public)

 ******************************************************************************/

inline bool
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
	const bool use
	)
{
	itsIsUsed = use;
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
	const JString& type
	)
{
	itsReturnType = type;
}

/******************************************************************************
 IsConst

 ******************************************************************************/

inline bool
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
	const bool isConst
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
	if (itsArgs == nullptr)
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
