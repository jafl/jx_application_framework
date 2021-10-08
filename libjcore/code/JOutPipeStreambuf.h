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

	using int_type = typename std::basic_streambuf<_CharT, _Traits>::int_type;

public:

	JOutPipeStreambuf(const int fd, const bool close);

	~JOutPipeStreambuf() override;

	int		GetDescriptor();

	bool	WillClosePipe() const;
	void	ShouldClosePipe(const bool close = true);

	void	close();

protected:

	std::streamsize	xsputn(const _CharT* s, std::streamsize n) override;
	int_type		overflow(int_type c) override;

private:

	const int	itsDescriptor;
	bool		itsCloseFlag;	// true => close when we are destructed
};

#include "JOutPipeStreambuf.tmpl"

#endif
