/******************************************************************************
 thxFileVersions.h

	Version information for unstructured data files

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_thxFileVersions
#define _H_thxFileVersions

#include <jTypes.h>

const JFileVersion kCurrentStateVersion = 11;

// version 11:
//	Discarded THX3DPlotDirector state.
// version 10:
//	Stores THX3DPlotDirector state.
// version 9:
//	Converted THXBaseConvDirector from 2 inputs + menus to 4 inputs.
// version 8:
//	JXHelpManager moved to JX shared prefs.
// version 7:
//	THXApp stores EPS graph printer setup.
// version 6:
//	JX2DPlotWidget version number increased.
//	THX2DPlotDirector print name moved to JX2DPlotWidget.
// version 5:
//	Stores JXChooseSaveFile setup.
// version 4:
//	THXExprDirector and THX2DPlotDirector store print name.
//	JXHelpManager setup version incremented.
// version 3:
//	THXApp stores PS graph printer setup.
// version 2:
//	THXApp stores expression window geometry and JXHelpManager setup.
// version 1:
//	THXApp stores visibility of keypad.

#endif
