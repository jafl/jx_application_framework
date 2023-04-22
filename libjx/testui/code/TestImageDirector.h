/******************************************************************************
 TestImageDirector.h

	Interface for the TestImageDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestImageDirector
#define _H_TestImageDirector

#include <jx-af/jx/JXWindowDirector.h>
#include <jx-af/jcore/JImage.h>		// for FileType

class JXTextMenu;
class JXImageWidget;
class JXPSPrinter;
class JXEPSPrinter;

class TestImageDirector : public JXWindowDirector
{
public:

	TestImageDirector(JXDirector* supervisor);

	~TestImageDirector() override;

	void	SetFileName(const JString& name);

protected:

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JString			itsFileName;

	JXImageWidget*	itsImageWidget;			// owned by its enclosure
	JXTextMenu*		itsFileMenu;			// owned by the menu bar

	JXPSPrinter*	itsPSPrinter;
	JXEPSPrinter*	itsEPSPrinter;

// begin JXLayout


// end JXLayout

private:

	void	BuildWindow();

	void	UpdateFileMenu();
	void	HandleFileMenu(const JIndex item);

	void	LoadImage();
	void	SaveImage(const JImage::FileType type) const;
	void	SaveMask() const;

	void	CopyImage() const;
	void	PasteImage();

	void	PrintPS() const;
	void	PrintEPS() const;
};


/*****************************************************************************
 SetFileName

 ******************************************************************************/

inline void
TestImageDirector::SetFileName
	(
	const JString& name
	)
{
	itsFileName = name;
}

#endif
