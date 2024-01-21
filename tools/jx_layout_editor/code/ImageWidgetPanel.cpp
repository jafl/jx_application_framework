/******************************************************************************
 ImageWidgetPanel.cpp

	BASE CLASS = WidgetPanelBase, JBroadcaster

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ImageWidgetPanel.h"
#include "WidgetParametersDialog.h"
#include "LayoutDocument.h"
#include "ImageCache.h"
#include "globals.h"
#include <jx-af/jx/JXWindow.h>
#include <jx-af/jx/JXWidgetSet.h>
#include <jx-af/jx/JXStaticText.h>
#include <jx-af/jx/JXImageMenu.h>
#include <jx-af/jcore/JRegex.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ImageWidgetPanel::ImageWidgetPanel
	(
	WidgetParametersDialog* dlog,

	LayoutDocument*	doc,
	const JString&	fullName
	)
	:
	itsImageIndex(1)
{
	itsImagePathList = jnew JPtrArray<JString>(JPtrArrayT::kDeleteAll);

	BuildPanel(dlog, doc, fullName);
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ImageWidgetPanel::~ImageWidgetPanel()
{
	jdelete itsImagePathList;
}

/******************************************************************************
 BuildPanel (private)

 ******************************************************************************/

static const JRegex excludeIconPattern("_busy|_selected|_pushed");

void
ImageWidgetPanel::BuildPanel
	(
	WidgetParametersDialog* dlog,

	LayoutDocument*	doc,
	const JString&	fullName
	)
{
	JXWindow* window = dlog->GetWindow();

// begin Panel

	auto* container =
		jnew JXWidgetSet(window,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 0,0, 460,100);
	assert( container != nullptr );

	auto* imageMenuLabel =
		jnew JXStaticText(JGetString("imageMenuLabel::ImageWidgetPanel::Panel"), container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 20,10, 50,20);
	assert( imageMenuLabel != nullptr );
	imageMenuLabel->SetToLabel();

	itsImageMenu =
		jnew JXImageMenu(JString::empty, 10, container,
					JXWidget::kFixedLeft, JXWidget::kFixedTop, 70,10, 100,80);
	assert( itsImageMenu != nullptr );

// end Panel

	dlog->AddPanel(this, container);

	JString path;
	if (doc->ExistsOnDisk())
	{
		ImageCache::FindProjectRoot(doc->GetFilePath(), &path);
	}

	GetImageCache()->BuildIconMenu(path, 0, &excludeIconPattern, false,
									itsImageMenu, itsImagePathList);

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
 GetValues

 ******************************************************************************/

void
ImageWidgetPanel::GetValues
	(
	JString* fullName
	)
{
	*fullName = *itsImagePathList->GetItem(itsImageIndex);
}
