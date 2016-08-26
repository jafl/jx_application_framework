/******************************************************************************
 JXEPSPrinter.h

	Interface for the JXEPSPrinter class

	Copyright � 1997-99 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JXEPSPrinter
#define _H_JXEPSPrinter

#include <JEPSPrinter.h>

class JXDisplay;
class JXImage;
class JXImagePainter;
class JXEPSPrintSetupDialog;

class JXEPSPrinter : public JEPSPrinter
{
public:

	JXEPSPrinter(JXDisplay* display);

	virtual ~JXEPSPrinter();

	// saving setup information

	void	ReadXEPSSetup(istream& input);
	void	WriteXEPSSetup(ostream& output) const;

	// preview

	virtual JPainter&	GetPreviewPainter(const JRect& bounds);

	// Print Setup dialog

	void	BeginUserPrintSetup();

protected:

	virtual JBoolean	GetPreviewImage(const JImage** image) const;
	virtual void		DeletePreviewData();

	virtual JXEPSPrintSetupDialog*
		CreatePrintSetupDialog(const JCharacter* fileName,
							   const JBoolean preview, const JBoolean bw);

	virtual JBoolean	EndUserPrintSetup(const JBroadcaster::Message& message,
										  JBoolean* changed);

	virtual void	Receive(JBroadcaster* sender, const Message& message);

private:

	JXDisplay*	itsDisplay;		// not owned

	JXImage*		itsPreviewImage;
	JXImagePainter*	itsPreviewPainter;

	JXEPSPrintSetupDialog*	itsPrintSetupDialog;

private:

	// not allowed

	JXEPSPrinter(const JXEPSPrinter& source);
	const JXEPSPrinter& operator=(const JXEPSPrinter& source);
};

#endif
