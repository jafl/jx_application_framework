/******************************************************************************
 thxHelpText.h

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_thxHelpText
#define _H_thxHelpText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

static const JCharacter* kTHXTOCHelpName       = "THXTOCHelp";
static const JCharacter* kTHXOverviewHelpName  = "THXOverviewHelp";
static const JCharacter* kTHXConstantsHelpName = "THXConstantsHelp";
static const JCharacter* kTHXExprHelpName      = "THXExprHelp";
static const JCharacter* kTHX2DPlotHelpName    = "THX3DPlotHelp";
static const JCharacter* kTHX3DPlotHelpName    = "THX3DPlotHelp";
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
	kTHX3DPlotHelpName,
	kTHXBaseConvHelpName,
	kTHXChangeLogName,
	kTHXCreditsName,
	"LICENSE"
	};

const JSize kTHXHelpSectionCount = sizeof(kTHXHelpSectionName) / sizeof(JCharacter*);

#endif
