/******************************************************************************
 JXImageSelection.cpp

	image/xpm, image/gif, image/png, image/jpeg

	BASE CLASS = JXSelectionData

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#include <JXImageSelection.h>
#include <JXDisplay.h>
#include <JXSelectionManager.h>
#include <JXColorManager.h>
#include <JXImage.h>
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
	itsImage = NULL;
	SetData(image);
}

JXImageSelection::JXImageSelection
	(
	JXImage* image
	)
	:
	JXSelectionData(image->GetDisplay())
{
	itsImage = NULL;
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
	itsImage = NULL;
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
	JXImage* copy = jnew JXImage(image);
	assert( copy != NULL );
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
	if (itsImage != NULL)
		{
		StopListening(itsImage->GetXColorManager());
		jdelete itsImage;
		}

	itsImage = image;
	ListenTo(itsImage->GetXColorManager());
}

/******************************************************************************
 ConvertData (virtual protected)

 ******************************************************************************/

JBoolean
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
	*data         = NULL;
	*dataLength   = 0;
	*returnType   = None;
	*bitsPerBlock = 8;

	if (itsImage == NULL)
		{
		return kJFalse;
		}

	JString fileName;
	if (!(JCreateTempFile(&fileName)).OK())
		{
		return kJFalse;
		}

	JError err = JUnknownError(1);
	if (requestType == itsXPMAtom)
		{
		err = itsImage->WriteXPM(fileName);
		}
	else if (requestType == itsGIFAtom)
		{
		err = itsImage->WriteGIF(fileName, kJFalse);	// if too many colors, use PNG
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
		return kJTrue;
		}

	JRemoveFile(fileName);
	return kJFalse;
}

/******************************************************************************
 ReceiveGoingAway (virtual protected)

	The given sender has been deleted.

 ******************************************************************************/

void
JXImageSelection::ReceiveGoingAway
	(
	JBroadcaster* sender
	)
{
	if (sender == itsImage->GetXColorManager())
		{
		jdelete itsImage;
		itsImage = NULL;
		}
	else
		{
		JXSelectionData::ReceiveGoingAway(sender);
		}
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

JBoolean
JXImageSelection::GetImage
	(
	const Atom		selectionName,
	const Time		time,
	JXDisplay*		display,
	JXImage**		image
	)
{
	return GetImage(display->GetSelectionManager(), selectionName, time,
					display->GetColorManager(), image);
}

JBoolean
JXImageSelection::GetImage
	(
	JXSelectionManager* selMgr,
	const Atom			selectionName,
	const Time			time,
	JXColorManager*			colormap,
	JXImage**			image
	)
{
	JXDisplay* display = colormap->GetDisplay();
	Atom atoms[ kAtomCount ];
	display->RegisterXAtoms(kAtomCount, kAtomNames, atoms);

	JArray<Atom> typeList;
	if (selMgr->GetAvailableTypes(selectionName, time, &typeList))
		{
		JBoolean xpm=kJFalse, gif=kJFalse, png=kJFalse, jpeg=kJFalse;

		const JSize count = typeList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const Atom type = typeList.GetElement(i);
			xpm  = JI2B( xpm  || type == atoms[ kXPMAtomIndex ]  );
			gif  = JI2B( gif  || type == atoms[ kGIFAtomIndex ]  );
			png  = JI2B( png  || type == atoms[ kPNGAtomIndex ]  );
			jpeg = JI2B( jpeg || type == atoms[ kJPEGAtomIndex ] );
			}

		while (xpm || gif || png || jpeg)
			{
			Atom type = None;
			if (png)
				{
				png  = kJFalse;
				type = atoms[ kPNGAtomIndex ];
				}
			else if (gif)
				{
				gif  = kJFalse;
				type = atoms[ kGIFAtomIndex ];
				}
			else if (xpm)
				{
				xpm  = kJFalse;
				type = atoms[ kXPMAtomIndex ];
				}
			else if (jpeg)		// JPEG is lossy
				{
				jpeg = kJFalse;
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
					return kJTrue;
					}
				}
			}
		}

	*image = NULL;
	return kJFalse;
}
