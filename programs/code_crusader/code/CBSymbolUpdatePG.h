/******************************************************************************
 CBSymbolUpdatePG.h

	Copyright (C) 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSymbolUpdatePG
#define _H_CBSymbolUpdatePG

#include <JProgressDisplay.h>

class CBSymbolUpdatePG : public JProgressDisplay
{
public:

	CBSymbolUpdatePG(std::ostream& link, const JSize scaleFactor);

	virtual ~CBSymbolUpdatePG();

	virtual JBoolean	IncrementProgress(const JCharacter* message = nullptr);
	virtual JBoolean	IncrementProgress(const JSize delta);
	virtual JBoolean	IncrementProgress(const JCharacter* message,
										  const JSize delta);
	virtual void		ProcessFinished();
	virtual void		DisplayBusyCursor();

	void	SetScaleFactor(const JSize scaleFactor);

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JCharacter* message, const JBoolean allowCancel,
									 const JBoolean allowBackground);

	virtual JBoolean	CheckForCancel();

private:

	std::ostream&	itsLink;
	JSize		itsScaleFactor;

private:

	// not allowed

	CBSymbolUpdatePG(const CBSymbolUpdatePG& source);
	const CBSymbolUpdatePG& operator=(const CBSymbolUpdatePG& source);
};


/******************************************************************************
 SetScaleFactor

 ******************************************************************************/

inline void
CBSymbolUpdatePG::SetScaleFactor
	(
	const JSize scaleFactor
	)
{
	itsScaleFactor = scaleFactor;
}

#endif
