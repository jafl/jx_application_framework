/******************************************************************************
 GNBPTPrinter.h

	Copyright (C) 1999 by Glenn W. Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_GNBPTPrinter
#define _H_GNBPTPrinter

#include <JXPTPrinter.h>
#include <JPrefObject.h>
#include <JString.h>

class GNBPTPrintSetupDialog;

class GNBPTPrinter : public JXPTPrinter, public JPrefObject
{
public:

	GNBPTPrinter(JPrefsManager* prefsMgr, const JPrefID& id);

	virtual ~GNBPTPrinter();

	void		HasSelection(const JBoolean selection);

	void		ShouldPrintSelection(const JBoolean print);
	JBoolean	WillPrintSelection() const;

	void		ShouldPrintNotes(const JBoolean print);
	JBoolean	WillPrintNotes() const;

	void		ShouldPrintClosed(const JBoolean print);
	JBoolean	WillPrintClosed() const;

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

	JBoolean	itsHasSelection;
	JBoolean	itsPrintSelectionFlag;
	JBoolean	itsPrintNotesFlag;
	JBoolean	itsPrintClosedFlag;

	GNBPTPrintSetupDialog*	itsPrintSetupDialog;

private:

	// not allowed

	GNBPTPrinter(const GNBPTPrinter& source);
	const GNBPTPrinter& operator=(const GNBPTPrinter& source);

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
 HasSelection

 ******************************************************************************/

inline void
GNBPTPrinter::HasSelection
	(
	const JBoolean selection
	)
{
	itsHasSelection	= selection;
}

/******************************************************************************
 PrintSelection

 ******************************************************************************/

inline JBoolean
GNBPTPrinter::WillPrintSelection()
	const
{
	if (!itsHasSelection)
		{
		return kJFalse;
		}
	return itsPrintSelectionFlag;
}

inline void
GNBPTPrinter::ShouldPrintSelection
	(
	const JBoolean print
	)
{
	itsPrintSelectionFlag = print;
}

/******************************************************************************
 PrintNotes

 ******************************************************************************/

inline JBoolean
GNBPTPrinter::WillPrintNotes()
	const
{
	return itsPrintNotesFlag;
}

inline void
GNBPTPrinter::ShouldPrintNotes
	(
	const JBoolean print
	)
{
	itsPrintNotesFlag = print;
}

/******************************************************************************
 PrintClosed

 ******************************************************************************/

inline JBoolean
GNBPTPrinter::WillPrintClosed()
	const
{
	return itsPrintClosedFlag;
}

inline void
GNBPTPrinter::ShouldPrintClosed
	(
	const JBoolean print
	)
{
	itsPrintClosedFlag = print;
}

#endif
