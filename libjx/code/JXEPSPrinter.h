/******************************************************************************
 JXEPSPrinter.h

	Interface for the JXEPSPrinter class

	Copyright (C) 1997-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JXEPSPrinter
#define _H_JXEPSPrinter

#include <jx-af/jcore/JEPSPrinter.h>

class JXDisplay;
class JXImage;
class JXImagePainter;
class JXEPSPrintSetupDialog;

class JXEPSPrinter : public JEPSPrinter
{
public:

	JXEPSPrinter(JXDisplay* display);

	~JXEPSPrinter() override;

	// saving setup information

	void	ReadXEPSSetup(std::istream& input);
	void	WriteXEPSSetup(std::ostream& output) const;

	// preview

	JPainter&	GetPreviewPainter(const JRect& bounds) override;

	// Print Setup dialog

	void	BeginUserPrintSetup();

protected:

	bool	GetPreviewImage(const JImage** image) const override;
	void	DeletePreviewData() override;

	virtual JXEPSPrintSetupDialog*
		CreatePrintSetupDialog(const JString& fileName,
							   const bool preview, const bool bw);

	virtual bool	EndUserPrintSetup(const JBroadcaster::Message& message,
									  bool* changed);

	void	Receive(JBroadcaster* sender, const Message& message) override;

private:

	JXDisplay*	itsDisplay;		// not owned

	JXImage*		itsPreviewImage;
	JXImagePainter*	itsPreviewPainter;

	JXEPSPrintSetupDialog*	itsPrintSetupDialog;
};

#endif
