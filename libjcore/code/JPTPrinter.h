/******************************************************************************
 JPTPrinter.h

	Copyright (C) 1999 by John Lindal.

 ******************************************************************************/

#ifndef _H_JPTPrinter
#define _H_JPTPrinter

#include "JPrinter.h"

class JString;

class JPTPrinter : public JPrinter
{
public:

	JPTPrinter();

	~JPTPrinter() override;

	virtual void	Print(const JString& text) = 0;

	// saving setup information

	void	ReadPTSetup(std::istream& input);
	void	WritePTSetup(std::ostream& output) const;

	// printing parameters

	JSize	GetCopyCount() const;
	void	SetCopyCount(const JSize count);

	JIndex	GetFirstPageToPrint() const;
	void	SetFirstPageToPrint(const JIndex index);

	JIndex	GetLastPageToPrint() const;
	void	SetLastPageToPrint(const JIndex index);

	bool	WillPrintAllPages() const;
	void	PrintAllPages();

	bool	WillPrintReverseOrder() const;
	void	ShouldPrintReverseOrder(const bool reverse);

	bool	WillPrintLineNumbers() const;
	void	ShouldPrintLineNumbers(const bool print);

	// page info

	JSize	GetPageWidth() const;
	void	SetPageWidth(const JSize width);

	JSize	GetPageHeight() const;
	void	SetPageHeight(const JSize height);

	JSize	GetTabWidth() const;
	void	SetTabWidth(const JSize width);

protected:

	bool	Print(const JString& text, std::ostream& output);

	virtual JSize	GetHeaderLineCount() const;
	virtual JSize	GetFooterLineCount() const;
	virtual void	PrintHeader(std::ostream& output, const JIndex pageIndex);
	virtual void	PrintFooter(std::ostream& output, const JIndex pageIndex);

private:

	JSize	itsCopyCount;			// for use by derived classes
	JIndex	itsFirstPageIndex;
	JIndex	itsLastPageIndex;
	JSize	itsPageWidth;			// characters
	JSize	itsPageHeight;			// lines
	JSize	itsTabWidth;			// characters
	bool	itsPrintReverseOrderFlag;
	bool	itsPrintLineNumberFlag;

private:

	void	InvertPageOrder(const JString& text, std::ostream& output) const;
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

inline bool
JPTPrinter::WillPrintAllPages()
	const
{
	return itsFirstPageIndex == 0 && itsLastPageIndex == 0;
}

inline void
JPTPrinter::PrintAllPages()
{
	itsFirstPageIndex = itsLastPageIndex = 0;
}

/******************************************************************************
 Reverse order printing

 ******************************************************************************/

inline bool
JPTPrinter::WillPrintReverseOrder()
	const
{
	return itsPrintReverseOrderFlag;
}

inline void
JPTPrinter::ShouldPrintReverseOrder
	(
	const bool reverse
	)
{
	itsPrintReverseOrderFlag = reverse;
}

/******************************************************************************
 Line numbers

 ******************************************************************************/

inline bool
JPTPrinter::WillPrintLineNumbers()
	const
{
	return itsPrintLineNumberFlag;
}

inline void
JPTPrinter::ShouldPrintLineNumbers
	(
	const bool print
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
