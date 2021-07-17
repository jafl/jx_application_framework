/******************************************************************************
 SyGColUtils.h

	Copyright (C) 1999 by Glenn W. Bach.

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

bool	InsertFMCol(JXTable* table, bool visibleCols[],
						const JIndex leadingCols, const GFMColType type,
						JIndex* index, JCoordinate* retWidth);
bool	RemoveFMCol(JXTable* table, bool visibleCols[],
						const JIndex leadingCols, const GFMColType type,
						JIndex* index);

JSize		GetFMColCount(const bool visibleCols[], const JIndex leadingCols);
JSize		GetFMColWidth(const bool visibleCols[], const JIndex leadingCols, const JIndex index);
void		SetFMCols(bool kShowUID, bool kShowGID, bool kShowSize,
					  bool kShowMode, bool kShowDate);
void		GetFMCols(bool* kShowUID, bool* kShowGID, bool* kShowSize,
					  bool* kShowMode, bool* kShowDate);
GFMColType	GetFMColType(const JIndex index, const JIndex leadingCols,
						 const bool visibleCols[]);
bool	GFMTypeVisible(const bool visibleCols[],const GFMColType type);
void		GetFMColTitle(const bool visibleCols[], const JIndex leadingCols,
						  const JIndex index, JString* str);
JCoordinate GetFMBufferWidth(const bool visibleCols[], const JIndex leadingCols, const JIndex index);
JSize		GetFMWidthUsed(const bool visibleCols[]);

#endif
