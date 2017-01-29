/******************************************************************************
 JPtrArray-JString.h

	Copyright (C) 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPtrArray_JString
#define _H_JPtrArray_JString

#include <JPtrArray.h>
#include <JStringPtrMap.h>
#include <JString.h>

std::istream& operator>>(std::istream&, JPtrArray<JString>&);
std::ostream& operator<<(std::ostream&, const JPtrArray<JString>&);

std::istream& operator>>(std::istream&, JStringPtrMap<JString>&);
std::ostream& operator<<(std::ostream&, const JStringPtrMap<JString>&);

JBoolean JSameStrings(const JPtrArray<JString>&, const JPtrArray<JString>&,
					  const JBoolean caseSensitive);

JListT::CompareResult
JCompareStringsCaseSensitive(JString* const &, JString* const &);

JListT::CompareResult
JCompareStringsCaseInsensitive(JString* const &, JString* const &);

#endif
