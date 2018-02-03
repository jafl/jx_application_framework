/******************************************************************************
 thxHelpText.h

	Copyright (C) 1998 by John Lindal.

 ******************************************************************************/

#ifndef _H_thxHelpText
#define _H_thxHelpText

#include <jTypes.h>

static const JCharacter* kTHXTOCHelpName       = "THXTOCHelp";
static const JCharacter* kTHXOverviewHelpName  = "THXOverviewHelp";
static const JCharacter* kTHXConstantsHelpName = "THXConstantsHelp";
static const JCharacter* kTHXExprHelpName      = "THXExprHelp";
static const JCharacter* kTHX2DPlotHelpName    = "THX2DPlotHelp";
static const JCharacter* kTHXBaseConvHelpName  = "THXBaseConvHelp";
static const JCharacter* kTHXChangeLogName     = "THXChangeLog";
static const JCharacter* kTHXCreditsName       = "THXCredits";

static const JCharacter* kTHXHelpSectionName[] =
	{
	kTHXTOCHelpName,
	kTHXOverviewHelpName,
	kTHXConstantsHelpName,
	kTHXExprHelpName,
	kTHX2DPlotHelpName,
	kTHXBaseConvHelpName,
	kTHXChangeLogName,
	kTHXCreditsName,
	"LICENSE"
	};

const JSize kTHXHelpSectionCount = sizeof(kTHXHelpSectionName) / sizeof(JCharacter*);

#endif
