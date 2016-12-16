/******************************************************************************
 GMFilterDialog.h

	Interface for the GMFilterDialog class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMFilterDialog
#define _H_GMFilterDialog


#include <JXDialogDirector.h>
#include <JPtrArray.h>
#include <JPrefObject.h>

class GMFilterNameTable;
class GMFilterConditionTable;
class GMFilterActionTable;
class GMFilter;
class JXVertPartition;
class JXTextButton;

class GMFilterDialog : public JXDialogDirector, public JPrefObject
{
public:

	GMFilterDialog(const JPtrArray<GMFilter>& filters);
	virtual ~GMFilterDialog();

	JPtrArray<GMFilter>*	GetFilters();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

private:

	GMFilterNameTable*		itsFilterNameTable;
	GMFilterConditionTable*	itsConditionTable;
	GMFilterActionTable*	itsActionTable;
	JPtrArray<GMFilter>*	itsFilters;

// begin JXLayout

	JXVertPartition* itsVertPartition;
	JXTextButton*    itsHelpButton;

// end JXLayout

// begin filterLayout


// end filterLayout

// begin conditionLayout


// end conditionLayout

// begin actionLayout


// end actionLayout

private:

	void BuildWindow();

	// not allowed

	GMFilterDialog(const GMFilterDialog& source);
	const GMFilterDialog& operator=(const GMFilterDialog& source);

public:
};

/******************************************************************************
 GetFilters (public)

 ******************************************************************************/

inline JPtrArray<GMFilter>*
GMFilterDialog::GetFilters()
{
	return itsFilters;
}


#endif
