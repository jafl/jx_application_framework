#ifndef _H_JBroadcasterMessageTarget
#define _H_JBroadcasterMessageTarget

/******************************************************************************
 JBroadcasterMessageTarget.h

	Maps a derived class of JBroadcaster::Message to a set of objects.

	Copyright (C) 2023 by John Lindal.

 *****************************************************************************/

#include <boost/any.hpp>
#include <typeinfo>

class JBroadcaster;
template <class T> class JArray;

struct JBroadcasterMessageTuple
{
	JBroadcaster*	obj;
	boost::any*		f;

	JBroadcasterMessageTuple()
		:
		obj(nullptr), f(nullptr)
	{ };

	JBroadcasterMessageTuple
		(
		JBroadcaster*	_obj,
		boost::any*		_f
		)
		:
		obj(_obj), f(_f)
	{ };

	void	CleanOut();
};

class JBroadcasterMessageTarget
{
public:

	const std::type_info*				type;
	JArray<JBroadcasterMessageTuple>*	list;

	JBroadcasterMessageTarget()
		:
		type(nullptr), list(nullptr)
	{ };

	JBroadcasterMessageTarget
		(
		const std::type_info& t
		)
		:
		type(&t), list(nullptr)
	{ };

	JBroadcasterMessageTarget
		(
		const std::type_info&				t,
		JArray<JBroadcasterMessageTuple>*	_list
		)
		:
		type(&t),
		list(_list)
	{ };

	JHashValue
	Hash()
		const
	{
		return type->hash_code();
	};

	void	CleanOut();
};

inline bool
operator==
	(
	const JBroadcasterMessageTarget& lhs,
	const JBroadcasterMessageTarget& rhs
	)
{
	return *lhs.type == *rhs.type;
}

#endif
