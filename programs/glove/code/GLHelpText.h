/******************************************************************************
 GLHelpText.h

	Copyright © 1998 by Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GLHelpText
#define _H_GLHelpText

#include <jTypes.h>


static const JCharacter* kGLTOCHelpName				= "GLTOCHelp";
static const JCharacter* kGLTableHelpName			= "GLTableHelp";
static const JCharacter* kGLTableDataHelpName		= "GLTableHelp#Data";
static const JCharacter* kGLTableFilesHelpName		= "GLTableHelp#Files";
static const JCharacter* kGLTablePrintHelpName		= "GLTableHelp#Print";
static const JCharacter* kGLPlotHelpName			= "GLPlotHelp";
static const JCharacter* kGLPlotSettingsHelpName	= "GLPlotHelp#Settings";
static const JCharacter* kGLPlotCursorsHelpName		= "GLPlotHelp#Cursors";
static const JCharacter* kGLPlotMarksHelpName		= "GLPlotHelp#Marks";
static const JCharacter* kGLPlotAnalysisHelpName	= "GLPlotHelp#Analysis";
static const JCharacter* kGLFitHelpName				= "GLFitHelp";
static const JCharacter* kGLFitAddingHelpName		= "GLFitHelp#AddingFits";
static const JCharacter* kGLModuleHelpName			= "GLModuleHelp";
static const JCharacter* kGLChangeLogName			= "GLChangeLog";
static const JCharacter* kGLCreditsName				= "GLCredits";

static const JCharacter* kHelpSectionName[] =
	{
	kGLTOCHelpName, 
	kGLTableHelpName,
	kGLPlotHelpName,
	kGLFitHelpName,
	kGLModuleHelpName, 
	kGLChangeLogName, 
	kGLCreditsName 
	};

static JSize kHelpSectionCount = sizeof(kHelpSectionName) / sizeof(JCharacter*);

#endif
