/******************************************************************************
 THXBaseConvDirector.h

	Interface for the THXBaseConvDirector class

	Copyright (C) 2000 by John Lindal.

 ******************************************************************************/

#ifndef _H_THXBaseConvDirector
#define _H_THXBaseConvDirector

#include <JXWindowDirector.h>

class JString;
class JXStaticText;
class JXTextButton;
class JXInputField;
class THXBaseConvMenu;

const JSize kTHXBaseCount = 4;

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

	bool		itsIgnoreTextFlag;
	JXInputField*	itsInput[ kTHXBaseCount ];

// begin JXLayout

	JXTextButton* itsCloseButton;
	JXInputField* its10Input;
	JXTextButton* itsHelpButton;
	JXInputField* its2Input;
	JXInputField* its8Input;
	JXInputField* its16Input;

// end JXLayout

private:

	void	BuildWindow();
	void	Convert(JBroadcaster* sender);

	// not allowed

	THXBaseConvDirector(const THXBaseConvDirector& source);
	const THXBaseConvDirector& operator=(const THXBaseConvDirector& source);
};

#endif
