/******************************************************************************
 TestImageDirector.h

	Interface for the TestImageDirector class

	Written by John Lindal.

 ******************************************************************************/

#ifndef _H_TestImageDirector
#define _H_TestImageDirector

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JXWindowDirector.h>
#include <JString.h>
#include <JImage.h>		// need definition of FileType

class JXTextMenu;
class JXImageWidget;
class JXPSPrinter;
class JXEPSPrinter;

class TestImageDirector : public JXWindowDirector
{
public:

	TestImageDirector(JXDirector* supervisor);

	virtual ~TestImageDirector();

protected:

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JString				itsFileName;

	JXImageWidget*		itsImageWidget;			// owned by its enclosure
	JXTextMenu*			itsFileMenu;			// owned by the menu bar

	JXPSPrinter*		itsPSPrinter;
	JXEPSPrinter*		itsEPSPrinter;

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

	// not allowed

	TestImageDirector(const TestImageDirector& source);
	const TestImageDirector& operator=(const TestImageDirector& source);
};

#endif
