/******************************************************************************
 THXBaseConvDirector.h

	Interface for the THXBaseConvDirector class

	Copyright (C) 1998 by John Lindal. All rights reserved.

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
	THXBaseConvDirector(std::istream& input, const JFileVersion vers,
						JXDirector* supervisor);

	virtual ~THXBaseConvDirector();

	void	WriteState(std::ostream& output) const;

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

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
