/******************************************************************************
 GMAttachmentTable.h

	Interface for the GMAttachmentTable class

	Copyright (C) 1999 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMAttachmentTable
#define _H_GMAttachmentTable

#include <JXTable.h>
#include <JPtrArray-JString.h>

class GMMIMEParser;
class JXImage;
class JXTEBase;

class GMAttachmentTable : public JXTable
{
public:

	enum AttachmentType
	{
		k7BitText = 1,
		k8BitText,
		kBinary
	};

public:

	GMAttachmentTable(const JBoolean acceptDrop,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);
	virtual ~GMAttachmentTable();

	void	SetParser(GMMIMEParser* parser);
	void	AdjustColWidth();

	void	AddFile(const JCharacter* filename, const JBoolean local);
	void	SetEditMenuHandler(JXTEBase* editor);

	JSize			GetAttachmentCount() const;
	const JString&	GetAttachmentName(const JIndex index, JBoolean* binary) const;

	void	OpenSelectedAttachments();
	void	SaveSelectedAttachments();

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void		Receive(JBroadcaster* sender, const Message& message);
	virtual void		TableDrawCell(JPainter& p, const JPoint& cell, const JRect& rect);
	virtual void		HandleMouseDown(const JPoint& pt, const JXMouseButton button,
										const JSize clickCount,
										const JXButtonStates& buttonStates,
										const JXKeyModifiers& modifiers);
	virtual void		HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
										const JXKeyModifiers& modifiers);
	virtual void		HandleMouseUp(const JPoint& pt, const JXMouseButton button,
									  const JXButtonStates& buttonStates,
									  const JXKeyModifiers& modifiers);

	virtual void		GetSelectionData(JXSelectionData* data,
										const JCharacter* id);
	virtual void		HandleDNDResponse(const JXContainer* target,
										  const JBoolean dropAccepted, const Atom action);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);


private:

	JBoolean		itsAcceptDrop;
	JXImage*		itsFileIcon;
	JXImage*		itsBinaryIcon;
	JCursorIndex	itsDNDCursor;
	JPoint			itsDownPt;
	JBoolean		itsDownInCell;
	JCoordinate		itsMinColWidth;
	JString			itsAttachDir;

	JPtrArray<JString>*		itsFiles;
	JArray<AttachmentType>*	itsTypes;
	JArray<JBoolean>*		itsEncoding;

	JXTEBase*		itsEditMenuHandler;

private:

	AttachmentType	GetAttachmentType(const JCharacter* filename);
	void			RemoveSelectedFiles();

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex item);

	// not allowed

	GMAttachmentTable(const GMAttachmentTable& source);
	const GMAttachmentTable& operator=(const GMAttachmentTable& source);

};

/******************************************************************************
 GetAttachmentCount (public)

 ******************************************************************************/

inline JSize
GMAttachmentTable::GetAttachmentCount()
	const
{
	return itsFiles->GetElementCount();
}

#endif
