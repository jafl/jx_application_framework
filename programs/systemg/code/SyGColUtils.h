/******************************************************************************
 SyGColUtils.h

	Copyright © 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_SyGColUtils
#define _H_SyGColUtils

#include <jTypes.h>

class JString;
class JXTable;

enum GFMColType
{
	kGFMUID = 0,
	kGFMGID,
	kGFMSize,
	kGFMMode,
	kGFMDate,
	kGFMName
};

JBoolean	InsertFMCol(JXTable* table, JBoolean visibleCols[],
						const JIndex leadingCols, const GFMColType type,
						JIndex* index, JCoordinate* retWidth);
JBoolean	RemoveFMCol(JXTable* table, JBoolean visibleCols[],
						const JIndex leadingCols, const GFMColType type,
						JIndex* index);

JSize		GetFMColCount(const JBoolean visibleCols[], const JIndex leadingCols);
JSize		GetFMColWidth(const JBoolean visibleCols[], const JIndex leadingCols, const JIndex index);
void		SetFMCols(JBoolean kShowUID, JBoolean kShowGID, JBoolean kShowSize,
					  JBoolean kShowMode, JBoolean kShowDate);
void		GetFMCols(JBoolean* kShowUID, JBoolean* kShowGID, JBoolean* kShowSize,
					  JBoolean* kShowMode, JBoolean* kShowDate);
GFMColType	GetFMColType(const JIndex index, const JIndex leadingCols,
						 const JBoolean visibleCols[]);
JBoolean	GFMTypeVisible(const JBoolean visibleCols[],const GFMColType type);
void		GetFMColTitle(const JBoolean visibleCols[], const JIndex leadingCols,
						  const JIndex index, JString* str);
JCoordinate GetFMBufferWidth(const JBoolean visibleCols[], const JIndex leadingCols, const JIndex index);
JSize		GetFMWidthUsed(const JBoolean visibleCols[]);

#endif
