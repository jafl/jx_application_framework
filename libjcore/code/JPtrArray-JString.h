/******************************************************************************
 JPtrArray-JString.h

	Copyright (C) 1997 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPtrArray_JString
#define _H_JPtrArray_JString

#include "JPtrArray.h"
#include "JStringPtrMap.h"
#include "JString.h"

std::istream& operator>>(std::istream&, JPtrArray<JString>&);
std::ostream& operator<<(std::ostream&, const JPtrArray<JString>&);

std::istream& operator>>(std::istream&, JStringPtrMap<JString>&);
std::ostream& operator<<(std::ostream&, const JStringPtrMap<JString>&);

bool JSameStrings(const JPtrArray<JString>&, const JPtrArray<JString>&,
				  const JString::Case caseSensitive);

std::weak_ordering
JCompareStringsCaseSensitive(JString* const &, JString* const &);

std::weak_ordering
JCompareStringsCaseInsensitive(JString* const &, JString* const &);

JString	JStringJoin(const JUtf8Byte* separator, const JPtrArray<JString>& list);
JString	JStringJoin(const JString& separator, const JPtrArray<JString>& list);

#endif
