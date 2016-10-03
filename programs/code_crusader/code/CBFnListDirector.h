/******************************************************************************
 CBFnListDirector.h

	Interface for the CBFnListDirector class

	Copyright (C) 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBFnListDirector
#define _H_CBFnListDirector

#include <JXWindowDirector.h>

class CBClass;
class CBTreeWidget;
class CBTreeDirector;
class CBFnListWidget;

class JXScrollbarSet;
class JXTextButton;
class JXTextMenu;
class JXPSPrinter;

class CBFnListDirector : public JXWindowDirector
{
public:

	CBFnListDirector(CBTreeDirector* supervisor, JXPSPrinter* printer,
					 const CBClass* theClass, const CBTreeWidget* treeWidget,
					 const JBoolean showInheritedFns,
					 const JBoolean forPopupMenu = kJFalse);
	CBFnListDirector(istream& input, const JFileVersion vers, JBoolean* keep,
					 CBTreeDirector* supervisor, JXPSPrinter* printer,
					 const CBTreeWidget* treeWidget);

	virtual ~CBFnListDirector();

	JBoolean	IsShowingClass(const CBClass* theClass) const;
	void		Reconnect();

	CBFnListWidget*	GetFnListWidget() const;

	void		StreamOut(ostream& output) const;
	static void	SkipSetup(istream& input);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	CBTreeDirector*	itsDirector;			// owns us
	CBFnListWidget*	itsFnListWidget;
	JXTextMenu*		itsActionsMenu;
	JXPSPrinter*	itsPrinter;				// not owned; can be NULL

// begin JXLayout


// end JXLayout

private:

	void	CBFnListDirectorX(CBTreeDirector* director,
							  JXPSPrinter* printer,
							  const JBoolean forPopupMenu,
							  const JBoolean willBeKept);

	JXScrollbarSet*	BuildWindow();

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex index);

	// not allowed

	CBFnListDirector(const CBFnListDirector& source);
	const CBFnListDirector& operator=(const CBFnListDirector& source);
};


/******************************************************************************
 GetFnListWidget

 ******************************************************************************/

inline CBFnListWidget*
CBFnListDirector::GetFnListWidget()
	const
{
	return itsFnListWidget;
}

#endif
