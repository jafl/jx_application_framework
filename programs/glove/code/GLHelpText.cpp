/******************************************************************************
 GLHelpText.cpp

	Copyright © 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#include "GLHelpText.h"

const JCharacter* kHelpSectionName[] =
	{ kGLTOCHelpName, kGLTableHelpName, kGLPlotHelpName,
		kGLModuleHelpName, kGLChangeLogName, kGLCreditsName };

const JCharacter* kHelpSectionTitle[] =
	{ kTOCHelpTitle, kTableHelpTitle, kPlotHelpTitle,
		kModuleHelpTitle, kChangeLogTitle, kCreditsTitle };

const JCharacter* kHelpSectionText[] =
	{ kTOCHelpText, kTableHelpText, kPlotHelpText,
		kModuleHelpText, kChangeLogText, kCreditsText };

JSize kHelpSectionCount = sizeof(kHelpSectionName) / sizeof(JCharacter*);
