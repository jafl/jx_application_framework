/******************************************************************************
 CBPathTable.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBPathTable
#define _H_CBPathTable

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXEditTable.h>

class JString;
class JStringTableData;
class JXImage;
class JXTextButton;
class JXPathInput;
class CBDirList;
class CBRelPathCSF;

class CBPathTable : public JXEditTable
{
public:

	CBPathTable(const CBDirList& pathList,
				JXTextButton* addPathButton, JXTextButton* removePathButton,
				JXTextButton* choosePathButton, CBRelPathCSF* csf,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~CBPathTable();

	void	AddDirectories(const JPtrArray<JString>& list);
	void	GetPathList(CBDirList* pathList) const;

	virtual JBoolean	IsEditable(const JPoint& cell) const;

protected:

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual void			TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual JXInputField*	CreateXInputField(const JPoint& cell,
											  const JCoordinate x, const JCoordinate y,
											  const JCoordinate w, const JCoordinate h);
	virtual JBoolean		ExtractInputData(const JPoint& cell);
	virtual void			PrepareDeleteXInputField();

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	virtual void	ApertureResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JStringTableData*	itsData;
	JXImage*			itsFolderIcon;
	JXPathInput*		itsPathInput;			// NULL unless editing
	JString				itsBasePath;

	JXTextButton*		itsAddPathButton;		// not owned
	JXTextButton*		itsRemovePathButton;	// not owned
	JXTextButton*		itsChoosePathButton;	// not owned
	CBRelPathCSF*		itsCSF;					// not owned

private:

	void	ChoosePath();
	void	UpdateButtons();

	// not allowed

	CBPathTable(const CBPathTable& source);
	const CBPathTable& operator=(const CBPathTable& source);
};

#endif
