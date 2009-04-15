/******************************************************************************
 TestImageDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include <JXStdInc.h>
#include "TestImageDirector.h"
#include <JXImageMask.h>
#include <JXDisplay.h>
#include <JXWindow.h>
#include <JXMenuBar.h>
#include <JXTextMenu.h>
#include <JXScrollbarSet.h>
#include <JXImageWidget.h>
#include <JXSelectionManager.h>
#include <JXImageSelection.h>
#include <JXPSPrinter.h>
#include <JXEPSPrinter.h>
#include <JXColormap.h>
#include <jGlobals.h>
#include <jAssert.h>

// File menu information

static const JCharacter* kFileMenuTitleStr  = "File";
static const JCharacter* kFileMenuShortcuts = "#F";
static const JCharacter* kFileMenuStr =
	"    Open...             %h o %k Ctrl-O"
	"  | Save as GIF...      %h g %k Ctrl-G"
	"  | Save as PNG...      %h n %k Ctrl-N"
	"  | Save as JPEG...     %h j %k Ctrl-J"
	"  | Save as XPM...      %h x %k Ctrl-X"
	"  | Save mask as XBM... %h b %k Ctrl-B"
	"%l| Copy                     %k Ctrl-C"
	"  | Paste                    %k Ctrl-V"
	"%l| Page setup..."
	"  | Print PostScript... %h p %k Ctrl-P"
	"%l| Print EPS..."
	"%l| Close               %h c %k Ctrl-W";

enum
{
	kOpenImageCmd = 1,
	kSaveGIFCmd, kSavePNGCmd, kSaveJPEGCmd,
	kSaveXPMCmd, kSaveMaskXBMCmd,
	kCopyImageCmd, kPasteImageCmd,
	kPageSetupCmd, kPrintPSCmd,
	kPrintEPSCmd,
	kCloseCmd
};

/******************************************************************************
 Constructor

 ******************************************************************************/

TestImageDirector::TestImageDirector
	(
	JXDirector* supervisor
	)
	:
	JXWindowDirector(supervisor)
{
	itsPSPrinter  = NULL;
	itsEPSPrinter = NULL;

	BuildWindow();

	JXDisplay* display = GetDisplay();

	itsPSPrinter = new JXPSPrinter(display, (GetWindow())->GetColormap());
	assert( itsPSPrinter != NULL );
	ListenTo(itsPSPrinter);

	itsEPSPrinter = new JXEPSPrinter(display, (GetWindow())->GetColormap());
	assert( itsEPSPrinter != NULL );
	ListenTo(itsEPSPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestImageDirector::~TestImageDirector()
{
	delete itsPSPrinter;
	delete itsEPSPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestImageDirector::BuildWindow()
{
// begin JXLayout

    JXWindow* window = new JXWindow(this, 400,330, "");
    assert( window != NULL );
    SetWindow(window);

    JXMenuBar* menuBar =
        new JXMenuBar(window,
                    JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
    assert( menuBar != NULL );

    JXScrollbarSet* scrollbarSet =
        new JXScrollbarSet(window,
                    JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
    assert( scrollbarSet != NULL );

// end JXLayout

	window->SetTitle("Test Image");
	window->SetWMClass("testjx", "TestImageDirector");
	window->SetMinSize(100,100);

	itsFileMenu = menuBar->AppendTextMenu(kFileMenuTitleStr);
	itsFileMenu->SetShortcuts(kFileMenuShortcuts);
	itsFileMenu->SetMenuItems(kFileMenuStr);
	ListenTo(itsFileMenu);

	itsImageWidget =
		new JXImageWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsImageWidget != NULL );
	itsImageWidget->FitToEnclosure();
}

/******************************************************************************
 Receive (protected)

 ******************************************************************************/

void
TestImageDirector::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsFileMenu && message.Is(JXMenu::kNeedsUpdate))
		{
		UpdateFileMenu();
		}
	else if (sender == itsFileMenu && message.Is(JXMenu::kItemSelected))
		{
		const JXMenu::ItemSelected* selection =
			dynamic_cast(const JXMenu::ItemSelected*, &message);
		assert( selection != NULL );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			PrintPS();
			}
		}

	else if (sender == itsEPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast(const JPrinter::PrintSetupFinished*, &message);
		assert( info != NULL );
		if (info->Successful())
			{
			PrintEPS();
			}
		}

	else
		{
		JXWindowDirector::Receive(sender, message);
		}
}

/******************************************************************************
 UpdateFileMenu (private)

 ******************************************************************************/

void
TestImageDirector::UpdateFileMenu()
{
	itsFileMenu->EnableItem(kOpenImageCmd);
	itsFileMenu->EnableItem(kPasteImageCmd);
	itsFileMenu->EnableItem(kPageSetupCmd);
	itsFileMenu->EnableItem(kCloseCmd);

	JXImage* image;
	if (itsImageWidget->GetImage(&image))
		{
		itsFileMenu->EnableItem(kSaveGIFCmd);
		itsFileMenu->EnableItem(kSavePNGCmd);
		itsFileMenu->EnableItem(kSaveJPEGCmd);
		itsFileMenu->EnableItem(kSaveXPMCmd);
		itsFileMenu->EnableItem(kCopyImageCmd);
		itsFileMenu->EnableItem(kPrintPSCmd);
		itsFileMenu->EnableItem(kPrintEPSCmd);

		if (image->HasMask())
			{
			itsFileMenu->EnableItem(kSaveMaskXBMCmd);
			}
		}
}

/******************************************************************************
 HandleFileMenu (private)

 ******************************************************************************/

void
TestImageDirector::HandleFileMenu
	(
	const JIndex index
	)
{
	if (index == kOpenImageCmd)
		{
		LoadImage();
		}
	else if (index == kSaveGIFCmd)
		{
		SaveImage(JImage::kGIFType);
		}
	else if (index == kSavePNGCmd)
		{
		SaveImage(JImage::kPNGType);
		}
	else if (index == kSaveJPEGCmd)
		{
		SaveImage(JImage::kJPEGType);
		}
	else if (index == kSaveXPMCmd)
		{
		SaveImage(JImage::kXPMType);
		}
	else if (index == kSaveMaskXBMCmd)
		{
		SaveMask();
		}

	else if (index == kCopyImageCmd)
		{
		CopyImage();
		}
	else if (index == kPasteImageCmd)
		{
		PasteImage();
		}

	else if (index == kPageSetupCmd)
		{
		itsPSPrinter->BeginUserPageSetup();
		}
	else if (index == kPrintPSCmd)
		{
		itsPSPrinter->BeginUserPrintSetup();
		}

	else if (index == kPrintEPSCmd)
		{
		itsEPSPrinter->BeginUserPrintSetup();
		}

	else if (index == kCloseCmd)
		{
		Close();
		}
}

/******************************************************************************
 LoadImage (private)

	A real program would find a way to restore the existing image
	if the new image couldn't be loaded.

 ******************************************************************************/

void
TestImageDirector::LoadImage()
{
	JString fullName;
	if ((JGetChooseSaveFile())->ChooseFile("Image to load:", NULL, &fullName))
		{
		itsImageWidget->SetImage(NULL, kJTrue);	// free current colors

		itsFileName = fullName;

		JXDisplay* display = GetDisplay();
		JXColormap* cmap   = GetColormap();

		JXImage* image;
		JError err = JNoError();
		if (JImage::GetFileType(itsFileName) == JImage::kXBMType)
			{
			JXImageMask* mask;
			err = JXImageMask::CreateFromXBM(display, cmap,
											 itsFileName, &mask);

			if (err.OK())
				{
				image = new JXImage(display, cmap,
									mask->GetWidth(), mask->GetHeight(),
									cmap->GetRedColor());
				assert( image != NULL );
				image->SetMask(mask);
				}
			}
		else
			{
			err = JXImage::CreateFromFile(display, cmap,
										  itsFileName, &image);
			}

		if (err.OK())
			{
			itsImageWidget->SetImage(image, kJTrue);
			}
		else
			{
			itsFileName.Clear();

			JString msg = "Unable to open the file because\n\n";
			msg += err.GetMessage();
			(JGetUserNotification())->ReportError(msg);
			}
		}
}

/******************************************************************************
 SaveImage (private)

	We really ought to provide a menu inside the Save File window
	to select the image format.

 ******************************************************************************/

void
TestImageDirector::SaveImage
	(
	const JImage::FileType type
	)
	const
{
	JXImage* image;
	JString fullName;
	if (!itsFileName.IsEmpty() &&
		itsImageWidget->GetImage(&image) &&
		(JGetChooseSaveFile())->SaveFile("Save image as:", NULL, itsFileName, &fullName))
		{
		JError err = JNoError();
		if (type == JImage::kGIFType)
			{
			err = image->WriteGIF(fullName, kJFalse);
			}
		else if (type == JImage::kPNGType)
			{
			err = image->WritePNG(fullName);
			}
		else if (type == JImage::kJPEGType)
			{
			err = image->WriteJPEG(fullName);
			}
		else if (type == JImage::kXPMType)
			{
			err = image->WriteXPM(fullName);
			}

		if (!err.OK())
			{
			JString msg = "Unable to save the image because\n\n";
			msg += err.GetMessage();
			(JGetUserNotification())->ReportError(msg);
			}
		}
}

/******************************************************************************
 SaveMask (private)

	Saves the image's mask as an XBM file.

 ******************************************************************************/

void
TestImageDirector::SaveMask()
	const
{
	const JString fileName = itsFileName + ".mask";

	JXImage* image;
	JXImageMask* mask;
	JString fullName;
	if (!itsFileName.IsEmpty() &&
		itsImageWidget->GetImage(&image) &&
		image->GetMask(&mask) &&
		(JGetChooseSaveFile())->SaveFile("Save mask as:", NULL, fileName, &fullName))
		{
		const JError err = mask->WriteXBM(fullName);
		if (!err.OK())
			{
			JString msg = "Unable to save the mask because\n\n";
			msg += err.GetMessage();
			(JGetUserNotification())->ReportError(msg);
			}
		}
}

/******************************************************************************
 CopyImage (private)

 ******************************************************************************/

void
TestImageDirector::CopyImage()
	const
{
	JXImage* image;
	if (itsImageWidget->GetImage(&image))
		{
		JXImageSelection* data = new JXImageSelection(*image);
		assert( data != NULL );
		((GetDisplay())->GetSelectionManager())->SetData(kJXClipboardName, data);
		}
}

/******************************************************************************
 PasteImage (private)

	A real program would find a way to restore the existing image
	if the new image couldn't be pasted.

 ******************************************************************************/

void
TestImageDirector::PasteImage()
{
	itsImageWidget->SetImage(NULL, kJTrue);	// free current colors

	JXImage* image = NULL;
	if (JXImageSelection::GetImage(kJXClipboardName, CurrentTime, GetDisplay(), &image))
		{
		itsImageWidget->SetImage(image, kJTrue);
		}
	else
		{
		(JGetUserNotification())->ReportError(
			"Unable to find a known image format.");
		}
}

/******************************************************************************
 PrintPS (private)

	A real program would paginate the picture if it doesn't fit on one page.

 ******************************************************************************/

void
TestImageDirector::PrintPS()
	const
{
	JXImage* image;
	if (itsImageWidget->GetImage(&image) &&
		itsPSPrinter->OpenDocument() &&
		itsPSPrinter->NewPage())
		{
		itsPSPrinter->JPainter::Image(*image, image->GetBounds(), 0,0);
		itsPSPrinter->CloseDocument();
		}
}

/*****************************************************************************
 PrintEPS (private)

 ******************************************************************************/

void
TestImageDirector::PrintEPS()
	const
{
	JXImage* image;
	if (itsImageWidget->GetImage(&image))
		{
		const JRect bounds = image->GetBounds();

		if (itsEPSPrinter->WantsPreview())
			{
			JPainter& p = itsEPSPrinter->GetPreviewPainter(bounds);
			p.Image(*image, bounds, bounds);
			}

		if (itsEPSPrinter->OpenDocument(bounds))
			{
			itsEPSPrinter->Image(*image, bounds, bounds);
			itsEPSPrinter->CloseDocument();
			}
		}
}
