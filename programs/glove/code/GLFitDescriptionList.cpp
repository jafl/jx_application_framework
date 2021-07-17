/******************************************************************************
 GLFitDescriptionList.cpp

	BASE CLASS = public JXEditTable

	Copyright (C) 2000 by Glenn W. Bach.

 *****************************************************************************/

#include <GLFitDescriptionList.h>

#include "GLFitManager.h"
#include "GLGlobals.h"

#include "glBuiltInFit.xpm"
#include "glNonLinearFit.xpm"
#include "glPolyFit.xpm"

#include <J2DPlotWidget.h>

#include <JXColorManager.h>
#include <JXDisplay.h>
#include <JXImage.h>
#include <JXInputField.h>

#include <JFontManager.h>
#include <JPainter.h>
#include <JTableSelection.h>

#include <jx_executable_small.xpm>

#include <jASCIIConstants.h>

#include <JMinMax.h>
#include <jAssert.h>

const JCoordinate kHMarginWidth = 3;
const JCoordinate kVMarginWidth = 1;
const JCoordinate kDefColWidth  = 100;
const JCoordinate kIconWidth	= 15;

const JUtf8Byte* GLFitDescriptionList::kFitSelected  = "GLFitDescriptionList::kFitSelected";
const JUtf8Byte* GLFitDescriptionList::kFitInitiated = "GLFitDescriptionList::kFitInitiated";;

/******************************************************************************
 Constructor

 *****************************************************************************/

GLFitDescriptionList::GLFitDescriptionList
	(
	JXScrollbarSet*		scrollbarSet,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXEditTable(1, kDefColWidth, scrollbarSet, enclosure, hSizing,vSizing, x,y, w,h),
	itsInput(nullptr)
{
	itsMinColWidth = 1;

	JFontManager* fontMgr = GetFontManager();
	const JSize rowHeight = 2*kVMarginWidth + JFontManager::GetDefaultFont().GetLineHeight(fontMgr);
	SetDefaultRowHeight(rowHeight);

	itsNameList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsNameList != nullptr);

	AppendCols(1);
	SyncWithManager();

	SetRowBorderInfo(0, JColorManager::GetBlackColor());
	SetColBorderInfo(0, JColorManager::GetBlackColor());

	itsBuiltInIcon	= jnew JXImage(GetDisplay(), JXPM(glBuiltInFit));
	assert(itsBuiltInIcon != nullptr);
	itsBuiltInIcon->ConvertToRemoteStorage();

	itsNonLinearIcon	= jnew JXImage(GetDisplay(), JXPM(glNonLinearFit));
	assert(itsNonLinearIcon != nullptr);
	itsNonLinearIcon->ConvertToRemoteStorage();

	itsPolyIcon	= jnew JXImage(GetDisplay(), JXPM(glPolyFit));
	assert(itsPolyIcon != nullptr);
	itsPolyIcon->ConvertToRemoteStorage();

	itsExecutableIcon = jnew JXImage(GetDisplay(), JXPM(jx_executable_small));
	assert( itsExecutableIcon != nullptr );
	itsExecutableIcon->ConvertToRemoteStorage();

	ListenTo(GLGetFitManager());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLFitDescriptionList::~GLFitDescriptionList()
{
	itsNameList->DeleteAll();
	jdelete itsNameList;		// we don't own the strings
	jdelete itsBuiltInIcon;
	jdelete itsNonLinearIcon;
	jdelete itsPolyIcon;
	jdelete itsExecutableIcon;
}

/******************************************************************************
 HandleMouseDown (protected)

 ******************************************************************************/

void
GLFitDescriptionList::HandleMouseDown
	(
	const JPoint& 			pt,
	const JXMouseButton 	button,
	const JSize 			clickCount,
	const JXButtonStates&	buttonStates,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	if (button == kJXLeftButton && GetCell(pt, &cell))
		{
		if (GetTableSelection().IsSelected(cell) && clickCount == 2)
			{
			Broadcast(FitInitiated(cell.y));
			}
		else if (!GetTableSelection().IsSelected(cell) && clickCount == 1)
			{
			SelectSingleCell(cell);
			Broadcast(FitSelected(cell.y));
			}
		}
	else
		{
		ScrollForWheel(button, modifiers);
		}
}

/******************************************************************************
 TableDrawCell (virtual protected)

 ******************************************************************************/

void
GLFitDescriptionList::TableDrawCell
	(
	JPainter&		p,
	const JPoint&	cell,
	const JRect&	rect
	)
{
	HilightIfSelected(p, cell, rect);

	const JString* curveName = itsNameList->GetElement(cell.y);

	const GLFitDescription& fd	= GLGetFitManager()->GetFitDescription(cell.y);

	JRect irect	= rect;
	irect.right	= rect.left + kIconWidth;
	if (fd.GetType()	== GLFitDescription::kPolynomial)
		{
		p.Image(*itsPolyIcon, itsPolyIcon->GetBounds(), irect);
		}
	else if (fd.GetType()	== GLFitDescription::kNonLinear)
		{
		p.Image(*itsNonLinearIcon, itsNonLinearIcon->GetBounds(), irect);
		}
	else if (fd.GetType()	== GLFitDescription::kModule)
		{
		p.Image(*itsExecutableIcon, itsExecutableIcon->GetBounds(), irect);
		}
	else if (fd.GetType()	>= GLFitDescription::kBLinear)
		{
		p.Image(*itsBuiltInIcon, itsBuiltInIcon->GetBounds(), irect);
		}
	
	JRect r = rect;
	r.left += kHMarginWidth + kIconWidth;
	p.String(r, *curveName, JPainter::kHAlignLeft, JPainter::kVAlignCenter);
}

/******************************************************************************
 ApertureResized (virtual protected)

 ******************************************************************************/

void
GLFitDescriptionList::ApertureResized
	(
	const JCoordinate dw,
	const JCoordinate dh
	)
{
	JXEditTable::ApertureResized(dw,dh);
	AdjustColWidth();
}

/******************************************************************************
 AdjustColWidth (private)

 ******************************************************************************/

void
GLFitDescriptionList::AdjustColWidth()
{
	SetColWidth(1, JMax(itsMinColWidth, GetApertureWidth()));
}

/******************************************************************************
 CreateXInputField (virtual protected)

 ******************************************************************************/

JXInputField*
GLFitDescriptionList::CreateXInputField
	(
	const JPoint& 		cell,
	const JCoordinate 	x,
	const JCoordinate 	y,
	const JCoordinate 	w,
	const JCoordinate 	h
	)
{
	JTableSelection& s = GetTableSelection();
	s.ClearSelection();
	s.SelectRow(cell.y);
	Broadcast(FitSelected(cell.y));

	assert(itsInput == nullptr);
	itsInput = jnew JXInputField(this, kFixedLeft, kFixedTop, x, y, w, h);
	assert(itsInput != nullptr);

	itsInput->GetText()->SetText(*(itsNameList->GetElement(cell.y)));
	itsInput->SetIsRequired();
	return itsInput;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GLFitDescriptionList::PrepareDeleteXInputField()
{
	itsInput = nullptr;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

bool
GLFitDescriptionList::ExtractInputData
	(
	const JPoint& cell
	)
{
	const JString& name = itsInput->GetText()->GetText();
	if (!name.IsEmpty())
		{
		*(itsNameList->GetElement(cell.y)) = name;
		return true;
		}
	else
		{
		return false;
		}
}

/******************************************************************************
 HandleKeyPress (virtual public)

 ******************************************************************************/

void
GLFitDescriptionList::HandleKeyPress
	(
	const JUtf8Character&	c,
	const int 				keySym,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	JTableSelection& s = GetTableSelection();
	if (!s.GetFirstSelectedCell(&cell))
		{
		return;
		}

	if (c == kJUpArrow)
		{
		cell.y--;
		if (CellValid(cell))
			{
			BeginEditing(cell);
			}
		}
	else if (c == kJDownArrow)
		{
		cell.y++;
		if (CellValid(cell))
			{
			BeginEditing(cell);
			}
		}

	else
		{
		JXEditTable::HandleKeyPress(c, keySym, modifiers);
		}
}

/******************************************************************************
 GetCurrentFitIndex (public)

 ******************************************************************************/

bool
GLFitDescriptionList::GetCurrentFitIndex
	(
	JIndex* index
	)
{
	JPoint cell;
	if (GetTableSelection().GetSingleSelectedCell(&cell))
		{
		*index	= cell.y;
		return true;
		}
	return false;
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GLFitDescriptionList::Receive
	(
	JBroadcaster* 	sender, 
	const Message&	message
	)
{
	if (sender == GLGetFitManager() && message.Is(GLFitManager::kFitsChanged))
		{
		SyncWithManager();
		}
}

/******************************************************************************
 SyncWithManager (private)

 ******************************************************************************/

void
GLFitDescriptionList::SyncWithManager()
{
	RemoveAllRows();
	
	const JSize count = GLGetFitManager()->GetFitCount();
	AppendRows(count);

	itsNameList->DeleteAll();
	
	for (JIndex i=1; i<=count; i++)
		{
		const GLFitDescription& fd	= GLGetFitManager()->GetFitDescription(i);
		JString* str	= jnew JString(fd.GetFnName());
		assert(str != nullptr);
		itsNameList->Append(str);

		const JCoordinate width = 2*kHMarginWidth + kIconWidth + 
			JFontManager::GetDefaultFont().GetStringWidth(GetFontManager(), *str);
		if (width > itsMinColWidth)
			{
			itsMinColWidth = width;
			}
		}

	AdjustColWidth();
}
