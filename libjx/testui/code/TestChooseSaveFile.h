/******************************************************************************
 TestChooseSaveFile.h

	Interface for the TestChooseSaveFile class.

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestChooseSaveFile
#define _H_TestChooseSaveFile

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

	virtual void	SetChooseFileFilters(JDirInfo* dirInfo) override;

	virtual JXSaveFileDialog*
	CreateSaveFileDialog(JXDirector* supervisor, JDirInfo* dirInfo,
						 const JString& fileFilter, const JString& origName,
						 const JString& prompt, const JString& message) override;

	virtual void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	TestSaveFileDialog*	itsTestSaveDialog;
	SaveFormat			itsSaveFormat;

private:

	// not allowed

	TestChooseSaveFile(const TestChooseSaveFile& source);
	const TestChooseSaveFile& operator=(const TestChooseSaveFile& source);
};

#endif
