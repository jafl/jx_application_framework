/******************************************************************************
 TestTextEditDocument.h

	Interface for the TestTextEditDocument class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestTextEditDocument
#define _H_TestTextEditDocument

#include <jx-af/jx/JXFileDocument.h>

class JStyledText;
class JXTextMenu;
class TestTextEditor;

class TestTextEditDocument : public JXFileDocument
{
public:

	TestTextEditDocument(JXDirector* supervisor);
	TestTextEditDocument(JXDirector* supervisor, const JString& fileName, const bool privateFmt);

	~TestTextEditDocument();

protected:

	void	ReadFile(const JString& fileName);
	void	WriteTextFile(std::ostream& output, const bool safetySave) const override;
	void	DiscardChanges() override;
	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	enum EmulatorType
	{
		kNoEmulator = 1,
		kVIEmulator
	};

private:

	JStyledText*	itsText;
	TestTextEditor*	itsTextEditor1;		// owned by its enclosure
	TestTextEditor*	itsTextEditor2;		// owned by its enclosure
	JXTextMenu*		itsFileMenu;		// owned by menu bar
	JXTextMenu*		itsEmulatorMenu;	// owned by menu bar

	EmulatorType	itsEmulatorType;
	bool			itsWritePrivateFmtFlag;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const bool fileWritable);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);

	void	OpenFiles();
	void	RevertToSaved();

	void	UpdateEmulatorMenu();
	void	HandleEmulatorMenu(const JIndex item);
};

#endif
