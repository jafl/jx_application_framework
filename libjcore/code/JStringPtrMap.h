#ifndef _H_JStringPtrMap
#define _H_JStringPtrMap

/******************************************************************************
 JStringPtrMap.h

	Interface for the JStringPtrMap template.

	Copyright (C) 1998 by Dustin Laurence.  All rights reserved.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include <JStringMap.h>
#include <JStringPtrMapCursor.h>

template <class V>
class JStringPtrMap : public JStringMap<V*>
{
	friend class JStringMapCursor<V*>;

public:

	JStringPtrMap(const JPtrArrayT::CleanUpAction action,
				  const JSize lgSize = kJDefaultLgMinTableSize);
	virtual ~JStringPtrMap();

	JBoolean GetElement(const JString& key, V** ptr);
	JBoolean GetElement(const JString& key, const V** ptr) const;

	JBoolean SetElement(const JString& key, V* ptr,
						const JPtrArrayT::SetElementAction action,
						const JStringMapT::SetType type = JStringMapT::kAlways);
	JBoolean SetNewElement(const JString& key, V* ptr);	// avoid shadowing
	JBoolean SetOldElement(const JString& key, V* ptr,
						   const JPtrArrayT::SetElementAction action);
	JBoolean SetContains(const JString& key, V* ptr,
						 const JPtrArrayT::SetElementAction action);

	JBoolean GetElement(const JUtf8Byte* key, V** ptr);
	JBoolean GetElement(const JUtf8Byte* key, const V** ptr) const;

	JBoolean SetElement(const JUtf8Byte* key, V* ptr,
						const JPtrArrayT::SetElementAction action,
						const JStringMapT::SetType type = JStringMapT::kAlways);
	JBoolean SetNewElement(const JUtf8Byte* key, V* ptr);	// avoid shadowing
	JBoolean SetOldElement(const JUtf8Byte* key, V* ptr,
						   const JPtrArrayT::SetElementAction action);
	JBoolean SetContains(const JUtf8Byte* key, V* ptr,
						 const JPtrArrayT::SetElementAction action);

	// these insert a *copy* of the object into the array
	// (only available if template instantiated with #define JStringPtrMapCopy)

	JBoolean SetElement(const JString& key, const V& data,
						const JPtrArrayT::SetElementAction action,
						const JStringMapT::SetType type = JStringMapT::kAlways);
	JBoolean SetNewElement(const JString& key, const V& data);
	JBoolean SetOldElement(const JString& key, const V& data,
						   const JPtrArrayT::SetElementAction action);
	JBoolean SetContains(const JString& key, const V& data,
						 const JPtrArrayT::SetElementAction action);

	JBoolean	DeleteElement(const JString& key);
	JBoolean	DeleteElementAsArray(const JString& key);

	JBoolean SetElement(const JUtf8Byte* key, const V& data,
						const JPtrArrayT::SetElementAction action,
						const JStringMapT::SetType type = JStringMapT::kAlways);
	JBoolean SetNewElement(const JUtf8Byte* key, const V& data);
	JBoolean SetOldElement(const JUtf8Byte* key, const V& data,
						   const JPtrArrayT::SetElementAction action);
	JBoolean SetContains(const JUtf8Byte* key, const V& data,
						 const JPtrArrayT::SetElementAction action);

	JBoolean	DeleteElement(const JUtf8Byte* key);
	JBoolean	DeleteElementAsArray(const JUtf8Byte* key);

	void	CleanOut();		// safest
	void	DeleteAll();
	void	DeleteAllAsArrays();

	JPtrArrayT::CleanUpAction	GetCleanUpAction() const;
	void						SetCleanUpAction(const JPtrArrayT::CleanUpAction action);

protected:

	virtual void PrepareForSet(const JPtrArrayT::SetElementAction action);

private:

	JPtrArrayT::CleanUpAction itsCleanUpAction;

private:

	// not allowed

	JStringPtrMap(const JStringPtrMap& source);
	const JStringPtrMap& operator=(const JStringPtrMap& source);
};


/******************************************************************************
 GetElement

	These override the version in JStringMap to preserve constness.

 *****************************************************************************/

template <class V>
inline JBoolean
JStringPtrMap<V>::GetElement
	(
	const JUtf8Byte* key,
	V**              ptr
	)
{
	return JStringMap<V*>::GetElement(JString(key, 0, kJFalse), ptr);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::GetElement
	(
	const JString& key,
	V**            ptr
	)
{
	return JStringMap<V*>::GetElement(key, ptr);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::GetElement
	(
	const JUtf8Byte* key,
	const V**        ptr
	)
	const
{
	return JStringMap<V*>::GetElement(JString(key, 0, kJFalse), const_cast<V**>(ptr));
}

template <class V>
inline JBoolean
JStringPtrMap<V>::GetElement
	(
	const JString& key,
	const V**      ptr
	)
	const
{
	return JStringMap<V*>::GetElement(key, const_cast<V**>(ptr));
}

/******************************************************************************
 Set...Element

	These override the versions in JStringMap to force you to consider the
	SetElementAction.

 *****************************************************************************/

template <class V>
inline JBoolean
JStringPtrMap<V>::SetElement
	(
	const JUtf8Byte*                   key,
	V*                                 ptr,
	const JPtrArrayT::SetElementAction action,
	const JStringMapT::SetType         type // = kAlways
	)
{
	JBoolean existed;
	return JStringMap<V*>::SetElement(JString(key, 0, kJFalse), ptr, action, type, &existed);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::SetElement
	(
	const JString&                     key,
	V*                                 ptr,
	const JPtrArrayT::SetElementAction action,
	const JStringMapT::SetType         type // = kAlways
	)
{
	JBoolean existed;
	return JStringMap<V*>::SetElement(key, ptr, action, type, &existed);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::SetNewElement
	(
	const JUtf8Byte* key,
	V*               ptr
	)
{
	return JStringMap<V*>::SetNewElement(JString(key, 0, kJFalse), ptr);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::SetNewElement
	(
	const JString& key,
	V*             ptr
	)
{
	return JStringMap<V*>::SetNewElement(key, ptr);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::SetOldElement
	(
	const JUtf8Byte*                   key,
	V*                                 ptr,
	const JPtrArrayT::SetElementAction action
	)
{
	return SetElement(JString(key, 0, kJFalse), ptr, action, JStringMapT::kIfOld);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::SetOldElement
	(
	const JString&                     key,
	V*                                 ptr,
	const JPtrArrayT::SetElementAction action
	)
{
	return SetElement(key, ptr, action, JStringMapT::kIfOld);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::SetContains
	(
	const JUtf8Byte*                   key,
	V*                                 ptr,
	const JPtrArrayT::SetElementAction action
	)
{
	JBoolean existed;
	JStringMap<V*>::SetElement(JString(key, 0, kJFalse), ptr, action, JStringMapT::kAlways, &existed);
	return existed;
}

template <class V>
inline JBoolean
JStringPtrMap<V>::SetContains
	(
	const JString&                     key,
	V*                                 ptr,
	const JPtrArrayT::SetElementAction action
	)
{
	JBoolean existed;
	JStringMap<V*>::SetElement(key, ptr, action, JStringMapT::kAlways, &existed);
	return existed;
}

/******************************************************************************
 DeleteElement

 *****************************************************************************/

template <class V>
inline JBoolean
JStringPtrMap<V>::DeleteElement
	(
	const JUtf8Byte* key
	)
{
	return JStringMap<V*>::RemoveElement(JString(key, 0, kJFalse), JPtrArrayT::kDelete);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::DeleteElement
	(
	const JString& key
	)
{
	return JStringMap<V*>::RemoveElement(key, JPtrArrayT::kDelete);
}

/******************************************************************************
 DeleteElementAsArray

 *****************************************************************************/

template <class V>
inline JBoolean
JStringPtrMap<V>::DeleteElementAsArray
	(
	const JUtf8Byte* key
	)
{
	return JStringMap<V*>::RemoveElement(JString(key, 0, kJFalse), JPtrArrayT::kDeleteAsArray);
}

template <class V>
inline JBoolean
JStringPtrMap<V>::DeleteElementAsArray
	(
	const JString& key
	)
{
	return JStringMap<V*>::RemoveElement(key, JPtrArrayT::kDeleteAsArray);
}

/******************************************************************************
 DeleteAll

 *****************************************************************************/

template <class V>
inline void
JStringPtrMap<V>::DeleteAll()
{
	JStringMap<V*>::RemoveAll(JPtrArrayT::kDelete);
}

/******************************************************************************
 DeleteAllAsArrays

 *****************************************************************************/

template <class V>
inline void
JStringPtrMap<V>::DeleteAllAsArrays()
{
	JStringMap<V*>::RemoveAll(JPtrArrayT::kDeleteAsArray);
}

/******************************************************************************
 Clean up action

	What to do when the destructor is called.  This is especially useful
	for objects allocated on the stack, because one can set the appropriate
	action and then never have to worry about short-circuit returns.

 ******************************************************************************/

template <class V>
inline JPtrArrayT::CleanUpAction
JStringPtrMap<V>::GetCleanUpAction()
	const
{
	return itsCleanUpAction;
}

template <class V>
inline void
JStringPtrMap<V>::SetCleanUpAction
	(
	const JPtrArrayT::CleanUpAction action
	)
{
	itsCleanUpAction = action;
}

#include <JStringPtrMap.tmpl>
#include <JStringPtrMapCursor.tmpl>

#endif
