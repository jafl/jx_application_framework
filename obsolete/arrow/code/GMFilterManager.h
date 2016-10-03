/******************************************************************************
 GMFilterManager.h

	Interface for the GMFilterManager class

	Copyright (C) 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMFilterManager
#define _H_GMFilterManager


#include <JBroadcaster.h>
#include <JPrefObject.h>
#include <JPtrArray.h>

class GMFilterDialog;
class GMFilter;
class GMFilterCondition;

class GMFilterManager : virtual public JBroadcaster, public JPrefObject
{
public:

	GMFilterManager();
	virtual ~GMFilterManager();

	void	EditFilters();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

	virtual void	ReadPrefs(istream& input);
	virtual void	WritePrefs(ostream& output) const;

private:

	GMFilterDialog*			itsDialog;
	JPtrArray<GMFilter>*	itsFilters;

private:

	JBoolean	OKToOverwrite();
	void		WriteFilterFile();
	void		WriteFilter(ostream& os, GMFilter* filter);
	void		WriteCondition(ostream& os, GMFilterCondition* condition);

	// not allowed

	GMFilterManager(const GMFilterManager& source);
	const GMFilterManager& operator=(const GMFilterManager& source);

public:
};

#endif
