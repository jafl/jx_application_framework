/******************************************************************************
 <pre>HelpText.h

   Copyright © <Year> by <Company>. All rights reserved.

 ******************************************************************************/

#ifndef _H_<pre>HelpText
#define _H_<pre>HelpText

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <jTypes.h>

static const JCharacter* k<PRE>TOCHelpName		= "<PRE>TOCHelp";
static const JCharacter* k<PRE>MainHelpName		= "<PRE>MainHelp";
static const JCharacter* k<PRE>OverviewHelpName	= "<PRE>OverviewHelp";
static const JCharacter* k<PRE>ChangeLogName	= "<PRE>Changes";
static const JCharacter* k<PRE>CreditsName		= "<PRE>Credits";

static const JCharacter* k<PRE>HelpSectionName[] =
	{
	k<PRE>TOCHelpName,
	k<PRE>OverviewHelpName,
	k<PRE>MainHelpName,
	k<PRE>ChangeLogName,
	k<PRE>CreditsName
	};

const JSize k<PRE>HelpSectionCount = sizeof(k<PRE>HelpSectionName) / sizeof(JCharacter*);

#endif
