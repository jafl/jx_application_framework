/******************************************************************************
 JXCheckboxGroup.h

	Interface for the JXCheckboxGroup class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXCheckboxGroup
#define _H_JXCheckboxGroup

#include <JContainer.h>
#include <JPtrArray.h>

class JXCheckbox;

class JXCheckboxGroup : public JContainer
{
public:

	virtual ~JXCheckboxGroup();

	bool	Includes(JXCheckbox* cb) const;
	void	Add(JXCheckbox* cb);
	void	Insert(const JIndex index, JXCheckbox* cb);
	void	Remove(JXCheckbox* cb);

protected:

	JXCheckboxGroup();
	JXCheckboxGroup(const JPtrArray<JXCheckbox>& cbList);

	bool	AllChecked() const;
	bool	AllUnchecked() const;

	bool	AllDisabled() const;
	bool	CheckboxDisabled(const JIndex index) const;

	bool					FindCheckbox(JBroadcaster* obj, JIndex* index) const;
	JXCheckbox*				GetCheckbox(const JIndex index) const;
	JPtrArray<JXCheckbox>*	GetCheckboxList() const;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	EnforceConstraints(const JIndex cbIndex) = 0;

	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	JPtrArray<JXCheckbox>*	itsCBList;		// contents are not owned

private:

	// not allowed

	JXCheckboxGroup(const JXCheckboxGroup&) = delete;
	JXCheckboxGroup& operator=(const JXCheckboxGroup&) = delete;
};


/******************************************************************************
 Includes

 ******************************************************************************/

inline bool
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
	return itsCBList->GetElement(index);
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
