/******************************************************************************
 JXImageSelection.cpp

	image/xpm, image/gif, image/png, image/jpeg

	BASE CLASS = JXSelectionData

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JXStdInc.h>
#include <JXImageSelection.h>
#include <JXDisplay.h>
#include <JXSelectionManager.h>
#include <JXColormap.h>
#include <JXImage.h>
#include <jStreamUtil.h>
#include <jFStreamUtil.h>
#include <jFileUtil.h>
#include <stdio.h>
#include <jAssert.h>

static const JCharacter* kXPMXAtomName  = "image/xpm";
static const JCharacter* kGIFXAtomName  = "image/gif";
static const JCharacter* kPNGXAtomName  = "image/png";
static const JCharacter* kJPEGXAtomName = "image/jpeg";

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
	const JCharacter*	id
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
	delete itsImage;
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
	itsXPMXAtom = AddType(kXPMXAtomName);

	#ifdef _J_HAS_GD
	itsGIFXAtom = AddType(kGIFXAtomName);
	itsPNGXAtom = AddType(kPNGXAtomName);
	itsJPEGXAtom = AddType(kJPEGXAtomName);
	#endif
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
	JXImage* copy = new JXImage(image);
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
		StopListening(itsImage->GetXColormap());
		delete itsImage;
		}

	itsImage = image;
	ListenTo(itsImage->GetXColormap());
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
	if (requestType == itsXPMXAtom)
		{
		err = itsImage->WriteXPM(fileName);
		}
	else if (requestType == itsGIFXAtom)
		{
		err = itsImage->WriteGIF(fileName, kJFalse);	// if too many colors, use PNG
		}
	else if (requestType == itsPNGXAtom)
		{
		err = itsImage->WritePNG(fileName);
		}
	else if (requestType == itsJPEGXAtom)
		{
		err = itsImage->WriteJPEG(fileName);
		}

	if (err.OK())
		{
		JString imageData;
		JReadFile(fileName, &imageData);
		JRemoveFile(fileName);

		*returnType = requestType;
		*dataLength = imageData.GetLength();
		*data       = new unsigned char[ *dataLength ];
		if (*data != NULL)
			{
			memcpy(*data, imageData.GetCString(), *dataLength);
			return kJTrue;
			}
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
	if (sender == itsImage->GetXColormap())
		{
		delete itsImage;
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

const JCharacter*
JXImageSelection::GetXPMXAtomName()
{
	return kXPMXAtomName;
}

const JCharacter*
JXImageSelection::GetGIFXAtomName()
{
	return kGIFXAtomName;
}

const JCharacter*
JXImageSelection::GetPNGXAtomName()
{
	return kPNGXAtomName;
}

const JCharacter*
JXImageSelection::GetJPEGXAtomName()
{
	return kJPEGXAtomName;
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
	JXImage**		image,
	const JBoolean	allowApproxColors
	)
{
	return GetImage(display->GetSelectionManager(), selectionName, time,
					display->GetColormap(), image, allowApproxColors);
}

JBoolean
JXImageSelection::GetImage
	(
	JXSelectionManager* selMgr,
	const Atom			selectionName,
	const Time			time,
	JXColormap*			colormap,
	JXImage**			image,
	const JBoolean		allowApproxColors
	)
{
	JXDisplay* display   = colormap->GetDisplay();
	const Atom xpmXAtom  = display->RegisterXAtom(kXPMXAtomName);
	const Atom gifXAtom  = display->RegisterXAtom(kGIFXAtomName);
	const Atom pngXAtom  = display->RegisterXAtom(kPNGXAtomName);
	const Atom jpegXAtom = display->RegisterXAtom(kJPEGXAtomName);

	JArray<Atom> typeList;
	if (selMgr->GetAvailableTypes(selectionName, time, &typeList))
		{
		JBoolean xpm=kJFalse, gif=kJFalse, png=kJFalse, jpeg=kJFalse;

		const JSize count = typeList.GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			const Atom type = typeList.GetElement(i);
			xpm  = JI2B( xpm  || type == xpmXAtom  );
			gif  = JI2B( gif  || type == gifXAtom  );
			png  = JI2B( png  || type == pngXAtom  );
			jpeg = JI2B( jpeg || type == jpegXAtom );
			}

		while (xpm || gif || png || jpeg)
			{
			Atom type = None;
			if (png)
				{
				png  = kJFalse;
				type = pngXAtom;
				}
			else if (gif)
				{
				gif  = kJFalse;
				type = gifXAtom;
				}
			else if (xpm)
				{
				xpm  = kJFalse;
				type = xpmXAtom;
				}
			else if (jpeg)		// JPEG is lossy
				{
				jpeg = kJFalse;
				type = jpegXAtom;
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
				ofstream output(fileName);
				output.write((char*) data, dataLength);
				output.close();

				selMgr->DeleteData(&data, delMethod);

				const JError err =
					JXImage::CreateFromFile(display, colormap, fileName, image,
											allowApproxColors);
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
