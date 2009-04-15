/******************************************************************************
 JChooseSaveFile.cpp

	Abstract base class to let user select and save files.

	Derived classes must follow these guidelines:

		ChooseFile		- verify that the file exists
		ChooseFiles		- verify that the files exist
		ChooseRPath		- verify that the path exists
		ChooseRWPath	- verify that the path exists and is writable
		SaveFile		- warn if the path is not writable,
							the file exists and is not writable,
							or the file exists and is writable

	Copyright © 1994-98 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <JChooseSaveFile.h>

/******************************************************************************
 Constructor

 ******************************************************************************/

JChooseSaveFile::JChooseSaveFile()
{
}

/******************************************************************************
 Destructor

 ******************************************************************************/

JChooseSaveFile::~JChooseSaveFile()
{
}
