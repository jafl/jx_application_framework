/******************************************************************************
 GMFilterAction.cc

	Copyright © 2000 by Glenn W. Bach.  All rights reserved.

 *****************************************************************************/

#include <GMFilterAction.h>

#include <jAssert.h>

const JFileVersion kCurrentSetupVersion	= 0;

/******************************************************************************
 Constructor

 *****************************************************************************/

GMFilterAction::GMFilterAction()
	:
	itsIsMailbox(kJTrue),
	itsIsCopying(kJFalse)
{
}

GMFilterAction::GMFilterAction
	(
	const JBoolean		mailbox,
	const JBoolean		copy,
	const JCharacter*	dest
	)
	:
	itsIsMailbox(mailbox),
	itsIsCopying(copy),
	itsDestination(dest)
{
}

GMFilterAction::GMFilterAction
	(
	const GMFilterAction& source
	)
	:
	itsIsMailbox(source.itsIsMailbox),
	itsIsCopying(source.itsIsCopying),
	itsDestination(source.itsDestination)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

GMFilterAction::~GMFilterAction()
{
}

/******************************************************************************
 Mailbox (public)

 ******************************************************************************/

JBoolean
GMFilterAction::IsMailbox()
	const
{
	return itsIsMailbox;
}

void
GMFilterAction::ShouldBeMailbox
	(
	const JBoolean mailbox
	)
{
	itsIsMailbox	= mailbox;
}

/******************************************************************************
 Copy (public)

 ******************************************************************************/

JBoolean
GMFilterAction::IsCopying()
	const
{
	return itsIsCopying;
}

void
GMFilterAction::ShouldCopy
	(
	const JBoolean copy
	)
{
	itsIsCopying	= copy;
}

/******************************************************************************
 Destination (public)

 ******************************************************************************/

const JString&
GMFilterAction::GetDestination()
	const
{
	return itsDestination;
}

void
GMFilterAction::SetDestination
	(
	const JCharacter* dest
	)
{
	itsDestination	= dest;
}

/******************************************************************************
 ReadSetup (public)

 ******************************************************************************/

void
GMFilterAction::ReadSetup
	(
	istream& input
	)
{
	JFileVersion version;
	input >> version;
	if (version > kCurrentSetupVersion)
		{
		return;
		}
	input >> itsIsMailbox;
	input >> itsIsCopying;
	input >> itsDestination;
}

/******************************************************************************
 WriteSetup (private)

 ******************************************************************************/

void
GMFilterAction::WriteSetup
	(
	ostream& output
	)
	const
{
	output << ' ' << kCurrentSetupVersion << ' ';
	output << itsIsMailbox << ' ';
	output << itsIsCopying << ' ';
	output << itsDestination << ' ';
}
