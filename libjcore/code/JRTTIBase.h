/******************************************************************************
 JRTTIBase.h

	Base class for all errors

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JRTTIBase
#define _H_JRTTIBase

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

class JRTTIBase
{
public:

	virtual	~JRTTIBase();

	JBoolean	Is(const JCharacter* type) const;

	const JCharacter*
	GetType() const
	{
		return itsType;
	};

protected:

	JRTTIBase(const JCharacter* type)
		:
		itsType(type)
	{ };

private:

	const JCharacter* itsType;
};

/******************************************************************************
 Comparison operators

 ******************************************************************************/

inline int
operator==
	(
	const JRTTIBase& obj1,
	const JRTTIBase& obj2
	)
{
	return obj1.Is(obj2.GetType());
}

inline int
operator==
	(
	const JRTTIBase&	obj,
	const JCharacter*	type
	)
{
	return obj.Is(type);
}

inline int
operator==
	(
	const JCharacter*	type,
	const JRTTIBase&	obj
	)
{
	return obj.Is(type);
}

inline int
operator!=
	(
	const JRTTIBase& obj1,
	const JRTTIBase& obj2
	)
{
	return ! obj1.Is(obj2.GetType());
}

inline int
operator!=
	(
	const JRTTIBase&	obj,
	const JCharacter*	type
	)
{
	return ! obj.Is(type);
}

inline int
operator!=
	(
	const JCharacter*	type,
	const JRTTIBase&	obj
	)
{
	return ! obj.Is(type);
}

#endif
