/******************************************************************************
 CBSymbolUpdatePG.h

	Copyright © 2007 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBSymbolUpdatePG
#define _H_CBSymbolUpdatePG

#include <JProgressDisplay.h>

class CBSymbolUpdatePG : public JProgressDisplay
{
public:

	CBSymbolUpdatePG(std::ostream& link, const JSize scaleFactor);

	virtual ~CBSymbolUpdatePG();

	virtual JBoolean	IncrementProgress(const JString& message = JString::empty) override;
	virtual JBoolean	IncrementProgress(const JSize delta) override;
	virtual JBoolean	IncrementProgress(const JString& message, const JSize delta) override;
	virtual void		ProcessFinished() override;
	virtual void		DisplayBusyCursor() override;

	void	SetScaleFactor(const JSize scaleFactor);

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JString& message, const JBoolean allowCancel,
									 const JBoolean allowBackground) override;

	virtual JBoolean	CheckForCancel() override;

private:

	std::ostream&	itsLink;
	JSize			itsScaleFactor;

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
