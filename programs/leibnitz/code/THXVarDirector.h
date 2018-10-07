/******************************************************************************
 THXVarDirector.h

	Interface for the THXVarDirector class

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_THXVarDirector
#define _H_THXVarDirector

#include <JXWindowDirector.h>

class JXTextMenu;
class JXTextButton;
class THXVarList;
class THXVarTable;

class THXVarDirector : public JXWindowDirector
{
public:

	THXVarDirector(JXDirector* supervisor, THXVarList* varList);
	THXVarDirector(std::istream& input, const JFileVersion vers,
				   JXDirector* supervisor, THXVarList* varList);

	virtual ~THXVarDirector();

	void	WriteState(std::ostream& output) const;

	// public so THXApp can call it

	virtual JBoolean	OKToDeactivate() override;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	THXVarTable*	itsVarTable;
	JXTextMenu*		itsActionsMenu;
	JXTextMenu*		itsHelpMenu;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(THXVarList* varList);

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	// not allowed

	THXVarDirector(const THXVarDirector& source);
	const THXVarDirector& operator=(const THXVarDirector& source);
};

#endif
