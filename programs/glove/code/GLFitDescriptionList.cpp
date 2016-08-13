/******************************************************************************
 GLFitDescriptionList.cpp

	BASE CLASS = public JXEditTable

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GLFitDescriptionList.h>

#include "GLFitManager.h"
#include "GLGlobals.h"

#include "glBuiltInFit.xpm"
#include "glNonLinearFit.xpm"
#include "glPolyFit.xpm"

#include <J2DPlotWidget.h>

#include <JXColormap.h>
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

const JCharacter* GLFitDescriptionList::kFitSelected 	= "GLFitDescriptionList::kFitSelected";
const JCharacter* GLFitDescriptionList::kFitInitiated	= "GLFitDescriptionList::kFitInitiated";;

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
	itsInput(NULL)
{
	itsMinColWidth = 1;

	const JFontManager* fontMgr = GetFontManager();
	const JSize rowHeight = 2*kVMarginWidth +
		fontMgr->GetLineHeight(JGetDefaultFontName(), kJDefaultFontSize, JFontStyle());
	SetDefaultRowHeight(rowHeight);

	const JSize count = GetFitManager()->GetFitCount();

	itsNameList = new JPtrArray<JString>(JPtrArrayT::kDeleteAll);
	assert(itsNameList != NULL);

	AppendCols(1);
	SyncWithManager();

	JXColormap* colormap = GetColormap();
	SetRowBorderInfo(0, colormap->GetBlackColor());
	SetColBorderInfo(0, colormap->GetBlackColor());

	itsBuiltInIcon	= new JXImage(GetDisplay(), JXPM(glBuiltInFit));
	assert(itsBuiltInIcon != NULL);
	itsBuiltInIcon->ConvertToRemoteStorage();

	itsNonLinearIcon	= new JXImage(GetDisplay(), JXPM(glNonLinearFit));
	assert(itsNonLinearIcon != NULL);
	itsNonLinearIcon->ConvertToRemoteStorage();

	itsPolyIcon	= new JXImage(GetDisplay(), JXPM(glPolyFit));
	assert(itsPolyIcon != NULL);
	itsPolyIcon->ConvertToRemoteStorage();

	itsExecutableIcon = new JXImage(GetDisplay(), JXPM(jx_executable_small));
	assert( itsExecutableIcon != NULL );
	itsExecutableIcon->ConvertToRemoteStorage();

	ListenTo(GetFitManager());
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GLFitDescriptionList::~GLFitDescriptionList()
{
	itsNameList->DeleteAll();
	delete itsNameList;		// we don't own the strings
	delete itsBuiltInIcon;
	delete itsNonLinearIcon;
	delete itsPolyIcon;
	delete itsExecutableIcon;
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

	const JString* curveName = itsNameList->NthElement(cell.y);

	const GLFitDescription& fd	= GetFitManager()->GetFitDescription(cell.y);

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

	assert(itsInput == NULL);
	itsInput = new JXInputField(this, kFixedLeft, kFixedTop, x, y, w, h);
	assert(itsInput != NULL);

	itsInput->SetText(*(itsNameList->NthElement(cell.y)));
	itsInput->SetIsRequired();
	return itsInput;
}

/******************************************************************************
 PrepareDeleteXInputField (virtual protected)

 ******************************************************************************/

void
GLFitDescriptionList::PrepareDeleteXInputField()
{
	itsInput = NULL;
}

/******************************************************************************
 ExtractInputData (virtual protected)

 ******************************************************************************/

JBoolean
GLFitDescriptionList::ExtractInputData
	(
	const JPoint& cell
	)
{
	const JString& name = itsInput->GetText();
	if (!name.IsEmpty())
		{
		*(itsNameList->NthElement(cell.y)) = name;
		return kJTrue;
		}
	else
		{
		return kJFalse;
		}
}

/******************************************************************************
 HandleKeyPress (virtual public)

 ******************************************************************************/

void
GLFitDescriptionList::HandleKeyPress
	(
	const int 				key,
	const JXKeyModifiers&	modifiers
	)
{
	JPoint cell;
	JTableSelection& s = GetTableSelection();
	if (!s.GetFirstSelectedCell(&cell))
		{
		return;
		}

	if (key == kJUpArrow)
		{
		cell.y--;
		if (CellValid(cell))
			{
			BeginEditing(cell);
			}
		}
	else if (key == kJDownArrow)
		{
		cell.y++;
		if (CellValid(cell))
			{
			BeginEditing(cell);
			}
		}

	else
		{
		JXEditTable::HandleKeyPress(key, modifiers);
		}
}

/******************************************************************************
 GetCurrentFitIndex (public)

 ******************************************************************************/

JBoolean
GLFitDescriptionList::GetCurrentFitIndex
	(
	JIndex* index
	)
{
	JPoint cell;
	if (GetTableSelection().GetSingleSelectedCell(&cell))
		{
		*index	= cell.y;
		return kJTrue;
		}
	return kJFalse;
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
	if (sender == GetFitManager() && message.Is(GLFitManager::kFitsChanged))
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
	
	const JSize count = GetFitManager()->GetFitCount();
	AppendRows(count);

	itsNameList->DeleteAll();
	
	for (JIndex i=1; i<=count; i++)
		{
		const GLFitDescription& fd	= GetFitManager()->GetFitDescription(i);
		JString* str	= new JString(fd.GetFnName());
		assert(str != NULL);
		itsNameList->Append(str);

		const JCoordinate width = 2*kHMarginWidth + kIconWidth + 
			GetFontManager()->GetStringWidth(JGetDefaultFontName(), kJDefaultFontSize,
									JFontStyle(), *str);
		if (width > itsMinColWidth)
			{
			itsMinColWidth = width;
			}
		}

	AdjustColWidth();
}
