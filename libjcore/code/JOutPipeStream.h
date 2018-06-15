/******************************************************************************
 JOutPipeStream.h

	Interface for the JOutPipeStream class

	Copyright (C) 1998 by John Lindal.

 *****************************************************************************/

#ifndef _H_JOutPipeStream
#define _H_JOutPipeStream

#include "JOutPipeStreambuf.h"

class JOutPipeStream : public std::ostream
{
public:

	JOutPipeStream(const int fd, const JBoolean close);

	virtual ~JOutPipeStream();

	void	close();

	JBoolean	WillClosePipe() const;
	void		ShouldClosePipe(const JBoolean close = kJTrue);

private:

	JOutPipeStreambuf<char>	itsBuffer;

private:

	// not allowed

	JOutPipeStream(const JOutPipeStream& source);
	const JOutPipeStream& operator=(const JOutPipeStream& source);
};


/******************************************************************************
 Closing the underlying pipe

 *****************************************************************************/

inline JBoolean
JOutPipeStream::WillClosePipe()
	const
{
	return itsBuffer.WillClosePipe();
}

inline void
JOutPipeStream::ShouldClosePipe
	(
	const JBoolean close
	)
{
	itsBuffer.ShouldClosePipe(close);
}

#endif
