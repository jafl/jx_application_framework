/******************************************************************************
 TestChooseSaveFile.h

	Interface for the TestChooseSaveFile class.

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestChooseSaveFile
#define _H_TestChooseSaveFile

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXChooseSaveFile.h>

class TestSaveFileDialog;

class TestChooseSaveFile : public JXChooseSaveFile
{
public:

	enum SaveFormat
	{
		kGIFFormat = 1,		// matches ID's of radio buttons
		kPNGFormat,
		kJPEGFormat
	};

public:

	TestChooseSaveFile();

	virtual ~TestChooseSaveFile();

	SaveFormat	GetSaveFormat() const;

protected:

	virtual void	SetChooseFileFilters(JDirInfo* dirInfo);

	virtual JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JCharacter* fileFilter, const JCharacter* origName,
						 const JCharacter* prompt, const JCharacter* message);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	TestSaveFileDialog*	itsTestSaveDialog;
	SaveFormat			itsSaveFormat;

private:

	// not allowed

	TestChooseSaveFile(const TestChooseSaveFile& source);
	const TestChooseSaveFile& operator=(const TestChooseSaveFile& source);
};

#endif
