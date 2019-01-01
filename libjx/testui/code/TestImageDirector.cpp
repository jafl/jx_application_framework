/******************************************************************************
 TestImageDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

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
#include <JXColorManager.h>
#include <jGlobals.h>
#include <jAssert.h>

// File menu information

static const JUtf8Byte* kFileMenuStr =
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
	itsPSPrinter  = nullptr;
	itsEPSPrinter = nullptr;

	BuildWindow();

	JXDisplay* display = GetDisplay();

	itsPSPrinter = jnew JXPSPrinter(display);
	assert( itsPSPrinter != nullptr );
	ListenTo(itsPSPrinter);

	itsEPSPrinter = jnew JXEPSPrinter(display);
	assert( itsEPSPrinter != nullptr );
	ListenTo(itsEPSPrinter);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

TestImageDirector::~TestImageDirector()
{
	jdelete itsPSPrinter;
	jdelete itsEPSPrinter;
}

/******************************************************************************
 BuildWindow (private)

 ******************************************************************************/

void
TestImageDirector::BuildWindow()
{
// begin JXLayout

	JXWindow* window = jnew JXWindow(this, 400,330, JString::empty);
	assert( window != nullptr );

	JXMenuBar* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	JXScrollbarSet* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != nullptr );

// end JXLayout

	window->SetTitle(JGetString("WindowTitle::TestImageDirector"));
	window->SetWMClass("testjx", "TestImageDirector");
	window->SetMinSize(100,100);

	itsFileMenu = menuBar->AppendTextMenu(JGetString("FileMenuTitle::TestImageDirector"));
	itsFileMenu->SetShortcuts(JGetString("FileMenuShortcut::TestImageDirector"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	ListenTo(itsFileMenu);

	itsImageWidget =
		jnew JXImageWidget(scrollbarSet, scrollbarSet->GetScrollEnclosure(),
						  JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 10,10);
	assert( itsImageWidget != nullptr );
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
			dynamic_cast<const JXMenu::ItemSelected*>(&message);
		assert( selection != nullptr );
		HandleFileMenu(selection->GetIndex());
		}

	else if (sender == itsPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
		if (info->Successful())
			{
			PrintPS();
			}
		}

	else if (sender == itsEPSPrinter &&
			 message.Is(JPrinter::kPrintSetupFinished))
		{
		const JPrinter::PrintSetupFinished* info =
			dynamic_cast<const JPrinter::PrintSetupFinished*>(&message);
		assert( info != nullptr );
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
	if (JGetChooseSaveFile()->ChooseFile(JGetString("ChooseImagePrompt::TestImageDirector"), JString::empty, &fullName))
		{
		itsImageWidget->SetImage(nullptr, kJTrue);	// free current colors

		itsFileName = fullName;

		JXDisplay* display = GetDisplay();

		JXImage* image;
		JError err = JNoError();
		if (JImage::GetFileType(itsFileName) == JImage::kXBMType)
			{
			JXImageMask* mask;
			err = JXImageMask::CreateFromXBM(display, itsFileName, &mask);

			if (err.OK())
				{
				image = jnew JXImage(display, mask->GetWidth(), mask->GetHeight(),
									JColorManager::GetRedColor());
				assert( image != nullptr );
				image->SetMask(mask);
				}
			}
		else
			{
			err = JXImage::CreateFromFile(display, itsFileName, &image);
			}

		if (err.OK())
			{
			itsImageWidget->SetImage(image, kJTrue);
			}
		else
			{
			itsFileName.Clear();
			JGetStringManager()->ReportError("UnableToLoadImage::TestImageDirector", err);
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
		JGetChooseSaveFile()->SaveFile(
			JGetString("SaveImagePrompt::TestImageDirector"),
			JString::empty, itsFileName, &fullName))
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

		JGetStringManager()->ReportError("UnableToSaveImage::TestImageDirector", err);
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
		JGetChooseSaveFile()->SaveFile(
			JGetString("SaveMaskPrompt::TestImageDirector"),
			JString::empty, fileName, &fullName))
		{
		const JError err = mask->WriteXBM(fullName);
		JGetStringManager()->ReportError("UnableToSaveMask::TestImageDirector", err);
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
		JXImageSelection* data = jnew JXImageSelection(*image);
		assert( data != nullptr );
		(GetDisplay()->GetSelectionManager())->SetData(kJXClipboardName, data);
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
	itsImageWidget->SetImage(nullptr, kJTrue);	// free current colors

	JXImage* image = nullptr;
	if (JXImageSelection::GetImage(kJXClipboardName, CurrentTime, GetDisplay(), &image))
		{
		itsImageWidget->SetImage(image, kJTrue);
		}
	else
		{
		JGetUserNotification()->ReportError(JGetString("UnrecognizedFormat::TestImageDirector"));
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
