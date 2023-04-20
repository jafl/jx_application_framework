/******************************************************************************
 JRTTIBase.h

	Base class for all errors

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JRTTIBase
#define _H_JRTTIBase

#include "jTypes.h"

class JRTTIBase
{
public:

	virtual ~JRTTIBase();

	bool	Is(const JUtf8Byte* type) const;

	const JUtf8Byte*
	GetType() const
	{
		return itsType;
	};

protected:

	JRTTIBase(const JUtf8Byte* type)
		:
		itsType(type)
	{ };

private:

	const JUtf8Byte* itsType;
};

/******************************************************************************
 Comparison operators

 ******************************************************************************/

inline bool
operator==
	(
	const JRTTIBase& obj1,
	const JRTTIBase& obj2
	)
{
	return obj1.Is(obj2.GetType());
}

inline bool
operator==
	(
	const JRTTIBase&	obj,
	const JUtf8Byte*	type
	)
{
	return obj.Is(type);
}

inline bool
operator==
	(
	const JUtf8Byte*	type,
	const JRTTIBase&	obj
	)
{
	return obj.Is(type);
}

#endif
