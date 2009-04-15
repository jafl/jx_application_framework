/******************************************************************************
 JXPSPrinter.h

	Interface for the JXPSPrinter class

	Copyright © 1996-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPSPrinter
#define _H_JXPSPrinter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPSPrinter.h>

class JXDisplay;
class JXColormap;
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

	JXPSPrinter(const JXDisplay* display, const JXColormap* colormap);

	virtual ~JXPSPrinter();

	// saving setup information

	void	ReadXPSSetup(istream& input);
	void	WriteXPSSetup(ostream& output) const;

	// printing control

	virtual JBoolean	OpenDocument();
	virtual void		CloseDocument();

	// printing parameters

	Destination		GetDestination() const;
	void			SetDestination(const Destination dest,
								   const JCharacter* printCmd,
								   const JCharacter* fileName);

	const JString&	GetPrintCmd() const;
	void			SetPrintCmd(const JCharacter* cmd);

	const JString&	GetFileName() const;
	void			SetFileName(const JCharacter* name);

	JBoolean	WillCollatePages() const;
	void		CollatePages(const JBoolean doIt);

	// Page Setup and Print Setup dialogs

	void	BeginUserPageSetup();
	void	BeginUserPrintSetup();

protected:

	virtual JXPSPageSetupDialog*
		CreatePageSetupDialog(const PaperType paper, const ImageOrientation orient);

	virtual JXPSPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JCharacter* printCmd, const JCharacter* fileName,
							   const JBoolean collate, const JBoolean bw);

	virtual JBoolean	EndUserPageSetup(const JBroadcaster::Message& message);
	virtual JBoolean	EndUserPrintSetup(const JBroadcaster::Message& message,
										  JBoolean* changed);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	Destination	itsDestination;
	JString*	itsPrintCmd;
	JString*	itsFileName;
	JBoolean	itsCollateFlag;

	JXPSPageSetupDialog*	itsPageSetupDialog;
	JXPSPrintSetupDialog*	itsPrintSetupDialog;

private:

	// not allowed

	JXPSPrinter(const JXPSPrinter& source);
	const JXPSPrinter& operator=(const JXPSPrinter& source);
};

istream& operator>>(istream& input, JXPSPrinter::Destination& dest);
ostream& operator<<(ostream& output, const JXPSPrinter::Destination dest);


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
	return *itsPrintCmd;
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

inline const JString&
JXPSPrinter::GetFileName()
	const
{
	return *itsFileName;
}

/******************************************************************************
 Collated pages

 ******************************************************************************/

inline JBoolean
JXPSPrinter::WillCollatePages()
	const
{
	return itsCollateFlag;
}

inline void
JXPSPrinter::CollatePages
	(
	const JBoolean doIt
	)
{
	itsCollateFlag = doIt;
}

#endif
