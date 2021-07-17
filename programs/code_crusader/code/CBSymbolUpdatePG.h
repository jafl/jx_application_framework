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

	virtual bool	IncrementProgress(const JString& message = JString::empty) override;
	virtual bool	IncrementProgress(const JSize delta) override;
	virtual bool	IncrementProgress(const JString& message, const JSize delta) override;
	virtual void		ProcessFinished() override;
	virtual void		DisplayBusyCursor() override;

	void	SetScaleFactor(const JSize scaleFactor);

protected:

	virtual void	ProcessBeginning(const ProcessType processType, const JSize stepCount,
									 const JString& message, const bool allowCancel,
									 const bool allowBackground) override;

	virtual bool	CheckForCancel() override;

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
