/******************************************************************************
 THXBaseConvDirector.h

	Interface for the THXBaseConvDirector class

	Copyright © 1998 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_THXBaseConvDirector
#define _H_THXBaseConvDirector

#include <JXWindowDirector.h>

class JXStaticText;
class JXTextButton;
class JXInputField;
class THXBaseConvMenu;

class THXBaseConvDirector : public JXWindowDirector
{
public:

	THXBaseConvDirector(JXDirector* supervisor);
	THXBaseConvDirector(istream& input, const JFileVersion vers,
						JXDirector* supervisor);

	virtual ~THXBaseConvDirector();

	void	WriteState(ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

// begin JXLayout

    JXTextButton*    itsCloseButton;
    JXInputField*    itsFromValue;
    JXTextButton*    itsHelpButton;
    THXBaseConvMenu* itsFromBase;
    THXBaseConvMenu* itsToBase;
    JXStaticText*    itsToValue;

// end JXLayout

private:

	void	BuildWindow();
	void	Convert();

	// not allowed

	THXBaseConvDirector(const THXBaseConvDirector& source);
	const THXBaseConvDirector& operator=(const THXBaseConvDirector& source);
};

#endif
