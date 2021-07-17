/******************************************************************************
 CBPSPrinter.h

	Copyright © 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPSPrinter
#define _H_CBPSPrinter

#include <JXPSPrinter.h>
#include <JPrefObject.h>
#include <JString.h>

class CBTextEditor;
class CBPSPrintSetupDialog;

class CBPSPrinter : public JXPSPrinter, public JPrefObject
{
public:

	CBPSPrinter(JXDisplay* display);

	virtual ~CBPSPrinter();

	const JString&	GetHeaderName() const;
	void			SetPrintInfo(CBTextEditor* te, const JString& headerName);

	virtual bool	OpenDocument();
	virtual void		CloseDocument();
	virtual void		CancelDocument();

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual JXPSPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JString& printCmd, const JString& fileName,
							   const bool collate, const bool bw);

	virtual bool	EndUserPrintSetup(const JBroadcaster::Message& message,
										  bool* changed);

private:

	JSize			itsFontSize;
	CBTextEditor*	itsTE;			// nullptr unless printing; not owned
	JString			itsHeaderName;	// only used during printing

	CBPSPrintSetupDialog*	itsCBPrintSetupDialog;

private:

	// not allowed

	CBPSPrinter(const CBPSPrinter& source);
	const CBPSPrinter& operator=(const CBPSPrinter& source);
};


/******************************************************************************
 GetHeaderName

 ******************************************************************************/

inline const JString&
CBPSPrinter::GetHeaderName()
	const
{
	return itsHeaderName;
}

/******************************************************************************
 SetPrintInfo

 ******************************************************************************/

inline void
CBPSPrinter::SetPrintInfo
	(
	CBTextEditor*	te,
	const JString&	headerName
	)
{
	itsTE         = te;
	itsHeaderName = headerName;
}

#endif
