/******************************************************************************
 GMPTPrinter.h

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GMPTPrinter
#define _H_GMPTPrinter

#include <JXPTPrinter.h>
#include <JPrefObject.h>
#include <JString.h>

class GMPTPrintSetupDialog;

class GMPTPrinter : public JXPTPrinter, public JPrefObject
{
public:

	GMPTPrinter(JPrefsManager* prefsMgr, const JPrefID& id);

	virtual ~GMPTPrinter();

	JBoolean	WillPrintHeader() const;
	void		ShouldPrintHeader(const JBoolean print);

protected:

	virtual JXPTPrintSetupDialog*
		CreatePrintSetupDialog(const Destination destination,
							   const JCharacter* printCmd, const JCharacter* fileName,
							   const JBoolean printLineNumbers);

	virtual JBoolean	EndUserPrintSetup(const JBroadcaster::Message& message);
	virtual void		Receive(JBroadcaster* sender, const Message& message);

	virtual void		ReadPrefs(std::istream& input);
	virtual void		WritePrefs(std::ostream& output) const;

private:

	JBoolean	itsPrintHeaderFlag;

	GMPTPrintSetupDialog*	itsPrintSetupDialog;

private:

	// not allowed

	GMPTPrinter(const GMPTPrinter& source);
	const GMPTPrinter& operator=(const GMPTPrinter& source);

public:

	static const JCharacter* kPrintStarting;
	static const JCharacter* kPrintFinished;

	class PrintStarting : public JBroadcaster::Message
		{
		public:

			PrintStarting()
				:
				JBroadcaster::Message(kPrintStarting)
				{ };
		};

	class PrintFinished : public JBroadcaster::Message
		{
		public:

			PrintFinished()
				:
				JBroadcaster::Message(kPrintFinished)
				{ };
		};
};


/******************************************************************************
 PrintHeader

 ******************************************************************************/

inline JBoolean
GMPTPrinter::WillPrintHeader()
	const
{
	return itsPrintHeaderFlag;
}

inline void
GMPTPrinter::ShouldPrintHeader
	(
	const JBoolean print
	)
{
	itsPrintHeaderFlag = print;
}

#endif
