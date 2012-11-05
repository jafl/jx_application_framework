/******************************************************************************
 JISOStyler.h

	Copyright © 1998 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JISOStyler
#define _H_JISOStyler

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JString.h>
#include <JFontStyle.h>

class JTextEditor;

class JISOStyler
{
public:

	JISOStyler(JTextEditor* editor);

	virtual	~JISOStyler();

	void	FilterISO(const JString& text);

private:

	JString			itsData;
	JTextEditor*	itsEditor;
	JFontStyle		itsFontStyle;

private:

	void	AdjustStyle(const JString& valStr);

	// not allowed

	JISOStyler(const JISOStyler& source);
	const JISOStyler& operator=(const JISOStyler& source);
};

#endif
