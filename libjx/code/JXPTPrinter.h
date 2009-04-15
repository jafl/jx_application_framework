/******************************************************************************
 JXPTPrinter.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXPTPrinter
#define _H_JXPTPrinter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPTPrinter.h>

class JString;
class JXPTPrintSetupDialog;
class JXPTPageSetupDialog;

class JXPTPrinter : public JPTPrinter
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

	JXPTPrinter();

	virtual ~JXPTPrinter();

	virtual void	Print(const JCharacter* text);

	// saving setup information

	void	ReadXPTSetup(istream& input);
	void	WriteXPTSetup(ostream& output) const;

	// printing parameters

	Destination		GetDestination() const;
	void			SetDestination(const Destination dest,
								   const JCharacter* printCmd,
								   const JCharacter* fileName);

	const JString&	GetPrintCmd() const;
	void			SetPrintCmd(const JCharacter* cmd);

	const JString&	GetFileName() const;
	void			SetFileName(const JCharacter* name);

	// Page Setup and Print Setup dialogs

	void	BeginUserPageSetup();
	void	BeginUserPrintSetup();

protected:

	virtual JXPTPageSetupDialog*
		CreatePageSetupDialog(const JCharacter* printCmd,
							  const JSize pageWidth, const JSize pageHeight,
							  const JSize minPageHeight,
							  const JBoolean printReverseOrder);

	virtual JXPTPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JCharacter* printCmd, const JCharacter* fileName,
							   const JBoolean printLineNumbers);

	virtual JBoolean	EndUserPageSetup(const JBroadcaster::Message& message);
	virtual JBoolean	EndUserPrintSetup(const JBroadcaster::Message& message,
										  JBoolean* changed);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	Destination	itsDestination;
	JString*	itsPrintCmd;
	JString*	itsFileName;

	JXPTPageSetupDialog*	itsPageSetupDialog;
	JXPTPrintSetupDialog*	itsPrintSetupDialog;

private:

	// not allowed

	JXPTPrinter(const JXPTPrinter& source);
	const JXPTPrinter& operator=(const JXPTPrinter& source);
};

istream& operator>>(istream& input, JXPTPrinter::Destination& dest);
ostream& operator<<(ostream& output, const JXPTPrinter::Destination dest);


/******************************************************************************
 GetDestination

 ******************************************************************************/

inline JXPTPrinter::Destination
JXPTPrinter::GetDestination()
	const
{
	return itsDestination;
}

/******************************************************************************
 GetPrintCmd

 ******************************************************************************/

inline const JString&
JXPTPrinter::GetPrintCmd()
	const
{
	return *itsPrintCmd;
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

inline const JString&
JXPTPrinter::GetFileName()
	const
{
	return *itsFileName;
}

#endif
