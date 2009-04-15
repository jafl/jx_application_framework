/******************************************************************************
 ClipboardWidget.cpp

	This widget shows how to support copy and paste, using the X Selection
	mechanism encapsulated in JXSelectionManager.

	ConvertSelection() and Paste() do the real work.  In this simple case,
	we simply send and receive the buffer maintained by the JString.  In
	general, the safest way to send formatted data is in ASCII format.
	strstreams make it very easy to pack and unpack the data.

	BASE CLASS = JXWidget

	Written by Glenn Bach - 1998.

 ******************************************************************************/

#include <JXStdInc.h>
#include "ClipboardWidget.h"

#include <JXColormap.h>
#include <JXDisplay.h>
#include <JXMenuBar.h>
#include <JXSelectionManager.h>
#include <JXTextMenu.h>
#include <JXTextSelection.h>
#include <JXWidget.h>
#include <JXWindowPainter.h>

#include <jXPainterUtil.h>
#include <jXConstants.h>
#include <jXGlobals.h>
#include <jAssert.h>

// This defines the menu title and menu items
static const JCharacter* kEditMenuTitleStr = "Edit";
static const JCharacter* kEditMenuStr =
	"Copy %k Meta-C | Paste %k Meta-V";

enum
{
	kCopyCmd = 1,
	kPasteCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

ClipboardWidget::ClipboardWidget
	(
	const JCharacter* 	text,
	JXMenuBar* 			menuBar,
	JXContainer* 		enclosure,
	const HSizingOption hSizing,
	const VSizingOption vSizing,
	const JCoordinate 	x,
	const JCoordinate 	y,
	const JCoordinate 	w,
	const JCoordinate 	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x, y, w, h),
	itsText(text)
{
	// Set the background color.
	JXColormap* cmap = GetColormap();
	SetBackColor(cmap->GetWhiteColor());

    // Create the menu and attach it to the menu bar.
    itsEditMenu = menuBar->AppendTextMenu(kEditMenuTitleStr);

    // Set the menu items.
    itsEditMenu->SetMenuItems(kEditMenuStr);

    // The menu items don't need to be disabled
    itsEditMenu->SetUpdateAction(JXMenu::kDisableNone);

    // Listen for messages from the menu.
	ListenTo(itsEditMenu);

	// Register the data types that we support.  The atoms that we need
	// here, namely text atoms, are already defined, so we don't need to
	// define them again. If we had a special selection type we would use
	// GetDisplay()->RegisterXAtom(OurAtomString) to register it.
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ClipboardWidget::~ClipboardWidget()
{
	// There is nothing to delete.
	// itsEditMenu is deleted by the menu bar
}

/******************************************************************************
 Receive (virtual protected)

	Listen for menu selections and deactivated dialog windows.

 ******************************************************************************/

void
ClipboardWidget::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	// Check to see if the a menu item was selected.
	if (sender == itsEditMenu && message.Is(JXMenu::kItemSelected))
		{
		// Cast the sender so we can access its functions.
		 const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );

		// Handle the menu selection
		HandleEditMenu(selection->GetIndex());
		}
}

/******************************************************************************
 Draw

	This gets called by the event loop every time the ClipboardWidget
	needs to be redrawn.

	p is the Painter that you use to draw to the screen.  The main reason
	for Painter is to hide the system dependent details of drawing,
	and to provide a uniform interface for drawing to the screen, to an
	offscreen image, and to a printer.

	rect is the area that needs to be redrawn.  In simple cases, you
	can just draw everything.  For complex widgets, you often want to
	optimize and only draw the part inside rect.

 ******************************************************************************/

void
ClipboardWidget::Draw
	(
	JXWindowPainter& p,
	const JRect&     rect
	)
{
	// We need the colormap in order to specify colors.
	JXColormap* cmap = GetColormap();

	// This is where everything happens
	// See JPainter.h for available functions

	// This sets the color of the pen.
	p.SetPenColor(cmap->GetCyanColor());

	// This draws our rectangle.
	p.Rect(10, 10, 150, 50);

	// This draws itsText.
	p.String(20,20,itsText,
				130, JPainter::kHAlignCenter,
				30, JPainter::kVAlignCenter);
}

/******************************************************************************
 DrawBorder

	This gets called by the event loop every time the ClipboardWidget's
	border needs to be redrawn. Since we don't want a border, we leave it
	blank.

 ******************************************************************************/

void
ClipboardWidget::DrawBorder
	(
	JXWindowPainter& p,
	const JRect&     frame
	)
{
}

/******************************************************************************
 SetText

	This sets our text string.

 ******************************************************************************/

void
ClipboardWidget::SetText
	(
	const JCharacter* text
	)
{
	// Set our JString to the specified text.
	itsText = text;
}

/******************************************************************************
 GetText

	This retreives our text string.

 ******************************************************************************/

const JString&
ClipboardWidget::GetText()
	const
{
	return itsText;
}

/******************************************************************************
 HandleEditMenu

 ******************************************************************************/

void
ClipboardWidget::HandleEditMenu
	(
	const JIndex index
	)
{
	if (index == kCopyCmd)
		{
		// We instantiate a selection object that is appropriate for
		// our data. 
		JXTextSelection* data = new JXTextSelection(GetDisplay(), itsText);
		assert(data != NULL);

		// The selection data is then given to the selection manager.
		if (!(GetSelectionManager())->SetData(kJXClipboardName, data))
			{
			(JGetUserNotification())->ReportError("Unable to copy to the X Clipboard.");
			}
		}
	else if (index == kPasteCmd)
		{
		// Paste if the clipboard has the type we need.
		Paste();
		}
}

/******************************************************************************
 Paste

	We try to get either XA_STRING or TEXT.  TEXT is polymorphic, so we
	check the returned type and discard it if it is not XA_STRING.

 ******************************************************************************/

void
ClipboardWidget::Paste()
{
	// Get the window and selection manager for use below.
	JXWindow* window           = GetWindow();
	JXSelectionManager* selMgr = GetSelectionManager();

	// If the clipboard is not empty, retrieve the available types.
	JArray<Atom> typeList;
	if (selMgr->GetAvailableTypes(kJXClipboardName, CurrentTime, &typeList))
		{

		// Loop through the available types to see if the clipboard has
		// one that we want.
		const JSize typeCount = typeList.GetElementCount();
		for (JIndex i=1; i<=typeCount; i++)
			{
			const Atom atom = typeList.GetElement(i);

			// Check if the i-th type is one we can use.
			if (atom == XA_STRING || atom == selMgr->GetTextXAtom())
				{
				// Get the data of the appropriate type.
				unsigned char* data = NULL;
				JSize dataLength;
				Atom returnType;
				JXSelectionManager::DeleteMethod dMethod;
				if (selMgr->GetData(kJXClipboardName, CurrentTime,
						atom, &returnType, &data, &dataLength,
						&dMethod))
					{
					// We can only handle the simplest format.
					if (returnType == XA_STRING)
						{
						// Copy the data into our text.
						itsText.Set(reinterpret_cast<JCharacter*>(data), dataLength);

						// Our text changed, so we need to refresh.
						Refresh();
						}

					// This is required to delete the allocated data.
					// Forgetting to do this will cause a memory leak!
					selMgr->DeleteData(&data, dMethod);

					if (returnType == XA_STRING)
						{
						// We succeeded, so we return.
						return;
						}
					}
				else
					{
					(JGetUserNotification())->ReportError(
						"Unable to retrieve text from the clipboard.");
					}
				}
			}

		// If we got this far, the data type that we want wasn't on the
		// clipboard.
		(JGetUserNotification())->ReportError("Unable to paste from clipboard.");
		}
	else
		{
		// There isn't anything on the clipboard.
		(JGetUserNotification())->ReportError("Clipboard is empty.");
		}
}
