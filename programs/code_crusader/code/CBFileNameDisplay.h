/******************************************************************************
 CBFileNameDisplay.h

	Copyright (C) 2001 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBFileNameDisplay
#define _H_CBFileNameDisplay

#include <JXFileInput.h>

class CBTextDocument;
class CBFileDragSource;

class CBFileNameDisplay : public JXFileInput
{
public:

	CBFileNameDisplay(CBTextDocument* doc, CBFileDragSource* dragSource,
					  JXContainer* enclosure,
					  const HSizingOption hSizing, const VSizingOption vSizing,
					  const JCoordinate x, const JCoordinate y,
					  const JCoordinate w, const JCoordinate h);

	virtual ~CBFileNameDisplay();

	void	SetTE(JXTEBase* te);
	void	DiskCopyIsModified(const JBoolean mod);

	virtual void		HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;
	virtual JBoolean	InputValid();

protected:

	virtual void	HandleFocusEvent() override;
	virtual void	HandleUnfocusEvent() override;

	virtual void	AdjustStylesBeforeRecalc(const JString& buffer, JRunArray<JFont>* styles,
											 JIndexRange* recalcRange, JIndexRange* redrawRange,
											 const JBoolean deletion);

private:

	enum Action
	{
		kCancel,
		kSaveAs,
		kRename
	};

private:

	JXTEBase*	itsTE;					// not owned
	JString		itsOrigFile;
	Action		itsUnfocusAction;

	CBTextDocument*		itsDoc;			// not owned
	CBFileDragSource*	itsDragSource;	// not owned
	JBoolean			itsDiskModFlag;
	JBoolean			itsCBHasFocusFlag;

private:

	void	UpdateDisplay(const JBoolean hasFocus);

	// not allowed

	CBFileNameDisplay(const CBFileNameDisplay& source);
	const CBFileNameDisplay& operator=(const CBFileNameDisplay& source);
};

#endif
