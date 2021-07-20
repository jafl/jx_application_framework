/******************************************************************************
 JXImageSelection.cpp

	image/xpm, image/gif, image/png, image/jpeg

	BASE CLASS = JXSelectionData

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include "JXImageSelection.h"
#include "JXDisplay.h"
#include "JXSelectionManager.h"
#include "JXImage.h"
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <stdio.h>
#include <jAssert.h>

static const JUtf8Byte* kAtomNames[ JXImageSelection::kAtomCount ] =
{
	"image/xpm",
	"image/gif",
	"image/png",
	"image/jpeg"
};

/******************************************************************************
 Constructor

 ******************************************************************************/

JXImageSelection::JXImageSelection
	(
	const JXImage& image
	)
	:
	JXSelectionData(image.GetDisplay())
{
	itsImage = nullptr;
	SetData(image);
}

JXImageSelection::JXImageSelection
	(
	JXImage* image
	)
	:
	JXSelectionData(image->GetDisplay())
{
	itsImage = nullptr;
	SetData(image);
}

JXImageSelection::JXImageSelection
	(
	JXWidget*			widget,
	const JUtf8Byte*	id
	)
	:
	JXSelectionData(widget, id)
{
	itsImage = nullptr;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JXImageSelection::~JXImageSelection()
{
	jdelete itsImage;
}

/******************************************************************************
 AddTypes (virtual protected)

 ******************************************************************************/

void
JXImageSelection::AddTypes
	(
	const Atom selectionName
	)
{
	GetDisplay()->RegisterXAtoms(kAtomCount, kAtomNames, itsAtoms);

	itsXPMAtom = itsAtoms[ kXPMAtomIndex ];
	AddType(itsXPMAtom);

	itsGIFAtom = itsAtoms[ kGIFAtomIndex ];
	AddType(itsGIFAtom);

	itsPNGAtom = itsAtoms[ kPNGAtomIndex ];
	AddType(itsPNGAtom);

	itsJPEGAtom = itsAtoms[ kJPEGAtomIndex ];
	AddType(itsJPEGAtom);
}

/******************************************************************************
 SetData

	Makes a copy of the image.

 ******************************************************************************/

void
JXImageSelection::SetData
	(
	const JXImage& image
	)
{
	auto* copy = jnew JXImage(image);
	assert( copy != nullptr );
	SetData(copy);
}

/******************************************************************************
 SetData

	Takes ownership of image.

 ******************************************************************************/

void
JXImageSelection::SetData
	(
	JXImage* image
	)
{
	if (itsImage != nullptr)
		{
		jdelete itsImage;
		}

	itsImage = image;
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

bool
JXImageSelection::ConvertData
	(
	const Atom		requestType,
	Atom*			returnType,
	unsigned char**	data,
	JSize*			dataLength,
	JSize*			bitsPerBlock
	)
	const
{
	*data         = nullptr;
	*dataLength   = 0;
	*returnType   = None;
	*bitsPerBlock = 8;

	if (itsImage == nullptr)
		{
		return false;
		}

	JString fileName;
	if (!(JCreateTempFile(&fileName)).OK())
		{
		return false;
		}

	JError err = JUnknownError(1);
	if (requestType == itsXPMAtom)
		{
		err = itsImage->WriteXPM(fileName);
		}
	else if (requestType == itsGIFAtom)
		{
		err = itsImage->WriteGIF(fileName, false);	// if too many colors, use PNG
		}
	else if (requestType == itsPNGAtom)
		{
		err = itsImage->WritePNG(fileName);
		}
	else if (requestType == itsJPEGAtom)
		{
		err = itsImage->WriteJPEG(fileName);
		}

	if (err.OK())
		{
		JString imageData;
		JReadFile(fileName, &imageData);
		JRemoveFile(fileName);

		*returnType = requestType;
		*dataLength = imageData.GetByteCount();
		*data       = (unsigned char*) imageData.AllocateBytes();
		return true;
		}

	JRemoveFile(fileName);
	return false;
}

/******************************************************************************
 Get X atom names (static)

 ******************************************************************************/

const JUtf8Byte*
JXImageSelection::GetXPMXAtomName()
{
	return kAtomNames[ kXPMAtomIndex ];
}

const JUtf8Byte*
JXImageSelection::GetGIFXAtomName()
{
	return kAtomNames[ kGIFAtomIndex ];
}

const JUtf8Byte*
JXImageSelection::GetPNGXAtomName()
{
	return kAtomNames[ kPNGAtomIndex ];
}

const JUtf8Byte*
JXImageSelection::GetJPEGXAtomName()
{
	return kAtomNames[ kJPEGAtomIndex ];
}

/******************************************************************************
 GetImage (static)

 ******************************************************************************/

bool
JXImageSelection::GetImage
	(
	const Atom		selectionName,
	const Time		time,
	JXDisplay*		display,
	JXImage**		image
	)
{
	return GetImage(display->GetSelectionManager(), selectionName, time, image);
}

bool
JXImageSelection::GetImage
	(
	JXSelectionManager* selMgr,
	const Atom			selectionName,
	const Time			time,
	JXImage**			image
	)
{
	JXDisplay* display = selMgr->GetDisplay();
	Atom atoms[ kAtomCount ];
	display->RegisterXAtoms(kAtomCount, kAtomNames, atoms);

	JArray<Atom> typeList;
	if (selMgr->GetAvailableTypes(selectionName, time, &typeList))
		{
		bool xpm=false, gif=false, png=false, jpeg=false;

		for (const Atom type : typeList)
			{
			xpm  = xpm  || type == atoms[ kXPMAtomIndex ];
			gif  = gif  || type == atoms[ kGIFAtomIndex ];
			png  = png  || type == atoms[ kPNGAtomIndex ];
			jpeg = jpeg || type == atoms[ kJPEGAtomIndex ];
			}

		while (xpm || gif || png || jpeg)
			{
			Atom type = None;
			if (png)
				{
				png  = false;
				type = atoms[ kPNGAtomIndex ];
				}
			else if (gif)
				{
				gif  = false;
				type = atoms[ kGIFAtomIndex ];
				}
			else if (xpm)
				{
				xpm  = false;
				type = atoms[ kXPMAtomIndex ];
				}
			else if (jpeg)		// JPEG is lossy
				{
				jpeg = false;
				type = atoms[ kJPEGAtomIndex ];
				}

			Atom returnType;
			unsigned char* data;
			JSize dataLength;
			JXSelectionManager::DeleteMethod delMethod;
			JString fileName;
			if (selMgr->GetData(selectionName, time, type,
								&returnType, &data, &dataLength, &delMethod) &&
				(JCreateTempFile(&fileName)).OK())
				{
				std::ofstream output(fileName.GetBytes());
				output.write((char*) data, dataLength);
				output.close();

				selMgr->DeleteData(&data, delMethod);

				const JError err =
					JXImage::CreateFromFile(display, fileName, image);
				JRemoveFile(fileName);

				if (err.OK())
					{
					return true;
					}
				}
			}
		}

	*image = nullptr;
	return false;
}
