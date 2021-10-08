/******************************************************************************
 TestSaveFileDialog.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestSaveFileDialog
#define _H_TestSaveFileDialog

#include <jx-af/jx/JXSaveFileDialog.h>
#include "TestChooseSaveFile.h"		// need definition of SaveFormat

class JXRadioGroup;

class TestSaveFileDialog : public JXSaveFileDialog
{
public:

	static TestSaveFileDialog*
		Create(JXDirector* supervisor, JDirInfo* dirInfo,
			   const JString& fileFilter,
			   const TestChooseSaveFile::SaveFormat saveFormat,
			   const JString& origName, const JString& prompt,
			   const JString& message = JString::empty);

	~TestSaveFileDialog() override;

	TestChooseSaveFile::SaveFormat	GetSaveFormat() const;

protected:

	TestSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
					   const JString& fileFilter,
					   const TestChooseSaveFile::SaveFormat saveFormat);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	TestChooseSaveFile::SaveFormat	itsSaveFormat;

// begin JXLayout

	JXRadioGroup* itsFormatRG;

// end JXLayout

private:

	void	BuildWindow(const JString& origName, const JString& prompt,
						const JString& message);

	void	HandleFormatChange(const JIndex id);
};

#endif
