/******************************************************************************
 SyGColUtils.cpp

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#include "SyGColUtils.h"
#include <JString.h>
#include <jAssert.h>

const JCoordinate kUIDWidth		= 60;
const JCoordinate kGIDWidth		= 60;
const JCoordinate kSizeWidth	= 60;
const JCoordinate kModeWidth	= 90;
const JCoordinate kDateWidth	= 180;

const JCharacter* kUIDTitleStr	= "User";
const JCharacter* kGIDTitleStr	= "Group";
const JCharacter* kSizeTitleStr	= "Size";
const JCharacter* kModeTitleStr	= "Permissions";
const JCharacter* kDateTitleStr	= "Date";

const JCoordinate kGFMBufferWidth = 6;

/******************************************************************************
 InsertFMCol

 ******************************************************************************/

JBoolean
InsertFMCol
	(
	JXTable*			table,
	JBoolean			visibleCols[],
	const JIndex		leadingCols,
	const GFMColType	type,
	JIndex*			index,
	JCoordinate*		retWidth
	)
{
	if (GFMTypeVisible(visibleCols, type))
		{
		return kJFalse;
		}

	*index = leadingCols + 1;

	if (type == kGFMUID)
		{
		*retWidth = kUIDWidth;
		visibleCols[kGFMUID] = kJTrue;
		return kJTrue;
		}

	if (type >= kGFMGID)
		{
		JBoolean found = kJFalse;
		JSize i = 3;

		while(i <= GetFMColCount(visibleCols, leadingCols) && !(found))
			{
			if (GetFMColType(i, leadingCols, visibleCols) > type)
				{
				*index = i;
				found = kJTrue;
				}
			i++;
			}

		if (!(found))
			{
			*index = GetFMColCount(visibleCols, leadingCols)+1;
			}

		JCoordinate width;

		if (type == kGFMUID)
			{
			*retWidth = kUIDWidth;
			visibleCols[kGFMUID] = kJTrue;
			}

		else if (type == kGFMGID)
			{
			*retWidth = kGIDWidth;
			visibleCols[kGFMGID] = kJTrue;
			}

		else if (type == kGFMSize)
			{
			*retWidth = kSizeWidth;
			visibleCols[kGFMSize] = kJTrue;
			}

		else if (type == kGFMMode)
			{
			*retWidth = kModeWidth;
			visibleCols[kGFMMode] = kJTrue;
			}

		else if (type == kGFMDate)
			{
			*retWidth = kDateWidth;
			visibleCols[kGFMDate] = kJTrue;
			}
		}

	return kJTrue;


}

/******************************************************************************
 RemoveFMCol

 ******************************************************************************/

JBoolean
RemoveFMCol
	(
	JXTable*			table,
	JBoolean			visibleCols[],
	const JIndex		leadingCols,
	const GFMColType	type,
	JIndex*			index
	)
{
	JSize i		= leadingCols + 1;
	JBoolean found	= kJFalse;

	while (i <= GetFMColCount(visibleCols, leadingCols) && !(found))
		{
		if (type == GetFMColType(i, leadingCols, visibleCols))
			{
			*index = i;
			found = kJTrue;
			}
		i++;
		}

	if (found)
		{

		if (type == kGFMUID)
			{
			visibleCols[kGFMUID] = kJFalse;
			}

		else if (type == kGFMGID)
			{
			visibleCols[kGFMGID] = kJFalse;
			}

		else if (type == kGFMSize)
			{
			visibleCols[kGFMSize] = kJFalse;
			}

		else if (type == kGFMMode)
			{
			visibleCols[kGFMMode] = kJFalse;
			}

		else if (type == kGFMDate)
			{
			visibleCols[kGFMDate] = kJFalse;
			}

		return kJTrue;
		}

	return kJFalse;
}

/******************************************************************************
 GetFMColCount

 ******************************************************************************/

JSize
GetFMColCount
	(
	const JBoolean	 visibleCols[],
	const JIndex	leadingCols
	)
{
	JSize temp = leadingCols;

	if (visibleCols[kGFMUID])
		{
		temp++;
		}

	if (visibleCols[kGFMGID])
		{
		temp++;
		}

	if (visibleCols[kGFMSize])
		{
		temp++;
		}

	if (visibleCols[kGFMMode])
		{
		temp++;
		}

	if (visibleCols[kGFMDate])
		{
		temp++;
		}

	return temp;
}

/******************************************************************************
 GetFMColWidth

 ******************************************************************************/

JSize
GetFMColWidth
	(
	const JBoolean	visibleCols[],
	const JIndex	leadingCols,
	const JIndex	index
	)
{
	GFMColType type = GetFMColType(index, leadingCols, visibleCols);

	if (type == kGFMUID)
		{
		return kUIDWidth;
		}

	else if (type == kGFMGID)
		{
		return kGIDWidth;
		}

	else if (type == kGFMSize)
		{
		return kSizeWidth;
		}

	else if (type == kGFMMode)
		{
		return kModeWidth;
		}

	else if (type == kGFMDate)
		{
		return kDateWidth;
		}

	return 0;

}

/******************************************************************************
 SetFMCols

 ******************************************************************************/

void
SetFMCols
	(
	JBoolean kShowUID,
	JBoolean kShowGID,
	JBoolean kShowSize,
	JBoolean kShowMode,
	JBoolean kShowDate
	)
{

}

/******************************************************************************
 GetFMCols

 ******************************************************************************/

void
GetFMCols
	(
	JBoolean* kShowUID,
	JBoolean* kShowGID,
	JBoolean* kShowSize,
	JBoolean* kShowMode,
	JBoolean* kShowDate
	)
{

}

/******************************************************************************
 GetFMColType

 ******************************************************************************/

GFMColType
GetFMColType
	(
	const JIndex	index,
	const JIndex	leadingCols,
	const JBoolean	visibleCols[]
	)
{
	JIndex temp = leadingCols;

	if (visibleCols[kGFMUID] == kJTrue)
		{
		temp++;
		if (temp == index)
			{
			return kGFMUID;
			}
		}

	if (visibleCols[kGFMGID] == kJTrue)
		{
		temp++;
		if (temp == index)
			{
			return kGFMGID;
			}
		}

	if (visibleCols[kGFMSize] == kJTrue)
		{
		temp++;
		if (temp == index)
			{
			return kGFMSize;
			}
		}

	if (visibleCols[kGFMMode] == kJTrue)
		{
		temp++;
		if (temp == index)
			{
			return kGFMMode;
			}
		}

	if (visibleCols[kGFMDate] == kJTrue)
		{
		temp++;
		if (temp == index)
			{
			return kGFMDate;
			}
		}

	return kGFMName;
}

/******************************************************************************
 GFMTypeVisible

 ******************************************************************************/

JBoolean
GFMTypeVisible
	(
	const JBoolean	 visibleCols[],
	const GFMColType type
	)
{
	if (type == kGFMUID)
		{
		if (visibleCols[kGFMUID] == kJTrue)
			{
			return kJTrue;
			}
		}

	else if (type == kGFMGID)
		{
		if (visibleCols[kGFMGID] == kJTrue)
			{
			return kJTrue;
			}
		}

	else if (type == kGFMSize)
		{
		if (visibleCols[kGFMSize] == kJTrue)
			{
			return kJTrue;
			}
		}

	else if (type == kGFMMode)
		{
		if (visibleCols[kGFMMode] == kJTrue)
			{
			return kJTrue;
			}
		}

	else if (type == kGFMDate)
		{
		if (visibleCols[kGFMDate] == kJTrue)
			{
			return kJTrue;
			}
		}

	return kJFalse;
}

/******************************************************************************
 GetFMColTitle

 ******************************************************************************/

void
GetFMColTitle
	(
	const JBoolean	visibleCols[],
	const JIndex	leadingCols,
	const JIndex	index,
	JString*		str
	)
{
	GFMColType type = GetFMColType(index, leadingCols, visibleCols);

	if (type == kGFMUID)
		{
		*str =  kUIDTitleStr;
		}

	else if (type == kGFMGID)
		{
		*str =  kGIDTitleStr;
		}

	else if (type == kGFMSize)
		{
		*str =  kSizeTitleStr;
		}

	else if (type == kGFMMode)
		{
		*str =  kModeTitleStr;
		}

	else if (type == kGFMDate)
		{
		*str =  kDateTitleStr;
		}


}

/******************************************************************************
 GetFMBufferWidth

 ******************************************************************************/

JCoordinate
GetFMBufferWidth
	(
	const JBoolean	visibleCols[],
	const JIndex	leadingCols,
	const JIndex	index
	)
{
	const GFMColType type = GetFMColType(index, leadingCols, visibleCols);
	return (type == kGFMSize ? -kGFMBufferWidth : kGFMBufferWidth);
}

/******************************************************************************
 GetRealColType

 ******************************************************************************/

GFMColType
GetRealColType
	(
	const JBoolean	visibleCols[],
	const JIndex	leadingCols,
	const JIndex	index,
	JIndex*		subIndex
	)
{
	GFMColType type = GetFMColType(index, leadingCols, visibleCols);

	if (type <= kGFMMode)
		{
		*subIndex = 1;
		return type;
		}

	else if (type == kGFMDate && !(GFMTypeVisible(visibleCols, kGFMMode)))
		{
		*subIndex = 1;
		return type;
		}

	JIndex i         = leadingCols + 1;
	JBoolean found   = kJFalse;
	JIndex tempIndex = 0;

	while (i <= GetFMColCount(visibleCols, leadingCols) && !(found))
		{
		if (GetFMColType(i, leadingCols, visibleCols) == kGFMMode)
			{
			tempIndex = i;
			found = kJTrue;
			}
		i++;
		}

	if (index - tempIndex >= 9)
		{
		*subIndex = 1;
		return kGFMDate;
		}

	*subIndex = index - tempIndex + 1;
	return kGFMMode;
}

/******************************************************************************
 GetFMWidthUsed

 ******************************************************************************/


JSize
GetFMWidthUsed
	(
	const JBoolean visibleCols[]
	)
{
	JSize width = 0;

	if (GFMTypeVisible(visibleCols, kGFMUID))
		{
		width += kUIDWidth;
		}

	if (GFMTypeVisible(visibleCols, kGFMGID))
		{
		width += kGIDWidth;
		}

	if (GFMTypeVisible(visibleCols, kGFMSize))
		{
		width += kSizeWidth;
		}

	if (GFMTypeVisible(visibleCols, kGFMMode))
		{
		width += kModeWidth;
		}

	if (GFMTypeVisible(visibleCols, kGFMDate))
		{
		width += kDateWidth;
		}

	return width;
}
