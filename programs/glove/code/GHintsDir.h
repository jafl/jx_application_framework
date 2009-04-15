/******************************************************************************
 GHintsDir.h

	Interface for the GHintsDir class

	Copyright © 1997 by Glenn Bach. 

 ******************************************************************************/

#ifndef _H_GHintsDir
#define _H_GHintsDir

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JXStaticText.h>
#include <JXWidget.h>
#include <jTypes.h>

class GHintsDir : public JXWindowDirector
{
public:

	GHintsDir(JXDirector* supervisor, const JCharacter* text, JXWidget* widget);

	virtual ~GHintsDir();

private:

	JXStaticText*			itsText;
	
private:

	// not allowed

	GHintsDir(const GHintsDir& source);
	const GHintsDir& operator=(const GHintsDir& source);
};

#endif