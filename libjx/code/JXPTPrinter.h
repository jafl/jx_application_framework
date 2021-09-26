/******************************************************************************
 JXPTPrinter.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXPTPrinter
#define _H_JXPTPrinter

#include <jx-af/jcore/JPTPrinter.h>
#include <jx-af/jcore/JString.h>

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

	virtual void	Print(const JString& text) override;

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
							  const bool printReverseOrder);

	virtual JXPTPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JString& printCmd, const JString& fileName,
							   const bool printLineNumbers);

	virtual bool	EndUserPageSetup(const JBroadcaster::Message& message);
	virtual bool	EndUserPrintSetup(const JBroadcaster::Message& message,
										  bool* changed);

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	Destination	itsDestination;
	JString		itsPrintCmd;
	JString		itsFileName;

	JXPTPageSetupDialog*	itsPageSetupDialog;
	JXPTPrintSetupDialog*	itsPrintSetupDialog;
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
