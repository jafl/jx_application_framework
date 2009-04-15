/******************************************************************************
 JPtrArray-JString.h

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPtrArray_JString
#define _H_JPtrArray_JString

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPtrArray.h>
#include <JStringPtrMap.h>
#include <JString.h>

istream& operator>>(istream&, JPtrArray<JString>&);
ostream& operator<<(ostream&, const JPtrArray<JString>&);

istream& operator>>(istream&, JStringPtrMap<JString>&);
ostream& operator<<(ostream&, const JStringPtrMap<JString>&);

JBoolean JSameStrings(const JPtrArray<JString>&, const JPtrArray<JString>&,
					  const JBoolean caseSensitive);

JOrderedSetT::CompareResult
JCompareStringsCaseSensitive(JString* const &, JString* const &);

JOrderedSetT::CompareResult
JCompareStringsCaseInsensitive(JString* const &, JString* const &);

#endif
