/******************************************************************************
 GMFilterAction.h

	Interface for the GMFilterAction class

	Copyright � 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#ifndef _H_GMFilterAction
#define _H_GMFilterAction

#include <JString.h>

class GMFilterAction
{
public:

	GMFilterAction();
	GMFilterAction(const JBoolean mailbox,
					  const JBoolean copy,
					  const JCharacter* dest);
	GMFilterAction(const GMFilterAction& source);
	virtual ~GMFilterAction();

	JBoolean		IsMailbox() const;
	void			ShouldBeMailbox(const JBoolean mailbox);

	JBoolean		IsCopying() const;
	void			ShouldCopy(const JBoolean copy);

	const JString&	GetDestination() const;
	void			SetDestination(const JCharacter* dest);

	void		ReadSetup(istream& input);
	void		WriteSetup(ostream& output) const;

private:

	JBoolean		itsIsMailbox;
	JBoolean		itsIsCopying;
	JString			itsDestination;

private:

	// not allowed

	const GMFilterAction& operator=(const GMFilterAction& source);
};

#endif
