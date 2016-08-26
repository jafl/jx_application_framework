/******************************************************************************
 cmHelpText.h

	Copyright © 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_cmHelpText
#define _H_cmHelpText

#include <jTypes.h>

static const JCharacter* kCMTOCHelpName              = "CMTOCHelp";
static const JCharacter* kCMOverviewHelpName         = "CMOverviewHelp";
static const JCharacter* kCMGettingStartedHelpName   = "CMGettingStartedHelp";

static const JCharacter* kCMCommandDirHelpName       = "CMCommandDirHelp";
static const JCharacter* kCMJavaConfigFileHelpName   = "CMJavaConfigFileHelp";
static const JCharacter* kCMXdebugConfigFileHelpName = "CMXdebugConfigFileHelp";

static const JCharacter* kCMSourceWindowHelpName     = "CMSourceWindowHelp";
static const JCharacter* kCMFileListHelpName         = "CMSourceWindowHelp#FileList";

static const JCharacter* kCMVarTreeHelpName          = "CMVarTreeHelp";
static const JCharacter* kCMArray1DHelpName          = "CMVarTreeHelp#Array1D";
static const JCharacter* kCMArray2DHelpName          = "CMVarTreeHelp#Array2D";
static const JCharacter* kCMPlot2DHelpName           = "CMVarTreeHelp#Plot2D";
static const JCharacter* kCMMemoryHelpName           = "CMVarTreeHelp#Memory";
static const JCharacter* kCMRegistersHelpName        = "CMVarTreeHelp#Registers";
static const JCharacter* kCMLocalVarsHelpName        = "CMVarTreeHelp#Local";

static const JCharacter* kCM2DPlotHelpName           = "CM2DPlotHelp";

static const JCharacter* kCMThreadsHelpName          = "CMThreadsHelp";
static const JCharacter* kCMStackHelpName            = "CMStackHelp";
static const JCharacter* kCMBreakpointsHelpName      = "CMBreakpointsHelp";

static const JCharacter* kCMMiscHelpName             = "CMMiscHelp";
static const JCharacter* kCMSearchTextHelpName       = "JXSearchTextHelp";
static const JCharacter* kCMRegexHelpName            = "JXRegexHelp";
static const JCharacter* kCMRegexQRefName            = "JXRegexQRef";
static const JCharacter* kCMDockHelpName             = "JXDockHelp";

static const JCharacter* kCMChangeLogName            = "CMChangeLog";
static const JCharacter* kCMCreditsName              = "CMCredits";

static const JCharacter* kCMHelpSectionName[] =
	{
	kCMTOCHelpName,
	kCMOverviewHelpName,
	kCMGettingStartedHelpName,
	kCMCommandDirHelpName,
	kCMJavaConfigFileHelpName,
	kCMXdebugConfigFileHelpName,
	kCMSourceWindowHelpName,
	kCMVarTreeHelpName,
	kCM2DPlotHelpName,
	kCMThreadsHelpName,
	kCMStackHelpName,
	kCMBreakpointsHelpName,
	kCMMiscHelpName,
	kCMSearchTextHelpName,
	kCMRegexHelpName,
	kCMRegexQRefName,
	kCMDockHelpName,
	kCMChangeLogName,
	kCMCreditsName,
	"LICENSE"
	};

const JSize kCMHelpSectionCount = sizeof(kCMHelpSectionName) / sizeof(JCharacter*);

#endif
