/******************************************************************************
 TestSaveFileDialog.h

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestSaveFileDialog
#define _H_TestSaveFileDialog

#include <jx-af/jx/JXSaveFileDialog.h>

class JXRadioGroup;

class TestSaveFileDialog : public JXSaveFileDialog
{
public:

	enum SaveFormat
	{
		kGIFFormat = 1,		// matches ID's of radio buttons
		kPNGFormat,
		kJPEGFormat
	};

public:

	static TestSaveFileDialog*
		Create(const SaveFormat saveFormat,
			   const JString& prompt,
			   const JString& startName,
			   const JString& fileFilter = JString::empty,
			   const JString& message = JString::empty);

	~TestSaveFileDialog() override;

	SaveFormat	GetSaveFormat() const;

protected:

	TestSaveFileDialog(const SaveFormat saveFormat,
					   const JString& fileFilter);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	SaveFormat	itsSaveFormat;

// begin JXLayout

	JXRadioGroup* itsFormatRG;

// end JXLayout

private:

	void	BuildWindow(const JString& startName,
						const JString& prompt, const JString& message);

	void	HandleFormatChange(const JIndex id);
};

#endif
