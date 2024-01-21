/******************************************************************************
 ImageWidget.cpp

	BASE CLASS = BaseWidget

	Copyright (C) 2023 by John Lindal.

 ******************************************************************************/

#include "ImageWidget.h"
#include "ImageChooserPanel.h"
#include "LayoutDocument.h"
#include "LayoutContainer.h"
#include "ImageCache.h"
#include "globals.h"
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/JXWindowPainter.h>
#include <jx-af/jx/jXPainterUtil.h>
#include <jx-af/jcore/jDirUtil.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

ImageWidget::ImageWidget
	(
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(false, layout, hSizing, vSizing, x,y, w,h)
{
}

ImageWidget::ImageWidget
	(
	std::istream&		input,
	const JFileVersion	vers,
	LayoutContainer*	layout,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	BaseWidget(input, vers, layout, hSizing, vSizing, x,y, w,h)
{
	JString fileName;
	bool isCoreImage;
	input >> isCoreImage >> fileName;

	if (!fileName.IsEmpty())
	{
		JString path;
		if (!isCoreImage)
		{
			LayoutDocument* doc = GetParentContainer()->GetDocument();
			assert( doc->ExistsOnDisk() );
			ImageCache::FindProjectRoot(doc->GetFilePath(), &path);
		}

		itsFullName = ImageCache::ConvertToFullName(path, fileName, isCoreImage);
	}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

ImageWidget::~ImageWidget()
{
}

/******************************************************************************
 StreamOut (virtual)

 ******************************************************************************/

void
ImageWidget::StreamOut
	(
	std::ostream& output
	)
	const
{
	output << JString("ImageWidget") << std::endl;

	BaseWidget::StreamOut(output);

	if (!itsFullName.IsEmpty())
	{
		JString p,n;
		JSplitPathAndName(itsFullName, &p, &n);

		output << p.StartsWith(JX_INCLUDE_PATH) << ' ' << n << std::endl;
	}
	else
	{
		output << false << ' ' << itsFullName << std::endl;
	}
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
ImageWidget::Draw
	(
	JXWindowPainter&	p,
	const JRect&		rect
	)
{
	JXImage* image;
	if (!itsFullName.IsEmpty() &&
		GetImageCache()->GetImage(itsFullName, &image))
	{
		p.Image(*image, image->GetBounds(), GetBounds());
	}
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
ImageWidget::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 GetClassName (virtual protected)

 ******************************************************************************/

JString
ImageWidget::GetClassName()
	const
{
	return "JXImageWidget";
}

/******************************************************************************
 PrintConfiguration (virtual protected)

 ******************************************************************************/

void
ImageWidget::PrintConfiguration
	(
	std::ostream&	output,
	const JString&	indent,
	const JString&	varName,
	JStringManager*	stringdb
	)
	const
{
	if (!itsFullName.IsEmpty())
	{
		const JString name = ImageCache::PrintInclude(itsFullName, output);

		indent.Print(output);
		varName.Print(output);
		output << "->SetXPM(";
		name.Print(output);
		output << ");" << std::endl;
	}
	else
	{
		BaseWidget::PrintConfiguration(output, indent, varName, stringdb);
	}
}

/******************************************************************************
 AddPanels (virtual protected)

 ******************************************************************************/

void
ImageWidget::AddPanels
	(
	WidgetParametersDialog* dlog
	)
{
	itsPanel = jnew ImageChooserPanel(dlog, GetParentContainer()->GetDocument(),
									  itsFullName);
}

/******************************************************************************
 SavePanelData (virtual protected)

 ******************************************************************************/

void
ImageWidget::SavePanelData()
{
	itsPanel->GetValues(&itsFullName);
	itsPanel = nullptr;
}
