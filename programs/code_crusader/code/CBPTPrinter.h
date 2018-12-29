/******************************************************************************
 CBPTPrinter.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_CBPTPrinter
#define _H_CBPTPrinter

#include <JXPTPrinter.h>
#include <JPrefObject.h>
#include <JString.h>

class CBPTPrintSetupDialog;

class CBPTPrinter : public JXPTPrinter, public JPrefObject
{
public:

	CBPTPrinter();

	virtual ~CBPTPrinter();

	void	SetHeaderName(const JString& name);

	// printing parameters

	JBoolean	WillPrintHeader() const;
	void		ShouldPrintHeader(const JBoolean print);

protected:

	virtual void	ReadPrefs(std::istream& input);
	virtual void	WritePrefs(std::ostream& output) const;

	virtual JSize	GetHeaderLineCount() const;
	virtual void	PrintHeader(std::ostream& output, const JIndex pageIndex);

	virtual JXPTPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JString& printCmd, const JString& fileName,
							   const JBoolean printLineNumbers);

	virtual JBoolean	EndUserPrintSetup(const JBroadcaster::Message& message,
										  JBoolean* changed);

private:

	JBoolean	itsPrintHeaderFlag;
	JString		itsHeaderName;

	CBPTPrintSetupDialog*	itsCBPrintSetupDialog;

private:

	// not allowed

	CBPTPrinter(const CBPTPrinter& source);
	const CBPTPrinter& operator=(const CBPTPrinter& source);
};


/******************************************************************************
 SetHeaderName

 ******************************************************************************/

inline void
CBPTPrinter::SetHeaderName
	(
	const JString& name
	)
{
	itsHeaderName = name;
}

/******************************************************************************
 SetHeaderName

 ******************************************************************************/

inline JBoolean
CBPTPrinter::WillPrintHeader()
	const
{
	return itsPrintHeaderFlag;
}

inline void
CBPTPrinter::ShouldPrintHeader
	(
	const JBoolean print
	)
{
	itsPrintHeaderFlag = print;
}

#endif
