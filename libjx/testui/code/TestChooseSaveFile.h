/******************************************************************************
 TestChooseSaveFile.h

	Interface for the TestChooseSaveFile class.

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestChooseSaveFile
#define _H_TestChooseSaveFile

#include <jx-af/jx/JXChooseSaveFile.h>

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

	~TestChooseSaveFile() override;

	SaveFormat	GetSaveFormat() const;

protected:

	void	SetChooseFileFilters(JDirInfo* dirInfo) override;

	JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JString& fileFilter, const JString& origName,
						 const JString& prompt, const JString& message) override;

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	TestSaveFileDialog*	itsTestSaveDialog;
	SaveFormat			itsSaveFormat;
};

#endif
