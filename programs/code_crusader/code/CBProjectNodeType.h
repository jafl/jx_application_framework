/******************************************************************************
 CBProjectNodeType.h

	Copyright © 1999 by John Lindal. All rights reserved.

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

istream& operator>>(istream& input, CBProjectNodeType& type);
ostream& operator<<(ostream& output, const CBProjectNodeType type);

#endif
