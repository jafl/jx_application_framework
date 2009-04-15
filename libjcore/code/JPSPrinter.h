/******************************************************************************
 JPSPrinter.h

	Interface for the JPSPrinter class

	Copyright © 1996-97 by John Lindal & Glenn Bach. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPSPrinter
#define _H_JPSPrinter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPagePrinter.h>
#include <JPSPrinterBase.h>

class JProgressDisplay;

class JPSPrinter : public JPagePrinter, public JPSPrinterBase
{
public:

	enum PaperType			// only append values because they are stored in files
	{
		kUSLetter = 0,
		kUSLegal,
		kUSExecutive,
		kA4Letter,
		kB5Letter
	};

public:

	JPSPrinter(const JFontManager* fontManager, const JColormap* colormap);

	virtual ~JPSPrinter();

	virtual JBoolean	WillPrintBlackWhite() const;

	// saving setup information

	void	ReadPSSetup(istream& input);
	void	WritePSSetup(ostream& output) const;

	// printing control

	virtual JBoolean	OpenDocument();
	virtual JBoolean	NewPage();
	virtual void		CloseDocument();
	virtual void		CancelDocument();

	// printing parameters

	PaperType	GetPaperType() const;
	void		SetPaperType(const PaperType type);

	JSize	GetCopyCount() const;
	void	SetCopyCount(const JSize count);

	JIndex	GetFirstPageToPrint() const;
	void	SetFirstPageToPrint(const JIndex index);

	JIndex	GetLastPageToPrint() const;
	void	SetLastPageToPrint(const JIndex index);

	JBoolean	WillPrintAllPages() const;
	void		PrintAllPages();

	// paper info

	JCoordinate	GetPaperWidth() const;
	JCoordinate	GetPaperHeight() const;
	JRect		GetPaperRect() const;

	static JCoordinate	GetPaperWidth(const PaperType type);
	static JCoordinate	GetPaperHeight(const PaperType type);

	// JPainter functions

	virtual JRect	SetClipRect(const JRect& r);
	virtual void	SetDashList(const JArray<JSize>& dashList, const JSize dashOffset = 0);

	virtual void	String(const JCoordinate left, const JCoordinate top,
						   const JCharacter* str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);
	virtual void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JCharacter* str,
						   const JCoordinate width = 0,
						   const HAlignment hAlign = kHAlignLeft,
						   const JCoordinate height = 0,
						   const VAlignment vAlign = kVAlignTop);

	virtual void	Point(const JCoordinate x, const JCoordinate y);

	virtual void	Line(const JCoordinate x1, const JCoordinate y1,
						 const JCoordinate x2, const JCoordinate y2);

	virtual void	Rect(const JCoordinate x, const JCoordinate y,
						 const JCoordinate w, const JCoordinate h);

	virtual void	Ellipse(const JCoordinate x, const JCoordinate y,
							const JCoordinate w, const JCoordinate h);

	virtual void	Arc(const JCoordinate x, const JCoordinate y,
						const JCoordinate w, const JCoordinate h,
						const JFloat startAngle, const JFloat deltaAngle);

	virtual void	Polygon(const JCoordinate left, const JCoordinate top,
							const JPolygon& poly);

	virtual void	Image(const JImage& image, const JRect& srcRect, const JRect& destRect);

protected:

	virtual const JPoint&	PSGetOrigin() const;
	virtual void			PSResetCoordinates();
	virtual JCoordinate		PSGetPrintableHeight() const;

	virtual JBoolean	PSShouldPrintCurrentPage() const;
	virtual void		PSPrintVersionComment(ostream& output);
	virtual void		PSPrintHeaderComments(ostream& output);
	virtual void		PSPrintSetupComments(ostream& output);

	virtual void	ImageOrientationChanged(const ImageOrientation orient);

private:

	PaperType	itsPaperType;

	JSize	itsCopyCount;
	JIndex	itsFirstPageIndex;
	JIndex	itsLastPageIndex;

	// used only during printing

	JIndex	itsPageCount;		// actual number of pages printed

	JProgressDisplay*	itsPG;

private:

	void	ClosePage();

	// not allowed

	JPSPrinter(const JPSPrinter& source);
	const JPSPrinter& operator=(const JPSPrinter& source);
};


// Global functions for JPSPrinter

istream& operator>>(istream& input, JPSPrinter::PaperType& type);
ostream& operator<<(ostream& output, const JPSPrinter::PaperType type);


/******************************************************************************
 Paper type

 ******************************************************************************/

inline JPSPrinter::PaperType
JPSPrinter::GetPaperType()
	const
{
	return itsPaperType;
}

inline void
JPSPrinter::SetPaperType
	(
	const PaperType type
	)
{
	itsPaperType = type;
	PSResetCoordinates();
}

/******************************************************************************
 Copies to print

 ******************************************************************************/

inline JSize
JPSPrinter::GetCopyCount()
	const
{
	return itsCopyCount;
}

inline void
JPSPrinter::SetCopyCount
	(
	const JSize count
	)
{
	itsCopyCount = count;
}

/******************************************************************************
 Page range to print

 ******************************************************************************/

inline JIndex
JPSPrinter::GetFirstPageToPrint()
	const
{
	return itsFirstPageIndex;
}

inline void
JPSPrinter::SetFirstPageToPrint
	(
	const JIndex index
	)
{
	itsFirstPageIndex = index;
}

inline JIndex
JPSPrinter::GetLastPageToPrint()
	const
{
	return itsLastPageIndex;
}

inline void
JPSPrinter::SetLastPageToPrint
	(
	const JIndex index
	)
{
	itsLastPageIndex = index;
}

inline JBoolean
JPSPrinter::WillPrintAllPages()
	const
{
	return JConvertToBoolean( itsFirstPageIndex == 0 && itsLastPageIndex == 0 );
}

inline void
JPSPrinter::PrintAllPages()
{
	itsFirstPageIndex = itsLastPageIndex = 0;
}

#endif
