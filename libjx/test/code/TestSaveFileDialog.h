/******************************************************************************
 TestSaveFileDialog.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestSaveFileDialog
#define _H_TestSaveFileDialog

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXSaveFileDialog.h>
#include "TestChooseSaveFile.h"		// need definition of SaveFormat

class JXRadioGroup;

class TestSaveFileDialog : public JXSaveFileDialog
{
public:

	static TestSaveFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JCharacter* fileFilter,
			   const TestChooseSaveFile::SaveFormat saveFormat,
			   const JCharacter* origName, const JCharacter* prompt,
			   const JCharacter* message = NULL);

	virtual ~TestSaveFileDialog();

	TestChooseSaveFile::SaveFormat	GetSaveFormat() const;

protected:

	TestSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					   const JCharacter* fileFilter,
					   const TestChooseSaveFile::SaveFormat saveFormat);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	TestChooseSaveFile::SaveFormat	itsSaveFormat;

// begin JXLayout

    JXRadioGroup* itsFormatRG;

// end JXLayout

private:

	void	BuildWindow(const JCharacter* origName, const JCharacter* prompt,
						const JCharacter* message = NULL);

	void	HandleFormatChange(const JIndex id);

	// not allowed

	TestSaveFileDialog(const TestSaveFileDialog& source);
	const TestSaveFileDialog& operator=(const TestSaveFileDialog& source);
};

#endif
