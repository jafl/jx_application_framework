/******************************************************************************
 GMFilter.h

	Interface for the GMFilter class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMFilter
#define _H_GMFilter

#include <JPtrArray.h>
#include <JString.h>

class GMFilterCondition;
class GMFilterAction;

class GMFilter
{
public:

public:

	GMFilter();
	GMFilter(const GMFilter& source);

	virtual ~GMFilter();

	const JString&	GetNickname() const;
	void			SetNickname(const JCharacter* nickname);

	JPtrArray<GMFilterCondition>*		GetConditions();
	const JPtrArray<GMFilterCondition>&	GetConditions() const;
	void								SetConditions(const JPtrArray<GMFilterCondition>& conditions);

	JPtrArray<GMFilterAction>*			GetActions();
	const JPtrArray<GMFilterAction>&	GetActions() const;
	void								SetActions(const JPtrArray<GMFilterAction>& actions);

	void				ReadSetup(istream& input);
	void				WriteSetup(ostream& output) const;

private:

	JString							itsNickname;
	JPtrArray<GMFilterCondition>*	itsConditions;
	JPtrArray<GMFilterAction>*		itsActions;

private:

	void GMFilterX();

	// not allowed

	const GMFilter& operator=(const GMFilter& source);

public:
};

/******************************************************************************
 GetConditions (public)

 ******************************************************************************/

inline const JPtrArray<GMFilterCondition>&
GMFilter::GetConditions()
	const
{
	return *itsConditions;
}

/******************************************************************************
 GetConditions (public)

 ******************************************************************************/

inline const JPtrArray<GMFilterAction>&
GMFilter::GetActions()
	const
{
	return *itsActions;
}


#endif
