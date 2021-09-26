/******************************************************************************
 JPtrArray-JString.h

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPtrArray_JString
#define _H_JPtrArray_JString

#include "jx-af/jcore/JPtrArray.h"
#include "jx-af/jcore/JStringPtrMap.h"
#include "jx-af/jcore/JString.h"

std::istream& operator>>(std::istream&, JPtrArray<JString>&);
std::ostream& operator<<(std::ostream&, const JPtrArray<JString>&);

std::istream& operator>>(std::istream&, JStringPtrMap<JString>&);
std::ostream& operator<<(std::ostream&, const JStringPtrMap<JString>&);

bool JSameStrings(const JPtrArray<JString>&, const JPtrArray<JString>&,
				  const JString::Case caseSensitive);

JListT::CompareResult
JCompareStringsCaseSensitive(JString* const &, JString* const &);

JListT::CompareResult
JCompareStringsCaseInsensitive(JString* const &, JString* const &);

JString	JStringJoin(const JUtf8Byte* separator, const JPtrArray<JString>& list);
JString	JStringJoin(const JString& separator, const JPtrArray<JString>& list);

#endif
