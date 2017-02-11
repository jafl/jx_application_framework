/******************************************************************************
 TestTextEditDocument.h

	Interface for the TestTextEditDocument class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestTextEditDocument
#define _H_TestTextEditDocument

#include <JXFileDocument.h>

class JXTextMenu;
class TestTextEditor;

class TestTextEditDocument : public JXFileDocument
{
public:

	TestTextEditDocument(JXDirector* supervisor);
	TestTextEditDocument(JXDirector* supervisor, const JString& fileName);

	virtual ~TestTextEditDocument();

protected:

	void			ReadFile(const JString& fileName);
	virtual void	WriteTextFile(std::ostream& output, const JBoolean safetySave) const;
	virtual void	DiscardChanges();
	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	enum DataType
	{
		kUnknownType,
		kPlainText,
		kHTML
	};

private:

	DataType		itsDataType;
	TestTextEditor*	itsTextEditor;	// owned by its enclosure
	JXTextMenu*		itsFileMenu;	// owned by menu bar

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow(const JBoolean fileWritable);

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);

	void	OpenFiles();
	void	RevertToSaved();

	// not allowed

	TestTextEditDocument(const TestTextEditDocument& source);
	const TestTextEditDocument& operator=(const TestTextEditDocument& source);
};

#endif
