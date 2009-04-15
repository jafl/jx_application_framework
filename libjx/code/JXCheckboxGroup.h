/******************************************************************************
 JXCheckboxGroup.h

	Interface for the JXCheckboxGroup class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXCheckboxGroup
#define _H_JXCheckboxGroup

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JContainer.h>
#include <JPtrArray.h>

class JXCheckbox;

class JXCheckboxGroup : public JContainer
{
public:

	virtual ~JXCheckboxGroup();

	JBoolean	Includes(JXCheckbox* cb) const;
	void		Add(JXCheckbox* cb);
	void		Insert(const JIndex index, JXCheckbox* cb);
	void		Remove(JXCheckbox* cb);

protected:

	JXCheckboxGroup();
	JXCheckboxGroup(const JPtrArray<JXCheckbox>& cbList);

	JBoolean	AllChecked() const;
	JBoolean	AllUnchecked() const;

	JBoolean	AllDisabled() const;
	JBoolean	CheckboxDisabled(const JIndex index) const;

	JBoolean				FindCheckbox(JBroadcaster* obj, JIndex* index) const;
	JXCheckbox*				GetCheckbox(const JIndex index) const;
	JPtrArray<JXCheckbox>*	GetCheckboxList() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	EnforceConstraints(const JIndex cbIndex) = 0;

private:

	JPtrArray<JXCheckbox>*	itsCBList;		// contents are not owned

private:

	// not allowed

	JXCheckboxGroup(const JXCheckboxGroup& source);
	const JXCheckboxGroup& operator=(const JXCheckboxGroup& source);
};


/******************************************************************************
 Includes

 ******************************************************************************/

inline JBoolean
JXCheckboxGroup::Includes
	(
	JXCheckbox* cb
	)
	const
{
	return itsCBList->Includes(cb);
}

/******************************************************************************
 GetCheckbox (protected)

 ******************************************************************************/

inline JXCheckbox*
JXCheckboxGroup::GetCheckbox
	(
	const JIndex index
	)
	const
{
	return itsCBList->NthElement(index);
}

/******************************************************************************
 GetCheckboxList (protected)

 ******************************************************************************/

inline JPtrArray<JXCheckbox>*
JXCheckboxGroup::GetCheckboxList()
	const
{
	return itsCBList;
}

#endif
