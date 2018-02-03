/******************************************************************************
 JXPTPrinter.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPTPrinter
#define _H_JXPTPrinter

#include <JPTPrinter.h>
#include <JString.h>

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

	virtual void	Print(const JString& text);

	// saving setup information

	void	ReadXPTSetup(std::istream& input);
	void	WriteXPTSetup(std::ostream& output) const;

	// printing parameters

	Destination		GetDestination() const;
	void			SetDestination(const Destination dest,
								   const JString& printCmd,
								   const JString& fileName);

	const JString&	GetPrintCmd() const;
	void			SetPrintCmd(const JString& cmd);

	const JString&	GetFileName() const;
	void			SetFileName(const JString& name);

	// Page Setup and Print Setup dialogs

	void	BeginUserPageSetup();
	void	BeginUserPrintSetup();

protected:

	virtual JXPTPageSetupDialog*
		CreatePageSetupDialog(const JString& printCmd,
							  const JSize pageWidth, const JSize pageHeight,
							  const JSize minPageHeight,
							  const JBoolean printReverseOrder);

	virtual JXPTPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JString& printCmd, const JString& fileName,
							   const JBoolean printLineNumbers);

	virtual JBoolean	EndUserPageSetup(const JBroadcaster::Message& message);
	virtual JBoolean	EndUserPrintSetup(const JBroadcaster::Message& message,
										  JBoolean* changed);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	Destination	itsDestination;
	JString		itsPrintCmd;
	JString		itsFileName;

	JXPTPageSetupDialog*	itsPageSetupDialog;
	JXPTPrintSetupDialog*	itsPrintSetupDialog;

private:

	// not allowed

	JXPTPrinter(const JXPTPrinter& source);
	const JXPTPrinter& operator=(const JXPTPrinter& source);
};

std::istream& operator>>(std::istream& input, JXPTPrinter::Destination& dest);
std::ostream& operator<<(std::ostream& output, const JXPTPrinter::Destination dest);


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
	return itsPrintCmd;
}

/******************************************************************************
 GetFileName

 ******************************************************************************/

inline const JString&
JXPTPrinter::GetFileName()
	const
{
	return itsFileName;
}

#endif
