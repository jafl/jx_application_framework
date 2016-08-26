/******************************************************************************
 SCVarListDirector.h

	Interface for the SCVarListDirector class

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCVarListDirector
#define _H_SCVarListDirector

#include <JXWindowDirector.h>

class SCCircuitVarList;
class SCVarTable;
class JXTextMenu;

class SCVarListDirector : public JXWindowDirector
{
public:

	SCVarListDirector(JXDirector* supervisor);

	virtual ~SCVarListDirector();

	void	SetVarList(SCCircuitVarList* varList);

	void	ReadSetup(istream& input, const JFileVersion vers);
	void	WriteSetup(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	SCCircuitVarList*	itsVarList;			// owned by supervisor
	SCVarTable*			itsVarTable;
	JXTextMenu*			itsVarMenu;			// owned by menu bar

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();

	void	UpdateVarMenu();
	void	HandleVarMenu(const JIndex item);

	// not allowed

	SCVarListDirector(const SCVarListDirector& source);
	const SCVarListDirector& operator=(const SCVarListDirector& source);
};

#endif
