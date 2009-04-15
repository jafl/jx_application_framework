/******************************************************************************
 JColorList.cpp

	Stores a list of static colors so they can be deallocated later.

	BASE CLASS = JContainer

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JColorList.h>
#include <JColormap.h>
#include <JOrderedSetUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JColorList::JColorList
	(
	JColormap* colormap
	)
	:
	JContainer()
{
	itsColormap  = colormap;
	itsColorList = NULL;
}

/******************************************************************************
 Copy constructor

 ******************************************************************************/

JColorList::JColorList
	(
	const JColorList& source
	)
	:
	JContainer(),
	itsColormap(source.itsColormap),
	itsColorList(NULL)
{
	if (source.itsColorList != NULL)
		{
		CreateColorList();
		*itsColorList = *(source.itsColorList);

		const JSize colorCount = itsColorList->GetElementCount();
		for (JIndex i=1; i<colorCount; i++)
			{
			itsColormap->UsingColor(itsColorList->GetElement(i));
			}
		}
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JColorList::~JColorList()
{
	RemoveAll();
	delete itsColorList;
}

/******************************************************************************
 Add

 ******************************************************************************/

JColorIndex
JColorList::Add
	(
	const JRGB& color
	)
{
	JColorIndex colorIndex;
	if (itsColormap->AllocateStaticColor(color, &colorIndex))
		{
		CreateColorList();
		if (!itsColorList->InsertSorted(colorIndex, kJFalse))
			{
			itsColormap->DeallocateColor(colorIndex);
			}
		return colorIndex;
		}
	else
		{
		return itsColormap->GetBlackColor();
		}
}

/******************************************************************************
 Add

 ******************************************************************************/

JColorIndex
JColorList::Add
	(
	const JCharacter* name
	)
{
	JColorIndex colorIndex;
	if (itsColormap->AllocateStaticNamedColor(name, &colorIndex))
		{
		CreateColorList();
		if (!itsColorList->InsertSorted(colorIndex, kJFalse))
			{
			itsColormap->DeallocateColor(colorIndex);
			}
		return colorIndex;
		}
	else
		{
		return itsColormap->GetBlackColor();
		}
}

/******************************************************************************
 Add

 ******************************************************************************/

void
JColorList::Add
	(
	const JColorIndex colorIndex
	)
{
	CreateColorList();
	if (itsColorList->InsertSorted(colorIndex, kJFalse))
		{
		itsColormap->UsingColor(colorIndex);
		}
}

/******************************************************************************
 Remove

 ******************************************************************************/

void
JColorList::Remove
	(
	const JColorIndex colorIndex
	)
{
	JIndex i;
	if (itsColorList != NULL &&
		itsColorList->SearchSorted(colorIndex, JOrderedSetT::kAnyMatch, &i))
		{
		itsColormap->DeallocateColor(colorIndex);
		itsColorList->RemoveElement(i);
		}
}

/******************************************************************************
 RemoveAll

 ******************************************************************************/

void
JColorList::RemoveAll()
{
	if (itsColorList != NULL)
		{
		const JSize count = itsColorList->GetElementCount();
		for (JIndex i=1; i<=count; i++)
			{
			itsColormap->DeallocateColor(itsColorList->GetElement(i));
			}
		itsColorList->RemoveAll();
		}
}

/******************************************************************************
 CreateColorList (private)

 ******************************************************************************/

void
JColorList::CreateColorList()
{
	if (itsColorList == NULL)
		{
		itsColorList = new JArray<JColorIndex>;
		assert( itsColorList != NULL );
		itsColorList->SetCompareFunction(JCompareIndices);
		InstallOrderedSet(itsColorList);
		}
}
