/******************************************************************************
 TestImageWidget.cpp

	BASE CLASS = JXImageWidget

	Written by John Lindal.

 ******************************************************************************/

#include "TestImageWidget.h"
#include "TestImageDirector.h"
#include <jx-af/jx/JXDisplay.h>
#include <jx-af/jx/JXSelectionManager.h>
#include <jx-af/jx/JXDNDManager.h>
#include <jx-af/jx/JXImage.h>
#include <jx-af/jx/jXUtil.h>
#include <jx-af/jcore/JError.h>
#include <jx-af/jcore/jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

TestImageWidget::TestImageWidget
	(
	TestImageDirector*	director,
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
	JXImageWidget(scrollbarSet, enclosure, hSizing, vSizing, x,y, w,h),
	itsDirector(director)
{
	const JUtf8Byte* kAtomNames[ JXImageSelection::kAtomCount ] =
	{
		JXImageSelection::GetXPMXAtomName(),
		JXImageSelection::GetGIFXAtomName(),
		JXImageSelection::GetPNGXAtomName(),
		JXImageSelection::GetJPEGXAtomName()
	};

	GetDisplay()->RegisterXAtoms(JXImageSelection::kAtomCount, kAtomNames, itsAtoms);
}

/******************************************************************************
 WillAcceptDrop (virtual protected)

	We accept all copy drops so we can print out the data types.

 ******************************************************************************/

bool
TestImageWidget::WillAcceptDrop
	(
	const JArray<Atom>&	typeList,
	Atom*				action,
	const JPoint&		pt,
	const Time			time,
	const JXWidget*		source
	)
{
	*action = GetDNDManager()->GetDNDActionPrivateXAtom();

	const Atom urlXAtom = GetSelectionManager()->GetURLXAtom();

	for (const auto type : typeList)
	{
		if (type == urlXAtom)
		{
			return true;
		}
	}

	return false;
}

/******************************************************************************
 HandleDNDDrop (virtual protected)

	This is called when the data is dropped.  The data is accessed via
	the selection manager, just like Paste.

 ******************************************************************************/

void
TestImageWidget::HandleDNDDrop
	(
	const JPoint&		pt,
	const JArray<Atom>&	typeList,
	const Atom			action,
	const Time			time,
	const JXWidget*		source
	)
{
	JXSelectionManager* selMgr = GetSelectionManager();

	Atom returnType;
	unsigned char* data;
	JSize dataLength;
	JXSelectionManager::DeleteMethod delMethod;
	if (!selMgr->GetData(GetDNDManager()->GetDNDSelectionName(),
						 time, selMgr->GetURLXAtom(),
						 &returnType, &data, &dataLength, &delMethod))
	{
		return;
	}

	if (returnType != selMgr->GetURLXAtom())
	{
		selMgr->DeleteData(&data, delMethod);
		return;
	}

	JPtrArray<JString> fileNameList(JPtrArrayT::kDeleteAll),
					   urlList(JPtrArrayT::kDeleteAll);
	JXUnpackFileNames((char*) data, dataLength, &fileNameList, &urlList);

	if (!fileNameList.IsEmpty())
	{
		JXImage* image;
		const JString& file = *fileNameList.GetFirstElement();
		const JError err    = JXImage::CreateFromFile(GetDisplay(), file, &image);
		if (err.OK())
		{
			SetImage(image, true);
			itsDirector->SetFileName(file);
		}
	}

	selMgr->DeleteData(&data, delMethod);
}
