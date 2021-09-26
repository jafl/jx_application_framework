/******************************************************************************
 JPSPrinter.h

	Interface for the JPSPrinter class

	Copyright (C) 1996-97 by John Lindal & Glenn Bach.

 ******************************************************************************/

#ifndef _H_JPSPrinter
#define _H_JPSPrinter

#include "jx-af/jcore/JPagePrinter.h"
#include "jx-af/jcore/JPSPrinterBase.h"

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

	JPSPrinter(JFontManager* fontManager);

	virtual ~JPSPrinter();

	virtual bool	WillPrintBlackWhite() const;

	// saving setup information

	void	ReadPSSetup(std::istream& input);
	void	WritePSSetup(std::ostream& output) const;

	// printing control

	virtual bool	OpenDocument();
	virtual bool	NewPage();
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

	bool	WillPrintAllPages() const;
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

	virtual void	StringNoSubstitutions(
						const JCoordinate left, const JCoordinate top,
						const JString& str);
	virtual void	String(const JFloat angle, const JCoordinate left,
						   const JCoordinate top, const JString& str,
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

	virtual bool	PSShouldPrintCurrentPage() const;
	virtual void		PSPrintVersionComment(std::ostream& output);
	virtual void		PSPrintHeaderComments(std::ostream& output);
	virtual void		PSPrintSetupComments(std::ostream& output);

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
};


// Global functions for JPSPrinter

std::istream& operator>>(std::istream& input, JPSPrinter::PaperType& type);
std::ostream& operator<<(std::ostream& output, const JPSPrinter::PaperType type);


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

inline bool
JPSPrinter::WillPrintAllPages()
	const
{
	return itsFirstPageIndex == 0 && itsLastPageIndex == 0;
}

inline void
JPSPrinter::PrintAllPages()
{
	itsFirstPageIndex = itsLastPageIndex = 0;
}

#endif
