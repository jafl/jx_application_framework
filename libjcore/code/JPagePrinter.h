/******************************************************************************
 JPagePrinter.h

	Interface for the JPagePrinter class

	Copyright (C) 1996-99 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPagePrinter
#define _H_JPagePrinter

#include "JPainter.h"
#include "JPrinter.h"

class JPagePrinter : public JPainter, public JPrinter
{
public:

	enum ImageOrientation	// only append values because they are stored in files
	{
		kPortrait = 0,
		kLandscape
	};

public:

	JPagePrinter(JFontManager* fontManager, const JRect& defaultClipRect);

	virtual ~JPagePrinter();

	virtual JBoolean	WillPrintBlackWhite() const = 0;

	ImageOrientation	GetOrientation() const;
	void				SetOrientation(const ImageOrientation orient);

	// page info

	JIndex		GetPageIndex() const;

	JCoordinate	GetPageWidth() const;
	JCoordinate	GetPageHeight() const;
	JRect		GetPageRect() const;
	void		SetPageSize(const JCoordinate w, const JCoordinate h);

	void		GetPageInfo(JIndex* pageIndex, JCoordinate* w, JCoordinate* h) const;

	// printing control

	virtual JBoolean	OpenDocument() = 0;
	virtual JBoolean	NewPage() = 0;
	virtual void		CloseDocument() = 0;
	virtual void		CancelDocument() = 0;

	// page header and footer

	void	LockHeader(const JCoordinate headerHeight);
	void	LockFooter(const JCoordinate footerHeight);

protected:

	void	SetPageIndex(const JIndex pageIndex);

	virtual void	ImageOrientationChanged(const ImageOrientation orient);

private:

	ImageOrientation	itsOrientation;

	JIndex		itsPageIndex;
	JCoordinate	itsPageWidth;
	JCoordinate	itsPageHeight;

private:

	// not allowed

	JPagePrinter(const JPagePrinter& source);
	const JPagePrinter& operator=(const JPagePrinter& source);
};


// Global functions for JPagePrinter

std::istream& operator>>(std::istream& input, JPagePrinter::ImageOrientation& orientation);
std::ostream& operator<<(std::ostream& output, const JPagePrinter::ImageOrientation orientation);


/******************************************************************************
 Image orientation

 ******************************************************************************/

inline JPagePrinter::ImageOrientation
JPagePrinter::GetOrientation()
	const
{
	return itsOrientation;
}

inline void
JPagePrinter::SetOrientation
	(
	const ImageOrientation orient
	)
{
	itsOrientation = orient;
	ImageOrientationChanged(itsOrientation);
}

/******************************************************************************
 Page info

 ******************************************************************************/

inline JIndex
JPagePrinter::GetPageIndex()
	const
{
	return itsPageIndex;
}

inline void
JPagePrinter::SetPageIndex		// protected
	(
	const JIndex pageIndex
	)
{
	itsPageIndex = pageIndex;
}

inline JCoordinate
JPagePrinter::GetPageWidth()
	const
{
	return itsPageWidth;
}

inline JCoordinate
JPagePrinter::GetPageHeight()
	const
{
	return itsPageHeight;
}

inline JRect
JPagePrinter::GetPageRect()
	const
{
	return JRect(0, 0, itsPageHeight, itsPageWidth);
}

inline void
JPagePrinter::SetPageSize
	(
	const JCoordinate w,
	const JCoordinate h
	)
{
	itsPageWidth  = w;
	itsPageHeight = h;
}

inline void
JPagePrinter::GetPageInfo
	(
	JIndex*			pageIndex,
	JCoordinate*	w,
	JCoordinate*	h
	)
	const
{
	*pageIndex = itsPageIndex;
	*w = itsPageWidth;
	*h = itsPageHeight;
}

#endif
