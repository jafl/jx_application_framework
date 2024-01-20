/******************************************************************************
 TestImageDirector.cpp

	BASE CLASS = JXWindowDirector

	Written by John Lindal.

 ******************************************************************************/

#include "TestImageDirector.h"
#include "TestImageWidget.h"
#include <jx-af/jx/JXImageMask.h>
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXMenuBar.h>
#include <jx-af/jx/JXTextMenu.h>
#include <jx-af/jx/JXScrollbarSet.h>
#include <jx-af/jx/JXSelectionManager.h>
#include <jx-af/jx/JXImageSelection.h>
#include <jx-af/jx/JXPSPrinter.h>
#include <jx-af/jx/JXEPSPrinter.h>
#include <jx-af/jx/JXColorManager.h>
#include <jx-af/jx/JXChooseFileDialog.h>
#include <jx-af/jx/JXSaveFileDialog.h>
#include <jx-af/jcore/jFileUtil.h>
#include <jx-af/jcore/jGlobals.h>
#include <jx-af/jcore/jAssert.h>

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

	itsEPSPrinter = jnew JXEPSPrinter(display);
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

#include "TestImageDirector-File.h"

void
TestImageDirector::BuildWindow()
{
// begin JXLayout

	auto* window = jnew JXWindow(this, 400,330, JGetString("WindowTitle::TestImageDirector::JXLayout"));

	auto* menuBar =
		jnew JXMenuBar(window,
					JXWidget::kHElastic, JXWidget::kFixedTop, 0,0, 400,30);
	assert( menuBar != nullptr );

	auto* scrollbarSet =
		jnew JXScrollbarSet(window,
					JXWidget::kHElastic, JXWidget::kVElastic, 0,30, 400,300);
	assert( scrollbarSet != nullptr );

	itsImageWidget =
		jnew TestImageWidget(this, scrollbarSet, scrollbarSet->GetScrollEnclosure(),
					JXWidget::kHElastic, JXWidget::kVElastic, 0,0, 385,285);

// end JXLayout

	window->SetWMClass("testjx", "TestImageDirector");
	window->SetMinSize(100,100);

	itsFileMenu = menuBar->AppendTextMenu(JGetString("MenuTitle::TestImageDirector_File"));
	itsFileMenu->SetMenuItems(kFileMenuStr);
	itsFileMenu->AttachHandlers(this,
		&TestImageDirector::UpdateFileMenu,
		&TestImageDirector::HandleFileMenu);
	ConfigureFileMenu(itsFileMenu);
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
		itsPSPrinter->EditUserPageSetup();
	}
	else if (index == kPrintPSCmd)
	{
		if (itsPSPrinter->ConfirmUserPrintSetup())
		{
			PrintPS();
		}
	}

	else if (index == kPrintEPSCmd)
	{
		if (itsEPSPrinter->ConfirmUserPrintSetup())
		{
			PrintEPS();
		}
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
	auto* dlog = JXChooseFileDialog::Create();

	if (dlog->DoDialog())
	{
		itsImageWidget->SetImage(nullptr, true);	// free current colors

		itsFileName = dlog->GetFullName();

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
			itsImageWidget->SetImage(image, true);
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
	if (itsFileName.IsEmpty() || !itsImageWidget->GetImage(&image))
	{
		return;
	}

	JString root, suffix, name = itsFileName;
	JSplitRootAndSuffix(itsFileName, &root, &suffix);
	if (type == JImage::kGIFType)
	{
		name = JCombineRootAndSuffix(root, "gif");
	}
	else if (type == JImage::kPNGType)
	{
		name = JCombineRootAndSuffix(root, "png");
	}
	else if (type == JImage::kJPEGType)
	{
		name = JCombineRootAndSuffix(root, "jpg");
	}
	else if (type == JImage::kXPMType)
	{
		name = JCombineRootAndSuffix(root, "xpm");
	}

	auto* dlog = JXSaveFileDialog::Create(JGetString("SaveImagePrompt::TestImageDirector"), name);

	if (dlog->DoDialog())
	{
		const JString fullName = dlog->GetFullName();

		JError err = JNoError();
		if (type == JImage::kGIFType)
		{
			err = image->WriteGIF(fullName, false);
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
	JXImage* image;
	JXImageMask* mask;
	if (itsFileName.IsEmpty() ||
		!itsImageWidget->GetImage(&image) ||
		!image->GetMask(&mask))
	{
		return;
	}

	auto* dlog = JXSaveFileDialog::Create(JGetString("SaveMaskPrompt::TestImageDirector"), itsFileName + ".mask");

	if (dlog->DoDialog())
	{
		const JError err = mask->WriteXBM(dlog->GetFullName());
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
	itsImageWidget->SetImage(nullptr, true);	// free current colors

	JXImage* image = nullptr;
	if (JXImageSelection::GetImage(kJXClipboardName, CurrentTime, GetDisplay(), &image))
	{
		itsImageWidget->SetImage(image, true);
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
		itsPSPrinter->Image(*image, image->GetBounds(), 0,0);
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
