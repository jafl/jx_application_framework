/******************************************************************************
 JPTPrinter.h

	Copyright © 1999 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JPTPrinter
#define _H_JPTPrinter

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JPrinter.h>

class JString;

class JPTPrinter : public JPrinter
{
public:

	JPTPrinter();

	virtual ~JPTPrinter();

	virtual void	Print(const JCharacter* text) = 0;

	// saving setup information

	void	ReadPTSetup(istream& input);
	void	WritePTSetup(ostream& output) const;

	// printing parameters

	JSize	GetCopyCount() const;
	void	SetCopyCount(const JSize count);

	JIndex	GetFirstPageToPrint() const;
	void	SetFirstPageToPrint(const JIndex index);

	JIndex	GetLastPageToPrint() const;
	void	SetLastPageToPrint(const JIndex index);

	JBoolean	WillPrintAllPages() const;
	void		PrintAllPages();

	JBoolean	WillPrintReverseOrder() const;
	void		ShouldPrintReverseOrder(const JBoolean reverse);

	JBoolean	WillPrintLineNumbers() const;
	void		ShouldPrintLineNumbers(const JBoolean print);

	// page info

	JSize	GetPageWidth() const;
	void	SetPageWidth(const JSize width);

	JSize	GetPageHeight() const;
	void	SetPageHeight(const JSize height);

	JSize	GetTabWidth() const;
	void	SetTabWidth(const JSize width);

protected:

	JBoolean	Print(const JCharacter* text, ostream& output);

	virtual JSize	GetHeaderLineCount() const;
	virtual JSize	GetFooterLineCount() const;
	virtual void	PrintHeader(ostream& output, const JIndex pageIndex);
	virtual void	PrintFooter(ostream& output, const JIndex pageIndex);

private:

	JSize		itsCopyCount;			// for use by derived classes
	JIndex		itsFirstPageIndex;
	JIndex		itsLastPageIndex;
	JSize		itsPageWidth;			// characters
	JSize		itsPageHeight;			// lines
	JSize		itsTabWidth;			// characters
	JBoolean	itsPrintReverseOrderFlag;
	JBoolean	itsPrintLineNumberFlag;

private:

	void	InvertPageOrder(const JString& text, ostream& output) const;

	// not allowed

	JPTPrinter(const JPTPrinter& source);
	const JPTPrinter& operator=(const JPTPrinter& source);
};


/******************************************************************************
 Copies to print

 ******************************************************************************/

inline JSize
JPTPrinter::GetCopyCount()
	const
{
	return itsCopyCount;
}

inline void
JPTPrinter::SetCopyCount
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
JPTPrinter::GetFirstPageToPrint()
	const
{
	return itsFirstPageIndex;
}

inline void
JPTPrinter::SetFirstPageToPrint
	(
	const JIndex index
	)
{
	itsFirstPageIndex = index;
}

inline JIndex
JPTPrinter::GetLastPageToPrint()
	const
{
	return itsLastPageIndex;
}

inline void
JPTPrinter::SetLastPageToPrint
	(
	const JIndex index
	)
{
	itsLastPageIndex = index;
}

inline JBoolean
JPTPrinter::WillPrintAllPages()
	const
{
	return JConvertToBoolean( itsFirstPageIndex == 0 && itsLastPageIndex == 0 );
}

inline void
JPTPrinter::PrintAllPages()
{
	itsFirstPageIndex = itsLastPageIndex = 0;
}

/******************************************************************************
 Reverse order printing

 ******************************************************************************/

inline JBoolean
JPTPrinter::WillPrintReverseOrder()
	const
{
	return itsPrintReverseOrderFlag;
}

inline void
JPTPrinter::ShouldPrintReverseOrder
	(
	const JBoolean reverse
	)
{
	itsPrintReverseOrderFlag = reverse;
}

/******************************************************************************
 Line numbers

 ******************************************************************************/

inline JBoolean
JPTPrinter::WillPrintLineNumbers()
	const
{
	return itsPrintLineNumberFlag;
}

inline void
JPTPrinter::ShouldPrintLineNumbers
	(
	const JBoolean print
	)
{
	itsPrintLineNumberFlag = print;
}

/******************************************************************************
 Get page size

	The page width is measured in characters, while the page height is
	measured in lines.

 ******************************************************************************/

inline JSize
JPTPrinter::GetPageWidth()
	const
{
	return itsPageWidth;
}

inline JSize
JPTPrinter::GetPageHeight()
	const
{
	return itsPageHeight;
}

/******************************************************************************
 Tab width

	The tab width is measured in characters.

 ******************************************************************************/

inline JSize
JPTPrinter::GetTabWidth()
	const
{
	return itsTabWidth;
}

inline void
JPTPrinter::SetTabWidth
	(
	const JSize width
	)
{
	itsTabWidth = width;
}

#endif
