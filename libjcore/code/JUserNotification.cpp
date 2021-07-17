/******************************************************************************
 JUserNotification.cpp

	Abstract base class for notifying the user of something.

	IsSilent() indicates whether or not DisplayMessage and ReportError
	should actually display the message.

	Copyright (C) 1994-96 by John Lindal.

 ******************************************************************************/

#include "JUserNotification.h"

bool JUserNotification::theBreakCROnlyFlag = false;

/******************************************************************************
 Constructor

 ******************************************************************************/

JUserNotification::JUserNotification()
{
	itsSilenceFlag = false;
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JUserNotification::~JUserNotification()
{
}
