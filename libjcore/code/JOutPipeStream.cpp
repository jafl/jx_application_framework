/******************************************************************************
 JOutPipeStream.cpp

	This class provides an ostream interface to the write end of a pipe.

	BASE CLASS = ostream

	Copyright © 1998 by John Lindal. All rights reserved.

 *****************************************************************************/

#include <JOutPipeStream.h>
#include <jStreamUtil.h>
#include <jAssert.h>

/******************************************************************************
 Constructor

 *****************************************************************************/

JOutPipeStream::JOutPipeStream
	(
	const int		fd,
	const JBoolean	close
	)
	:
	ios(&itsBuffer),
	ostream(&itsBuffer),
	itsBuffer(fd, close)
{
}

/******************************************************************************
 Destructor

 *****************************************************************************/

JOutPipeStream::~JOutPipeStream()
{
	close();
}

/******************************************************************************
 close

 *****************************************************************************/

void
JOutPipeStream::close()
{
	if (!bad())
		{
		flush();
		itsBuffer.close();
		JSetState(*this, badbit);
		}
}

//template class JOutPipeStreambuf<char, std::char_traits<char> >;
