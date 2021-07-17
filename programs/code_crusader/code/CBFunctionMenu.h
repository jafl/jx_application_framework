/******************************************************************************
 CBFunctionMenu.h

	Interface for the CBFunctionMenu class

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBFunctionMenu
#define _H_CBFunctionMenu

#include <JXTextMenu.h>
#include "CBCtagsUser.h"
#include <JString.h>

class JXFileDocument;
class JXTEBase;

class CBFunctionMenu : public JXTextMenu
{
public:

	CBFunctionMenu(JXFileDocument* doc, const CBTextFileType type,
				   JXTEBase* te, JXContainer* enclosure,
				   const HSizingOption hSizing, const VSizingOption vSizing,
				   const JCoordinate x, const JCoordinate y,
				   const JCoordinate w, const JCoordinate h);

	CBFunctionMenu(JXFileDocument* doc, const CBTextFileType type, JXTEBase* te,
				   JXMenu* owner, const JIndex itemIndex, JXContainer* enclosure);

	virtual ~CBFunctionMenu();

	void	TextChanged(const CBTextFileType type, const JString& fileName);

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXFileDocument*		itsDoc;				// not owned; can be nullptr (Medic)
	CBTextFileType		itsFileType;
	JXTEBase*			itsTE;				// not owned
	JArray<JIndex>*		itsLineIndexList;
	JArray<CBLanguage>*	itsLineLangList;
	JIndex				itsCaretItemIndex;
	bool			itsNeedsUpdate;		// true if must rebuild menu
	bool			itsSortFlag;		// true if menu is currently sorted
	bool			itsIncludeNSFlag;	// true if menu is displaying namespace
	bool			itsPackFlag;		// true if menu is currently packed

	#ifdef CODE_MEDIC
	JString itsFileName;
	#endif

private:

	void	CBFunctionMenuX(JXFileDocument* doc, const CBTextFileType type,
							JXTEBase* te);

	void	SetEmptyMenuItems();
	void	UpdateMenu();
	void	HandleSelection(const JIndex index);

	// not allowed

	CBFunctionMenu(const CBFunctionMenu& source);
	const CBFunctionMenu& operator=(const CBFunctionMenu& source);
};

#endif
