/******************************************************************************
 SVNListBase.h

	Copyright @ 2008 by John Lindal.

 ******************************************************************************/

#ifndef _H_SVNListBase
#define _H_SVNListBase

#include <JXStringList.h>
#include "SVNTabBase.h"
#include <ace/LSOCK_Stream.h>
#include <ace/UNIX_Addr.h>
#include <JMessageProtocol.h>	// template; requires ace includes

class JProcess;
class JXTextMenu;

class SVNListBase : public JXStringList, public SVNTabBase
{
public:

	typedef JMessageProtocol<ACE_LSOCK_STREAM>	RecordLink;

public:

	SVNListBase(SVNMainDirector* director, JXTextMenu* editMenu,
				const JString& cmd, const JBoolean refreshRepo,
				const JBoolean refreshStatus, const JBoolean reload,
				const JBoolean enableContextMenu,
				JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				const HSizingOption hSizing, const VSizingOption vSizing,
				const JCoordinate x, const JCoordinate y,
				const JCoordinate w, const JCoordinate h);

	virtual ~SVNListBase();

	virtual void	UpdateActionsMenu(JXTextMenu* menu);
	virtual void	UpdateInfoMenu(JXTextMenu* menu);
	virtual void	RefreshContent();
	virtual void	GetSelectedFiles(JPtrArray<JString>* fullNameList,
									 const JBoolean includeDeleted = kJFalse);
	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers) override;

	virtual void	OpenSelectedItems();

protected:

	enum
	{
		kDiffEditedSelectedFilesCtxCmd = 1,
		kDiffCurrentSelectedFilesCtxCmd,
		kDiffPrevSelectedFilesCtxCmd,
		kInfoLogSelectedFilesCtxCmd,
		kPropSelectedFilesCtxCmd,
		kIgnoreSelectionCtxCmd,
		kShowSelectedFilesCtxCmd
	};

protected:

	JPtrArray<JString>*	GetLineList();

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;

	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual JBoolean	ShouldDisplayLine(JString* line) const;
	virtual void		StyleLine(const JIndex index, const JString& line,
								  const JFontStyle& errorStyle,
								  const JFontStyle& addStyle,
								  const JFontStyle& removeStyle) = 0;
	virtual JString		ExtractRelativePath(const JString& line) const = 0;

	virtual void	UpdateContextMenu(JXTextMenu* menu);
	virtual void	CopySelectedItems(const JBoolean fullPath);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;
	virtual void	ReceiveGoingAway(JBroadcaster* sender) override;

private:

	class CompareLines : public JElementComparison<JString*>
	{
	public:

		CompareLines(SVNListBase* widget);
		virtual ~CompareLines();

		virtual JListT::CompareResult		Compare(JString* const &, JString* const &) const;
		virtual JElementComparison<JString*>*	Copy() const;

	private:

		SVNListBase*	itsWidget;		// not owned
	};

private:

	const JString		itsCmd;
	JBoolean			itsRefreshRepoFlag;
	JBoolean			itsRefreshStatusFlag;
	JBoolean			itsReloadOpenFilesFlag;
	JXTextMenu*			itsEditMenu;		// not owned
	const JBoolean		itsEnableContextMenuFlag;
	JXTextMenu*			itsContextMenu;		// NULL until first used

	JPtrArray<JString>*	itsLineList;		// displayed

	JProcess*			itsProcess;			// can be NULL
	RecordLink*			itsMessageLink;		// can be NULL
	RecordLink*			itsErrorLink;		// can be NULL
	JPtrArray<JString>*	itsErrorList;		// cache while process is running
	DisplayState		itsDisplayState;
	JPtrArray<JString>*	itsSavedSelection;

	JBoolean			itsIsDraggingFlag;

private:

	void	SetConnection(const int outFD, const int errFD);
	void	DeleteLinks();
	void	ReceiveMessageLine();
	void	ReceiveErrorLine();
	void	DisplayErrors();

	void	UpdateEditMenu();
	void	HandleEditMenu(const JIndex item);

	JBoolean	CreateContextMenu();
	void		HandleContextMenu(const JIndex index);

	// not allowed

	SVNListBase(const SVNListBase& source);
	const SVNListBase& operator=(const SVNListBase& source);
};


/******************************************************************************
 GetLineList (protected)

 ******************************************************************************/

inline JPtrArray<JString>*
SVNListBase::GetLineList()
{
	return itsLineList;
}

#endif
