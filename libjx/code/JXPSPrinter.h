/******************************************************************************
 JXPSPrinter.h

	Interface for the JXPSPrinter class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPSPrinter
#define _H_JXPSPrinter

#include <jx-af/jcore/JPSPrinter.h>

class JXDisplay;
class JXPSPrintSetupDialog;
class JXPSPageSetupDialog;

class JXPSPrinter : public JPSPrinter
{
public:

	// Do not change these values once they are assigned
	// because they are stored in files.

	enum Destination
	{
		kPrintToPrinter = 0,
		kPrintToFile    = 1
	};

public:

	JXPSPrinter(const JXDisplay* display);

	~JXPSPrinter();

	// saving setup information

	void	ReadXPSSetup(std::istream& input);
	void	WriteXPSSetup(std::ostream& output) const;

	// printing control

	bool	OpenDocument() override;
	void	CloseDocument() override;

	// printing parameters

	Destination		GetDestination() const;
	void			SetDestination(const Destination dest,
								   const JString& printCmd,
								   const JString& fileName);

	const JString&	GetPrintCmd() const;
	void			SetPrintCmd(const JString& cmd);

	const JString&	GetFileName() const;
	void			SetFileName(const JString& name);

	bool	WillCollatePages() const;
	void	CollatePages(const bool doIt);

	// Page Setup and Print Setup dialogs

	void	BeginUserPageSetup();
	void	BeginUserPrintSetup();

protected:

	virtual JXPSPageSetupDialog*
		CreatePageSetupDialog(const PaperType paper, const ImageOrientation orient);

	virtual JXPSPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JString& printCmd, const JString& fileName,
							   const bool collate, const bool bw);

	virtual bool	EndUserPageSetup(const JBroadcaster::Message& message);
	virtual bool	EndUserPrintSetup(const JBroadcaster::Message& message,
										  bool* changed);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	Destination	itsDestination;
	JString		itsPrintCmd;
	JString		itsFileName;
	bool		itsCollateFlag;

	JXPSPageSetupDialog*	itsPageSetupDialog;
	JXPSPrintSetupDialog*	itsPrintSetupDialog;
};

std::istream& operator>>(std::istream& input, JXPSPrinter::Destination& dest);
std::ostream& operator<<(std::ostream& output, const JXPSPrinter::Destination dest);


/******************************************************************************
 GetDestination

 ******************************************************************************/

inline JXPSPrinter::Destination
JXPSPrinter::GetDestination()
	const
{
	return itsDestination;
}

/******************************************************************************
 GetPrintCmd

 ******************************************************************************/

inline const JString&
JXPSPrinter::GetPrintCmd()
	const
{
	return itsPrintCmd;
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

inline const JString&
JXPSPrinter::GetFileName()
	const
{
	return itsFileName;
}

/******************************************************************************
 Collated pages

 ******************************************************************************/

inline bool
JXPSPrinter::WillCollatePages()
	const
{
	return itsCollateFlag;
}

inline void
JXPSPrinter::CollatePages
	(
	const bool doIt
	)
{
	itsCollateFlag = doIt;
}

#endif
