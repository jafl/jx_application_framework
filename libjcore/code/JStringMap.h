#ifndef _H_JStringMap
#define _H_JStringMap

/******************************************************************************
 JStringMap.h

	Interface for the JStringMap template.

	Copyright (C) 1997 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <JHashTable.h>
#include <JStringMapCursor.h>
#include <JStrValue.h>
#include <JPtrArray.h>	// need definition of CleanUpAction, SetElementAction

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

	JStringMap(const JBoolean copyKeys);
	JStringMap(const JSize lgSize = kJDefaultLgMinTableSize,
			   const JBoolean copyKeys = kJTrue);
	virtual ~JStringMap();

	JBoolean Contains(const JCharacter* const key) const;
	JBoolean GetElement(const JCharacter* const key, V* value) const;

	JBoolean SetElement(const JCharacter* const key, const V& value,
						const JStringMapT::SetType type = JStringMapT::kAlways);
	JBoolean SetNewElement(const JCharacter* const key, const V& value);
	JBoolean SetOldElement(const JCharacter* const key, const V& value);
	JBoolean SetContains(const JCharacter* const key, const V& value);

	JBoolean RemoveElement(const JCharacter* const key);
	void     RemoveAll();

	JBoolean KeysAreCopied() const { return itsCopyKeysFlag; };

protected:

	JBoolean     SetElement(const JCharacter* const key, const V& value,
							const JPtrArrayT::SetElementAction action,
							const JStringMapT::SetType type,
							JBoolean* existed);
	virtual void PrepareForSet(const JPtrArrayT::SetElementAction action);

	JBoolean RemoveElement(const JCharacter* const key,
						   const JPtrArrayT::SetElementAction action);
	void     RemoveAll(const JPtrArrayT::SetElementAction action);

private:

	const JBoolean itsCopyKeysFlag;

private:

	void JStringMapX();

	static JBoolean Compare(const JStrValue<V>& lhs, const JStrValue<V>& rhs);

	static JHashValue Hash(const JStrValue<V>& value);

	// not allowed

	JStringMap(const JStringMap& source);
	const JStringMap& operator=(const JStringMap& source);
};


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

	Returns kJTrue if the element was set, kJFalse otherwise.

 *****************************************************************************/

template <class V>
inline JBoolean
JStringMap<V>::SetElement
	(
	const JCharacter* const    key,
	const V&                   value,
	const JStringMapT::SetType type // = kAlways
	)
{
	JBoolean existed;
	return SetElement(key, value, JPtrArrayT::kForget, type, &existed);
}

/******************************************************************************
 SetNewElement

	Only sets the value if it does not already exist.  Returns kJTrue if the
	value was set.

 *****************************************************************************/

template <class V>
inline JBoolean
JStringMap<V>::SetNewElement
	(
	const JCharacter* const key,
	const V&                value
	)
{
	return SetElement(key, value, JStringMapT::kIfNew);
}

/******************************************************************************
 SetOldElement

	Only sets the value if it already exists.  Returns kJTrue if the value was
	set.

 *****************************************************************************/

template <class V>
inline JBoolean
JStringMap<V>::SetOldElement
	(
	const JCharacter* const key,
	const V&                value
	)
{
	return SetElement(key, value, JStringMapT::kIfOld);
}

/******************************************************************************
 SetContains

	A sometimes useful hybrid form; always sets the element, and returns kJTrue
	if the element already existed, kJFalse otherwise.

 *****************************************************************************/

template <class V>
inline JBoolean
JStringMap<V>::SetContains
	(
	const JCharacter* const key,
	const V&                value
	)
{
	JBoolean existed;
	SetElement(key, value, JPtrArrayT::kForget, JStringMapT::kAlways, &existed);
	return existed;
}

/******************************************************************************
 RemoveElement

 *****************************************************************************/

template <class V>
inline JBoolean
JStringMap<V>::RemoveElement
	(
	const JCharacter* const key
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

#include <JStringMap.tmpl>
#include <JStringMapCursor.tmpl>

#endif
