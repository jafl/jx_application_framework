/******************************************************************************
 GLHelpText.cpp

	Copyright (C) 1998 by Glenn Bach.

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
