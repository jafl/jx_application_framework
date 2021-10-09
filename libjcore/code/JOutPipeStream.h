/******************************************************************************
 JOutPipeStream.h

	Interface for the JOutPipeStream class

	Copyright (C) 1998 by John Lindal.

 *****************************************************************************/

#ifndef _H_JOutPipeStream
#define _H_JOutPipeStream

#include "jx-af/jcore/JOutPipeStreambuf.h"

class JOutPipeStream : public std::ostream
{
public:

	JOutPipeStream(const int fd, const bool close);

	~JOutPipeStream() override;

	void	close();

	bool	WillClosePipe() const;
	void	ShouldClosePipe(const bool close = true);

private:

	JOutPipeStreambuf<char>	itsBuffer;

private:

	// not allowed

	JOutPipeStream(const JOutPipeStream&) = delete;
	JOutPipeStream& operator=(const JOutPipeStream&) = delete;
};


/******************************************************************************
 Closing the underlying pipe

 *****************************************************************************/

inline bool
JOutPipeStream::WillClosePipe()
	const
{
	return itsBuffer.WillClosePipe();
}

inline void
JOutPipeStream::ShouldClosePipe
	(
	const bool close
	)
{
	itsBuffer.ShouldClosePipe(close);
}

#endif
