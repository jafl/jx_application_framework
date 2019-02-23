/******************************************************************************
 JXToolBarButton.cpp

	BASE CLASS = public JXButton

	Copyright (C) 1998 by Glenn W. Bach.

	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "JXToolBar.h"
#include "JXToolBarButton.h"
#include "JXHintDirector.h"
#include "JXTextMenu.h"
#include "JXImage.h"
#include "JXColorManager.h"
#include "JXWindowPainter.h"
#include "jXGlobals.h"
#include "jXPainterUtil.h"
#include <JFontManager.h>
#include <jAssert.h>

const JCoordinate kLabelBuffer = 10;
const JCoordinate kDualBuffer  = 5;

/******************************************************************************
 Constructor

 *****************************************************************************/

JXToolBarButton::JXToolBarButton
	(
	JXToolBar*			toolBar,
	JXTextMenu*			menu,
	const JString&		itemID,
	const Type			type,
	JXContainer*		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	h
	)
	:
	JXButton(enclosure, hSizing, vSizing, x, y, h, h),
	itsToolBar(toolBar),
	itsMenu(menu),
	itsMenuItemID(itemID),
	itsIsCheckedFlag(kJFalse),
	itsType(type),
	itsFont(JFontManager::GetDefaultFont()),
	itsImage(nullptr)
{
	JBoolean invalid;
	NeedsGeometryAdjustment(&invalid);	// gets actual font to use
	assert( !invalid );
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JXToolBarButton::~JXToolBarButton()
{
}

/******************************************************************************
 GetMenuItemIndex

 ******************************************************************************/

JBoolean
JXToolBarButton::GetMenuItemIndex
	(
	JIndex* itemIndex
	)
	const
{
	return itsMenu->ItemIDToIndex(itsMenuItemID, itemIndex);
}

JFont
JXToolBarButton::GetLabelFont()
	const
{
	JFont font = itsFont;
	if (!IsActive())
		{
		font.SetColor(JColorManager::GetInactiveLabelColor());
		}
	return font;
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
JXToolBarButton::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	const JRect bounds = GetBounds();

	if (itsType == kImage && itsImage != nullptr)
		{
		p.Image(*itsImage, itsImage->GetBounds(), bounds);
		}
	else if (itsType == kText || itsImage == nullptr)
		{
		p.SetFont(GetLabelFont());
		p.JPainter::String(bounds, itsLabel,
						   JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
	else
		{
		assert(itsImage != nullptr);

		JRect ri = bounds;
		ri.right = bounds.left + bounds.height();
		p.Image(*itsImage, itsImage->GetBounds(), ri);

		JRect rt = bounds;
		rt.left  = ri.right;
		rt.right = bounds.right - kDualBuffer;
		p.SetFont(GetLabelFont());
		p.JPainter::String(rt, itsLabel,
						   JPainter::kHAlignCenter, JPainter::kVAlignCenter);
		}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
JXToolBarButton::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
	const JSize borderWidth = GetBorderWidth();
	if (borderWidth > 0 && IsActive())
		{
		if (( IsPushed() && !itsIsCheckedFlag) ||
			(!IsPushed() &&  itsIsCheckedFlag))
			{
			JXDrawDownFrame(p, frame, borderWidth);
			}
		else
			{
			JXDrawUpFrame(p, frame, borderWidth);
			}
		}
	else if (borderWidth > 0)
		{
		p.SetLineWidth(borderWidth);
		if (itsIsCheckedFlag)
			{
			p.SetPenColor(JColorManager::GetWhiteColor());
			}
		else
			{
			p.SetPenColor(JColorManager::GetInactiveLabelColor());
			}
		p.RectInside(frame);
		}
}

/******************************************************************************
 NeedsGeometryAdjustment

 ******************************************************************************/

JBoolean
JXToolBarButton::NeedsGeometryAdjustment
	(
	JBoolean* invalid
	)
{
	JIndex itemIndex;
	if (!GetMenuItemIndex(&itemIndex))
		{
		*invalid = kJTrue;
		return kJTrue;
		}

	*invalid = kJFalse;

	// refresh if state changed

	const JBoolean checked = itsMenu->IsChecked(itemIndex);
	if (checked != itsIsCheckedFlag)
		{
		itsIsCheckedFlag = checked;
		Refresh();
		}

	// label still needed for hint even if only image is displayed

	const JString& label = itsMenu->GetItemText(itemIndex);
	if (itsLabel != label)
		{
		itsLabel = label;
		if (itsType != kImage)
			{
			Refresh();
			}
		}

	// assume image doesn't change

	itsMenu->GetItemImage(itemIndex, &itsImage);

	if (itsType == kImage && itsImage != nullptr)
		{
		JString hint = itsLabel;
		JString shortCut;
		if (itsMenu->GetItemNMShortcut(itemIndex, &shortCut))
			{
			hint += "   " + shortCut;
			}
		SetHint(hint);
		return kJFalse;
		}
	else
		{
		itsFont                 = itsMenu->GetItemFont(itemIndex);
		const JCoordinate width = itsFont.GetStringWidth(GetFontManager(), itsLabel);
		const JCoordinate newWidth =
			(itsType == kText || itsImage == nullptr) ?
				width + 2*kLabelBuffer :
				width + 2*kDualBuffer + GetFrameHeight();

		const JCoordinate delta = newWidth - GetFrameWidth();
		if (delta != 0)
			{
			AdjustSize(delta,0);
			return kJTrue;
			}
		else
			{
			return kJFalse;
			}
		}
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

 ******************************************************************************/

JBoolean
JXToolBarButton::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	JIndex itemIndex;
	if (GetMenuItemIndex(&itemIndex))
		{
		const JString* id;
		const JBoolean hasID = itsMenu->GetItemID(itemIndex, &id);
		assert( hasID );

		JXToolBar::WantsToDrop msg(*id, typeList, *action, time, source);
		itsToolBar->BroadcastWithFeedback(&msg);
		return msg.WasAccepted();
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	We use BroadcastWithFeedback() so the recipients can signal to each
	other that the message has been processed.

 ******************************************************************************/

void
JXToolBarButton::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JIndex itemIndex;
	if (GetMenuItemIndex(&itemIndex))
		{
		const JString* id;
		const JBoolean hasID = itsMenu->GetItemID(itemIndex, &id);
		assert( hasID );

		JXToolBar::HandleDrop msg(*id, typeList, action, time, source);
		itsToolBar->BroadcastWithFeedback(&msg);
		}
}
