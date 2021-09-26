/******************************************************************************
 JOutPipeStreambuf.h

	Copyright (C) 2006 by John Lindal.

 *****************************************************************************/

#ifndef _H_JOutPipeStreambuf
#define _H_JOutPipeStreambuf

#include "jx-af/jcore/jTypes.h"

template<typename _CharT, typename _Traits = std::char_traits<_CharT> >
class JOutPipeStreambuf : public std::basic_streambuf<_CharT, _Traits>
{
public:

	typedef typename std::basic_streambuf<_CharT, _Traits>::int_type int_type;

public:

	JOutPipeStreambuf(const int fd, const bool close);

	virtual	~JOutPipeStreambuf();

	int			GetDescriptor();

	bool	WillClosePipe() const;
	void		ShouldClosePipe(const bool close = true);

	void		close();

protected:

	virtual std::streamsize	xsputn(const _CharT* s, std::streamsize n);
	virtual int_type		overflow(int_type c);

private:

	const int	itsDescriptor;
	bool	itsCloseFlag;	// true => close when we are destructed
};

#include "JOutPipeStreambuf.tmpl"

#endif
