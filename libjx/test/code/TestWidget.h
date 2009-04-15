/******************************************************************************
 TestWidget.h

	Interface for the TestWidget class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestWidget
#define _H_TestWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXScrollableWidget.h>
#include <JKLRand.h>

class AnimateColorTask;
class ResizeWidgetDialog;

class JPainter;
class JPagePrinter;
class JEPSPrinter;
class JXImage;
class JXMenuBar;
class JXMenu;
class JXTextMenu;
class JXTextButton;

class TestWidget : public JXScrollableWidget
{
public:

	TestWidget(const JBoolean isMaster, const JBoolean isImage,
			   const JBoolean allocDynamicColors, JXMenuBar* menuBar,
			   JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	virtual ~TestWidget();

	void	Print(JPagePrinter& p);
	void	Print(JEPSPrinter& p);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);

	virtual void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
								  const JXButtonStates& buttonStates,
								  const JXKeyModifiers& modifiers);

	virtual JBoolean	HitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const Time time, const JXWidget* source);
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	virtual JBoolean	HandleClientMessage(const XClientMessageEvent& clientMessage);

	virtual void	BoundsResized(const JCoordinate dw, const JCoordinate dh);
	virtual void	Receive(JBroadcaster* sender, const Message& message);
	virtual void	ReceiveWithFeedback(JBroadcaster* sender, Message* message);

private:

	JBoolean	itsFillFlag;
	JColorIndex	itsNextAnimColor;

	JRect	its2Rect;
	JRect	its3Rect;

	JSize	itsRandPointCount;
	JKLRand	itsRNG;

	JArray<JColorIndex>*	itsAnimColorList;	// can be empty
	AnimateColorTask*		itsAnimColorTask;	// can be NULL

	JXImage*	itsXPMImage;
	JXImage*	itsPartialXPMImage;
	JXImage*	itsImageBuffer;		// can be NULL

	JXImage*	itsHomeImage;
	JRect		itsHomeRect;

	// cursors

	JCursorIndex	itsTrekCursor;
	JCursorIndex	itsGumbyCursor;
	JCursorIndex	itsBogosityCursor;
	JCursorIndex	itsFleurCursor;

	// owned by JXContainer

	JXTextButton*	itsAnimButton;
	JXTextButton*	itsQuitButton;		// can be NULL
	JXTextMenu*		itsSecretMenu;
	JXTextMenu*		itsSecretSubmenu;

	// owned by our Window's Director

	ResizeWidgetDialog*	itsResizeDialog;

	// owned by the menu bar

	JXTextMenu*	itsActionsMenu;
	JXTextMenu*	itsPointMenu;

	// used during drag

	JPoint	itsStartPt;
	JPoint	itsPrevPt;

private:

	void	DrawStuff(JPainter& p);
	void	CreateImageBuffer();

	void	UpdateActionsMenu();
	void	HandleActionsMenu(const JIndex item);

	void	UpdatePointMenu();
	void	HandlePointMenu(const JIndex item);

	void	GetNewSize();
	void	ChangeSize();

	void	BuildXlsfontsMenu(JXMenu* owner, JXContainer* enclosure);
	void	PrintSelectionTargets(const Time time);
	void	PrintSelectionText(const Atom selectionName, const Time time,
							   const Atom type) const;
	void	PrintFileNames(const Atom selectionName, const Time time) const;

	// not allowed

	TestWidget(const TestWidget& source);
	const TestWidget& operator=(const TestWidget& source);
};

#endif
