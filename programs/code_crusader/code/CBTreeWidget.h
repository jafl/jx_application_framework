/******************************************************************************
 CBTreeWidget.h

	Interface for the CBTreeWidget class

	Copyright © 1996-98 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBTreeWidget
#define _H_CBTreeWidget

#include <JXScrollableWidget.h>

class JString;
class JXDirector;
class JXTextMenu;
class JXImage;
class CBProjectDocument;
class CBTreeDirector;
class CBTree;
class CBClass;
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

	bool	FindClass(const JString& name,
						  const JXMouseButton button = kJXRightButton,
						  const bool raiseTreeWindow = false,
						  const bool reportNotFound = true,
						  const bool openFileIfSingleMatch = true,
						  const bool deselectAll = true) const;
	bool	FindFunction(const JString& fnName,
							 const bool caseSensitive,
							 const JXMouseButton button = kJXRightButton,
							 const bool raiseTreeWindow = false,
							 const bool reportNotFound = true,
							 const bool openFileIfSingleMatch = true,
							 const bool deselectAll = true) const;

	void	Print(JPagePrinter& p);
	void	Print(JEPSPrinter& p);

	void	ReadSetup(std::istream& setInput, const JFileVersion setVers);
	void	WriteSetup(std::ostream& setOutput) const;

	static bool	WillRaiseWindowWhenSingleMatch();
	static void		ShouldRaiseWindowWhenSingleMatch(const bool raise);

	virtual void	HandleKeyPress(const JUtf8Character& c,
								   const int keySym, const JXKeyModifiers& modifiers) override;

protected:

	virtual void	Draw(JXWindowPainter& p, const JRect& rect) override;
	virtual void	HandleMouseDown(const JPoint& pt, const JXMouseButton button,
									const JSize clickCount,
									const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;
	virtual void	HandleMouseDrag(const JPoint& pt, const JXButtonStates& buttonStates,
									const JXKeyModifiers& modifiers) override;

	virtual bool	HitSamePart(const JPoint& pt1, const JPoint& pt2) const override;

	virtual void	GetSelectionData(JXSelectionData* data,
									 const JString& id) override;
	virtual Atom	GetDNDAction(const JXContainer* target,
								 const JXButtonStates& buttonStates,
								 const JXKeyModifiers& modifiers) override;
	virtual void	HandleDNDResponse(const JXContainer* target,
									  const bool dropAccepted, const Atom action) override;

	virtual bool	WillAcceptDrop(const JArray<Atom>& typeList, Atom* action,
									   const JPoint& pt, const Time time,
									   const JXWidget* source) override;
	virtual void		HandleDNDDrop(const JPoint& pt, const JArray<Atom>& typeList,
									  const Atom action, const Time time,
									  const JXWidget* source) override;

	virtual void	HandleFocusEvent() override;
	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum DragType
	{
		kInvalidDrag,
		kWaitForPopupFnMenuDrag
	};

private:

	CBTreeDirector*	itsDirector;		// owns us
	CBTree*			itsTree;			// not owned
	JString			itsKeyBuffer;

	JXTextMenu*		itsFnMenu;
	DragType		itsDragType;
	JPoint			itsStartPt;
	JXMouseButton	itsFnMenuButton;
	Time			itsMouseDownTime;
	CBClass*		itsFnMenuClass;		// not owned

	static bool	itsRaiseWhenSingleMatchFlag;

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

inline bool
CBTreeWidget::WillRaiseWindowWhenSingleMatch()
{
	return itsRaiseWhenSingleMatchFlag;
}

inline void
CBTreeWidget::ShouldRaiseWindowWhenSingleMatch
	(
	const bool raise
	)
{
	itsRaiseWhenSingleMatchFlag = raise;
}

#endif
