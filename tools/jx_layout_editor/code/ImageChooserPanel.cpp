/******************************************************************************
 ImageChooserPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ImageChooserPanel.h"
#include "WidgetParametersDialog.h"
#include "LayoutDocument.h"
#include "ImageCache.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXImageMenu.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

static const JRegex excludeIconPattern("_busy|_selected|_pushed");

/******************************************************************************
 Constructor

 ******************************************************************************/

ImageChooserPanel::ImageChooserPanel
	(
	WidgetParametersDialog* dlog,

	LayoutDocument*	doc,
	const JString&	fullName
	)
	:
	itsImagePathList(jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll)),
	itsImageIndex(1)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,100);
	assert( container != nullptr );

	auto* imageMenuLabel =
		jnew JXStaticText(JGetString("imageMenuLabel::ImageChooserPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 50,20);
	imageMenuLabel->SetToLabel(false);

	itsImageMenu =
		jnew JXImageMenu(JGetString("itsImageMenu::ImageChooserPanel::Panel"), 10, container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,10, 100,80);

// end Panel

	dlog->AddPanel(this, container);

	JString path;
	if (doc->ExistsOnDisk())
	{
		ImageCache::FindProjectRoot(doc->GetFilePath(), &path);
	}

	GetImageCache()->BuildIconMenu(path, 0, &excludeIconPattern, true,
									itsImageMenu, itsImagePathList);

	const JSize count = itsImagePathList->GetItemCount();
	for (JIndex i=1; i<=count; i++)
	{
		if (*itsImagePathList->GetItem(i) == fullName)
		{
			itsImageIndex = i+1;
			break;
		}
	}

	itsImageMenu->SetUpdateAction(JXMenu::kDisableNone);
	itsImageMenu->SetToPopupChoice(true, itsImageIndex);

	ListenTo(itsImageMenu, std::function([this](const JXMenu::ItemSelected& msg)
	{
		itsImageIndex = msg.GetIndex();
		itsImageMenu->SetTitleImage(
			const_cast<JXImage*>(itsImageMenu->GetItemImage(itsImageIndex)), false);
	}));
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ImageChooserPanel::~ImageChooserPanel()
{
	jdelete itsImagePathList;
}

/******************************************************************************
 GetValues

 ******************************************************************************/

void
ImageChooserPanel::GetValues
	(
	JString* fullName
	)
{
	if (itsImageIndex > 1)
	{
		*fullName = *itsImagePathList->GetItem(itsImageIndex-1);
	}
	else
	{
		fullName->Clear();
	}
}

/******************************************************************************
 ReadImageName (static)

 ******************************************************************************/

JString
ImageChooserPanel::ReadImageName
	(
	std::istream&	input,
	LayoutDocument*	doc
	)
{
	JString fileName;
	bool isCoreImage;
	input >> isCoreImage >> fileName;

	if (fileName.IsEmpty())
	{
		return fileName;
	}

	JString path;
	if (!isCoreImage)
	{
		assert( doc->ExistsOnDisk() );
		ImageCache::FindProjectRoot(doc->GetFilePath(), &path);
	}

	return ImageCache::ConvertToFullName(path, fileName, isCoreImage);
}

/******************************************************************************
 WriteImageName (static)

 ******************************************************************************/

void
ImageChooserPanel::WriteImageName
	(
	const JString&	fullName,
	std::ostream&	output
	)
{
	if (!fullName.IsEmpty())
	{
		JString p,n;
		JSplitPathAndName(fullName, &p, &n);

		output << p.StartsWith(JX_INCLUDE_PATH) << ' ' << n << std::endl;
	}
	else
	{
		output << false << ' ' << fullName << std::endl;
	}
}
