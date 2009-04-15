/******************************************************************************
 JXScrolltab.cpp

	Saves a position along a JXScrollbar.

	BASE CLASS = JXWidget

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXScrolltab.h>
#include <JXScrollbar.h>
#include <JXTextMenu.h>
#include <JXColormap.h>
#include <jMath.h>
#include <jAssert.h>

const JCoordinate kAcrossSize = 8;
const JCoordinate kAlongSize  = 8;

// Action menu

static const JCharacter* kActionMenuStr =
	"Scroll to this scrolltab %k Left-click"
	"| Remove this scrolltab  %k Meta-left-click"
	"| Remove all scrolltabs  %k Meta-Shift-left-click";

enum
{
	kScrollToCmd = 1,
	kRemoveCmd,
	kRemoveAllTabsCmd
};

/******************************************************************************
 Constructor

	The scrolltab is initially placed outside its enclosing aperture so it
	does not flash when JXScrollbar redraws to calculate its correct
	position.

 ******************************************************************************/

JXScrolltab::JXScrolltab
	(
	JXScrollbar*		scrollbar,
	const JCoordinate	value
	)
	:
	JXWidget(scrollbar, kFixedLeft, kFixedTop, -20,0, 10,10),
	itsScrollbar(scrollbar),
	itsValue(value),
	itsActionMenu(NULL)
{
	SetBackColor((GetColormap())->GetBlueColor());
	itsScrollbar->ScrolltabCreated(this);
	UpdatePosition();
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXScrolltab::~JXScrolltab()
{
	itsScrollbar->ScrolltabDeleted(this);
}

/******************************************************************************
 ScaleValue

 ******************************************************************************/

void
JXScrolltab::ScaleValue
	(
	const JFloat scaleFactor
	)
{
	SetValue(JRound(scaleFactor * itsValue));
}

/******************************************************************************
 ScrollToTab

 ******************************************************************************/

void
JXScrolltab::ScrollToTab()
	const
{
	itsScrollbar->SetValue(itsValue);
}

/******************************************************************************
 Position relative to scrollbar

	Redrawing the scrollbar triggers a call to either PlaceHoriz() or PlaceVert().

 ******************************************************************************/

void
JXScrolltab::UpdatePosition()
{
	itsScrollbar->Refresh();
}

void
JXScrolltab::PlaceHoriz
	(
	const JCoordinate	xmin,
	const JCoordinate	xmax,
	const JFloat		scale
	)
{
	JCoordinate x = xmin + JRound(scale * (itsValue - JXScrollbar::kMinValue));
	if (x > xmax)
		{
		x = xmax;
		}
	if (x < xmin)
		{
		x = xmin;
		}

	Place(x, itsScrollbar->GetBoundsHeight() - kAcrossSize);
	SetSize(kAlongSize, kAcrossSize);
}

void
JXScrolltab::PlaceVert
	(
	const JCoordinate	ymin,
	const JCoordinate	ymax,
	const JFloat		scale
	)
{
	JCoordinate y = ymin + JRound(scale * (itsValue - JXScrollbar::kMinValue));
	if (y > ymax)
		{
		y = ymax;
		}
	if (y < ymin)
		{
		y = ymin;
		}

	Place(itsScrollbar->GetBoundsWidth() - kAcrossSize, y);
	SetSize(kAcrossSize, kAlongSize);
}

/******************************************************************************
 HandleMouseDown (virtual protected)

 ******************************************************************************/

void
JXScrolltab::HandleMouseDown
	(
	const JPoint&			pt,
	const JXMouseButton		button,
	const JSize				clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	const JBoolean shiftOn = modifiers.shift();
	const JBoolean metaOn  = modifiers.meta();

	if (button == kJXLeftButton && metaOn && shiftOn)
		{
		itsScrollbar->RemoveAllScrolltabs();
		}
	else if (button == kJXLeftButton && metaOn)
		{
		delete this;
		}
	else if (button == kJXLeftButton)
		{
		ScrollToTab();
		}
	else if (button == kJXRightButton)
		{
		OpenActionMenu(pt, buttonStates, modifiers);
		}
}

/******************************************************************************
 OpenActionMenu (private)

 ******************************************************************************/

void
JXScrolltab::OpenActionMenu
	(
	const JPoint&			pt,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	if (itsActionMenu == NULL)
		{
		itsActionMenu = new JXTextMenu("", this, kFixedLeft, kFixedTop, 0,0, 10,10);
		assert( itsActionMenu != NULL );
		itsActionMenu->SetToHiddenPopupMenu();
		itsActionMenu->SetMenuItems(kActionMenuStr);
		itsActionMenu->SetUpdateAction(JXMenu::kDisableNone);
		ListenTo(itsActionMenu);
		}

	itsActionMenu->PopUp(this, pt, buttonStates, modifiers);
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
JXScrolltab::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsActionMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleActionMenu(selection->GetIndex());	// can destroy us
		}

	else
		{
		JXWidget::Receive(sender, message);
		}
}

/******************************************************************************
 HandleActionMenu (private)

 ******************************************************************************/

void
JXScrolltab::HandleActionMenu
	(
	const JIndex index
	)
{
	if (index == kScrollToCmd)
		{
		ScrollToTab();
		}
	else if (index == kRemoveCmd)
		{
		delete this;							// destroys us
		}
	else if (index == kRemoveAllTabsCmd)
		{
		itsScrollbar->RemoveAllScrolltabs();	// destroys us
		}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXScrolltab::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXScrolltab::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 HandleMouseEnter (virtual protected)

 ******************************************************************************/

void
JXScrolltab::HandleMouseEnter()
{
	SetBackColor((GetColormap())->GetLightBlueColor());
}

/******************************************************************************
 HandleMouseLeave (virtual protected)

 ******************************************************************************/

void
JXScrolltab::HandleMouseLeave()
{
	SetBackColor((GetColormap())->GetBlueColor());
}

#define JTemplateType JXScrolltab
#include <JPtrArray.tmpls>
#undef JTemplateType
