/******************************************************************************
 CBProjectNodeType.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBProjectNodeType
#define _H_CBProjectNodeType

#include <jTypes.h>

// Do not change the file type values once they are assigned because
// they are stored in the prefs file.

enum CBProjectNodeType
{
	kCBRootNT = 0,
	kCBGroupNT,
	kCBFileNT,
	kCBLibraryNT
};

const JSize kCBNTCount = kCBLibraryNT+1;

std::istream& operator>>(std::istream& input, CBProjectNodeType& type);
std::ostream& operator<<(std::ostream& output, const CBProjectNodeType type);

#endif
