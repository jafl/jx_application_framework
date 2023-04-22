/******************************************************************************
 TestWidget.h

	Interface for the TestWidget class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestWidget
#define _H_TestWidget

#include <jx-af/jx/JXScrollableWidget.h>
#include <jx-af/jcore/JKLRand.h>

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

	TestWidget(const bool isMaster, const bool isImage,
			   JXMenuBar* menuBar, JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
			   const HSizingOption hSizing, const VSizingOption vSizing,
			   const JCoordinate x, const JCoordinate y,
			   const JCoordinate w, const JCoordinate h);

	~TestWidget() override;

	void	Print(JPagePrinter& p);
	void	Print(JEPSPrinter& p);

	void	HandleKeyPress(const JUtf8Character& c, const int keySym,
						   const JXKeyModifiers& modifiers) override;

protected:

	void	Draw(JXWindowPainter& p, const JRect& rect) override;

	void	AdjustCursor(const JPoint& pt, const JXKeyModifiers& modifiers) override;
	void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
							const JSize clickCount,
							const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
							const JXKeyModifiers& modifiers) override;
	void	HandleMouseUp(const JPoint& pt, const JXMouseButton button,
						  const JXButtonStates& buttonStates,
						  const JXKeyModifiers& modifiers) override;

	bool	HitSamePart(const JPoint& pt1, const JPoint& pt2) const override;

	Atom	GetDNDAction(const JXContainer* target,
						 const JXButtonStates& buttonStates,
						 const JXKeyModifiers& modifiers) override;

	bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
						   const JPoint& pt, const Time time,
						   const JXWidget* source) override;
	void	HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
						  const Atom action, const Time time,
						  const JXWidget* source) override;

	bool	HandleClientMessage(const XClientMessageEvent& clientMessage) override;

	void	BoundsResized(const JCoordinate dw, const JCoordinate dh) override;
	void	ReceiveWithFeedback(JBroadcaster* sender, Message* message) override;

private:

	bool	itsFillFlag;

	JRect	its2Rect;
	JRect	its3Rect;

	JSize	itsRandPointCount;
	JKLRand	itsRNG;

	JXImage*	itsXPMImage;
	JXImage*	itsPartialXPMImage;
	JXImage*	itsImageBuffer;		// can be nullptr

	JXImage*	itsHomeImage;
	JRect		itsHomeRect;

	// cursors

	JCursorIndex	itsTrekCursor;
	JCursorIndex	itsGumbyCursor;
	JCursorIndex	itsBogosityCursor;
	JCursorIndex	itsFleurCursor;

	// owned by JXContainer

	JXTextButton*	itsAnimButton;
	JXTextButton*	itsQuitButton;		// can be nullptr
	JXTextMenu*		itsSecretMenu;
	JXTextMenu*		itsSecretSubmenu;

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

	void	BuildXlsfontsMenu(JXMenu* owner, JXContainer* enclosure);
	void	PrintSelectionTargets(const Time time);
	bool	PrintSelectionText(const Atom selectionName, const Time time,
							   const Atom type) const;
	void	PrintFileNames(const Atom selectionName, const Time time,
						   const Atom type) const;
};

#endif
