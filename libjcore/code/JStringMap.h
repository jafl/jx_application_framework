#ifndef _H_JStringMap
#define _H_JStringMap

/******************************************************************************
 JStringMap.h

	Interface for the JStringMap template.

	Copyright (C) 1997 by Dustin Laurence.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "jx-af/jcore/JHashTable.h"
#include "jx-af/jcore/JStringMapCursor.h"
#include "jx-af/jcore/JStrValue.h"
#include "jx-af/jcore/JString.h"
#include "jx-af/jcore/JPtrArray.h"	// need definition of CleanUpAction, SetElementAction

struct JStringMapT
{
	enum SetType
	{
		kIfNew, // Only set if the element already exists
		kIfOld, // Only set if the element does not already exist
		kAlways // Always set the element
	};
};

template <class V>
class JStringMap : public JHashTable< JStrValue<V> >
{
	friend class JStringMapCursor<V>;

public:

	JStringMap(const JSize lgSize = kJDefaultLgMinTableSize);
	~JStringMap() override;

	bool Contains(const JString& key) const;
	bool GetElement(const JString& key, V* value) const;

	bool SetElement(const JString& key, const V& value,
					const JStringMapT::SetType type = JStringMapT::kAlways);
	bool SetNewElement(const JString& key, const V& value);
	bool SetOldElement(const JString& key, const V& value);
	bool SetContains(const JString& key, const V& value);

	bool RemoveElement(const JString& key);
	void RemoveAll();

	bool Contains(const JUtf8Byte* key) const;
	bool GetElement(const JUtf8Byte* key, V* value) const;

	bool SetElement(const JUtf8Byte* key, const V& value,
					const JStringMapT::SetType type = JStringMapT::kAlways);
	bool SetNewElement(const JUtf8Byte* key, const V& value);
	bool SetOldElement(const JUtf8Byte* key, const V& value);
	bool SetContains(const JUtf8Byte* key, const V& value);

	bool RemoveElement(const JUtf8Byte* key);

protected:

	bool     SetElement(const JString& key, const V& value,
						const JPtrArrayT::SetElementAction action,
						const JStringMapT::SetType type,
						bool* existed);
	virtual void PrepareForSet(const JPtrArrayT::SetElementAction action);

	bool RemoveElement(const JString& key,
					   const JPtrArrayT::SetElementAction action);
	void RemoveAll(const JPtrArrayT::SetElementAction action);

private:

	void JStringMapX();

	static bool Compare(const JStrValue<V>& lhs, const JStrValue<V>& rhs);

	static JHashValue Hash(const JStrValue<V>& value);
};


/******************************************************************************
 Contains

 *****************************************************************************/

template <class V>
inline bool
JStringMap<V>::Contains
	(
	const JUtf8Byte* key
	)
	const
{
	return Contains(JString(key, JString::kNoCopy));
}

/******************************************************************************
 GetElement

 *****************************************************************************/

template <class V>
inline bool
JStringMap<V>::GetElement
	(
	const JUtf8Byte* key,
	V*               value
	)
	const
{
	return GetElement(JString(key, JString::kNoCopy), value);
}

/******************************************************************************
 Set...Element

	The basic idea of setting a map element is simple.  However, it is
	common to write code which will set a value depending on whether it
	already exists.  Several forms of Set...Element support this notion
	more efficiently than calling Contains() and then SetElement().  In
	particular, the hash calculation and internal search will only happen
	once.

 *****************************************************************************/

/******************************************************************************
 SetElement

	The element is set if it already existed and 'type' is kIfOld or kAlways
	(the default) or if it did not exist and 'type' is kIfNew or kAlways.

	Returns true if the element was set, false otherwise.

 *****************************************************************************/

template <class V>
inline bool
JStringMap<V>::SetElement
	(
	const JUtf8Byte*           key,
	const V&                   value,
	const JStringMapT::SetType type // = kAlways
	)
{
	bool existed;
	return SetElement(JString(key, JString::kNoCopy), value, JPtrArrayT::kForget, type, &existed);
}

template <class V>
inline bool
JStringMap<V>::SetElement
	(
	const JString&             key,
	const V&                   value,
	const JStringMapT::SetType type // = kAlways
	)
{
	bool existed;
	return SetElement(key, value, JPtrArrayT::kForget, type, &existed);
}

/******************************************************************************
 SetNewElement

	Only sets the value if it does not already exist.  Returns true if the
	value was set.

 *****************************************************************************/

template <class V>
inline bool
JStringMap<V>::SetNewElement
	(
	const JUtf8Byte* key,
	const V&         value
	)
{
	return SetElement(JString(key, JString::kNoCopy), value, JStringMapT::kIfNew);
}

template <class V>
inline bool
JStringMap<V>::SetNewElement
	(
	const JString& key,
	const V&       value
	)
{
	return SetElement(key, value, JStringMapT::kIfNew);
}

/******************************************************************************
 SetOldElement

	Only sets the value if it already exists.  Returns true if the value was
	set.

 *****************************************************************************/

template <class V>
inline bool
JStringMap<V>::SetOldElement
	(
	const JUtf8Byte* key,
	const V&         value
	)
{
	return SetElement(JString(key, JString::kNoCopy), value, JStringMapT::kIfOld);
}

template <class V>
inline bool
JStringMap<V>::SetOldElement
	(
	const JString& key,
	const V&       value
	)
{
	return SetElement(key, value, JStringMapT::kIfOld);
}

/******************************************************************************
 SetContains

	A sometimes useful hybrid form; always sets the element, and returns true
	if the element already existed, false otherwise.

 *****************************************************************************/

template <class V>
inline bool
JStringMap<V>::SetContains
	(
	const JUtf8Byte* key,
	const V&         value
	)
{
	bool existed;
	SetElement(JString(key, JString::kNoCopy), value, JPtrArrayT::kForget, JStringMapT::kAlways, &existed);
	return existed;
}

template <class V>
inline bool
JStringMap<V>::SetContains
	(
	const JString& key,
	const V&       value
	)
{
	bool existed;
	SetElement(key, value, JPtrArrayT::kForget, JStringMapT::kAlways, &existed);
	return existed;
}

/******************************************************************************
 RemoveElement

 *****************************************************************************/

template <class V>
inline bool
JStringMap<V>::RemoveElement
	(
	const JUtf8Byte* key
	)
{
	return RemoveElement(JString(key, JString::kNoCopy), JPtrArrayT::kForget);
}

template <class V>
inline bool
JStringMap<V>::RemoveElement
	(
	const JString& key
	)
{
	return RemoveElement(key, JPtrArrayT::kForget);
}

/******************************************************************************
 RemoveAll

 *****************************************************************************/

template <class V>
inline void
JStringMap<V>::RemoveAll()
{
	RemoveAll(JPtrArrayT::kForget);
}

#include "JStringMap.tmpl"
#include "JStringMapCursor.tmpl"

#endif
