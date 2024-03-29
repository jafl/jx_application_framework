/******************************************************************************
 fileVersions.h

	Version information for unstructured data files

	Copyright © 2023 by John Lindal.

 ******************************************************************************/

#ifndef _H_fileVersions
#define _H_fileVersions

#include <jx-af/jcore/jTypes.h>

const JFileVersion kCurrentFileVersion = 9;

// version 9:
//	saves window min size (wtf?)
// version 8:
//	addes hideBorder to RadioGroup
// version 7:
//	adds containerIndex after parentIndex in LayoutContaienr
// version 6:
//	adds itsMonospaceFlag and itsHint to InputField
// version 5:
//	adds itsAlreadyLocalVarFlag to LayoutWidget
// version 4:
//	adds itsType to ImageWidget
// version 3:
//	adds itsXWMClass to LayoutContainer
// version 2:
//	adds itsDependencyNames to CustomWidget
// version 1:
//	adds itsValidationFlags to InputField

#endif
