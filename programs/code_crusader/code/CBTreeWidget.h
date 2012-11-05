/******************************************************************************
 CBTreeWidget.h

	Interface for the CBTreeWidget class

	Copyright © 1996-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_CBTreeWidget
#define _H_CBTreeWidget

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXScrollableWidget.h>

class JString;
class JXDirector;
class JXTextMenu;
class JXImage;
class CBProjectDocument;
class CBTreeDirector;
class CBTree;
class CBClass;
class CBFnListDirector;
class JPagePrinter;
class JEPSPrinter;

class CBTreeWidget : public JXScrollableWidget
{
public:

	static const JSize	kBorderWidth;

public:

	CBTreeWidget(CBTreeDirector* director, CBTree* tree,
				 JXScrollbarSet* scrollbarSet, JXContainer* enclosure,
				 const HSizingOption hSizing, const VSizingOption vSizing,
				 const JCoordinate x, const JCoordinate y,
				 const JCoordinate w, const JCoordinate h);

	virtual ~CBTreeWidget();

	CBTree*	GetTree() const;

	JBoolean	FindClass(const JCharacter* name,
						  const JXMouseButton button = kJXRightButton,
						  const JBoolean raiseTreeWindow = kJFalse,
						  const JBoolean reportNotFound = kJTrue,
						  const JBoolean openFileIfSingleMatch = kJTrue,
						  const JBoolean deselectAll = kJTrue) const;
	JBoolean	FindFunction(const JCharacter* fnName,
							 const JBoolean caseSensitive,
							 const JXMouseButton button = kJXRightButton,
							 const JBoolean raiseTreeWindow = kJFalse,
							 const JBoolean reportNotFound = kJTrue,
							 const JBoolean openFileIfSingleMatch = kJTrue,
							 const JBoolean deselectAll = kJTrue) const;

	void	Print(JPagePrinter& p);
	void	Print(JEPSPrinter& p);

	void	ReadSetup(istream& setInput, const JFileVersion setVers);
	void	WriteSetup(ostream& setOutput) const;

	static JBoolean	WillRaiseWindowWhenSingleMatch();
	static void		ShouldRaiseWindowWhenSingleMatch(const JBoolean raise);

	virtual void	HandleKeyPress(const int key, const JXKeyModifiers& modifiers);

	JXImage*	GetQtSignalImage() const;
	JXImage*	GetQtSlotImage() const;

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect);
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers);

	virtual JBoolean	HitSamePart(const JPoint& pt1, const JPoint& pt2) const;

	virtual void	GetSelectionData(JXSelectionData* data,
									 const JCharacter* id);
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers);
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const JBoolean dropAccepted, const Atom action);

	virtual JBoolean	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source);
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source);

	virtual void	HandleFocusEvent();
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	enum DragType
	{
		kInvalidDrag,
		kWaitForPopupFnMenuDrag
	};

private:

	CBTreeDirector*		itsDirector;		// owns us
	CBTree*				itsTree;			// not owned
	JString				itsKeyBuffer;

	JXTextMenu*			itsFnMenu;
	CBFnListDirector*	itsFnMenuDir;
	DragType			itsDragType;
	JPoint				itsStartPt;
	JXMouseButton		itsFnMenuButton;
	Time				itsMouseDownTime;
	CBClass*			itsFnMenuClass;		// not owned

	JString*			itsDNDBuffer;

	static JBoolean		itsRaiseWhenSingleMatchFlag;

	// for use by CBFnListWidget

	JXImage*	itsQtSignalImage;
	JXImage*	itsQtSlotImage;

private:

	void	ExpectPopupFnMenu(const JPoint& pt, const JXMouseButton button,
							  CBClass* theClass);

	// not allowed

	CBTreeWidget(const CBTreeWidget& source);
	const CBTreeWidget& operator=(const CBTreeWidget& source);
};


/******************************************************************************
 GetTree

 ******************************************************************************/

inline CBTree*
CBTreeWidget::GetTree()
	const
{
	return itsTree;
}

/******************************************************************************
 Raise window when single match (static)

 ******************************************************************************/

inline JBoolean
CBTreeWidget::WillRaiseWindowWhenSingleMatch()
{
	return itsRaiseWhenSingleMatchFlag;
}

inline void
CBTreeWidget::ShouldRaiseWindowWhenSingleMatch
	(
	const JBoolean raise
	)
{
	itsRaiseWhenSingleMatchFlag = raise;
}

/******************************************************************************
 Qt signal/slot images

 ******************************************************************************/

inline JXImage*
CBTreeWidget::GetQtSignalImage()
	const
{
	return itsQtSignalImage;
}

inline JXImage*
CBTreeWidget::GetQtSlotImage()
	const
{
	return itsQtSlotImage;
}

#endif
