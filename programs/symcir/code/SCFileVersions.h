/******************************************************************************
 SCFileVersions.h

	Version information for unstructured data files

	Copyright © 1997 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_SCFileVersions
#define _H_SCFileVersions

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

const JFileVersion kCurrentMainFileVersion = 2;

// version 2:
//	SCExprEditor uses Read/WriteXEPSSetup().
// version 1:
//	SCCircuitDocument saves plots.

#endif
