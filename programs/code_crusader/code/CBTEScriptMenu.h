/******************************************************************************
 CBTEScriptMenu.h

	Copyright © 2001 by John Lindal.

 *****************************************************************************/

#ifndef _H_CBTEScriptMenu
#define _H_CBTEScriptMenu

#include <JXFSDirMenu.h>

class CBTextEditor;

class CBTEScriptMenu : public JXFSDirMenu
{
public:

	CBTEScriptMenu(CBTextEditor* te, const JString& path,
				   const JString& title, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	CBTEScriptMenu(CBTextEditor* te, const JString& path,
				   JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual	~CBTEScriptMenu();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	CBTextEditor*	itsTE;

private:

	void		CBTEScriptMenuX();
	bool	UpdateSelf();
	bool	HandleSelection(const JIndex index);

	static bool	ShowExecutables(const JDirEntry& entry);

	// not allowed

	CBTEScriptMenu(const CBTEScriptMenu& source);
	const CBTEScriptMenu& operator=(const CBTEScriptMenu& source);
};

#endif
