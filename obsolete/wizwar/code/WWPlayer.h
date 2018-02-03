/******************************************************************************
 WWPlayer.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_WWPlayer
#define _H_WWPlayer

#include <JString.h>

class WWPlayer : virtual public JBroadcaster
{
public:

	WWPlayer(const JIndex playerIndex);

	virtual ~WWPlayer();

	JIndex			GetPlayerIndex() const;
	const JString&	GetName() const;
	const JString&	GetEmail() const;

	void			ReadIdentity(std::istream& input);
	virtual void	WriteState(std::ostream& output) const;

protected:

	// This is not private because Wiz/WarPlayer should ideally be a single
	// class.  Linkage issues force the split.

	JIndex	itsPlayerIndex;
	JString	itsName;
	JString	itsEmail;

private:

	// not allowed

	WWPlayer(const WWPlayer& source);
	WWPlayer& operator=(const WWPlayer& source);
};


/******************************************************************************
 GetPlayerIndex

 ******************************************************************************/

inline JIndex
WWPlayer::GetPlayerIndex()
	const
{
	return itsPlayerIndex;
}

/******************************************************************************
 GetName

 ******************************************************************************/

inline const JString&
WWPlayer::GetName()
	const
{
	return itsName;
}

/******************************************************************************
 GetEmail

 ******************************************************************************/

inline const JString&
WWPlayer::GetEmail()
	const
{
	return itsEmail;
}

#endif
