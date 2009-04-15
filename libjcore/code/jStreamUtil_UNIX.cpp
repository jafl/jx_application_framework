/******************************************************************************
 jStreamUtil_UNIX.cpp

	UNIX System routines to do what streams ought to be able to do
	in the first place.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <JCoreStdInc.h>
#include <jStreamUtil.h>

/******************************************************************************
 JTellg

 ******************************************************************************/

JSize
JTellg
	(
	istream& stream
	)
{
	return stream.tellg();
}

/******************************************************************************
 JSeekg

 ******************************************************************************/

#ifdef _J_CYGWIN

void
JSeekg
	(
	istream&	stream,
	long		position
	)
{
	stream.seekg((streamoff) position);
}

void
JSeekg
	(
	istream&			stream,
	long				offset,
	JIOStreamSeekDir	direction
	)
{
	stream.seekg((streamoff) offset, direction);
}

#endif

void
JSeekg
	(
	istream&	stream,
	streampos	position
	)
{
	stream.seekg(position);
}

void
JSeekg
	(
	istream&			stream,
	streamoff			offset,
	JIOStreamSeekDir	direction
	)
{
	stream.seekg(offset, direction);
}

/******************************************************************************
 JTellp

 ******************************************************************************/

JSize
JTellp
	(
	ostream& stream
	)
{
	return stream.tellp();
}

/******************************************************************************
 JSeekp

 ******************************************************************************/

#ifdef _J_CYGWIN

void
JSeekp
	(
	ostream&	stream,
	long		position
	)
{
	stream.seekp((streamoff) position);
}

void
JSeekp
	(
	ostream&			stream,
	long				offset,
	JIOStreamSeekDir	direction
	)
{
	stream.seekp((streamoff) offset, direction);
}

#endif

void
JSeekp
	(
	ostream&	stream,
	streampos	position
	)
{
	stream.seekp(position);
}

void
JSeekp
	(
	ostream&			stream,
	streamoff			offset,
	JIOStreamSeekDir	direction
	)
{
	stream.seekp(offset, direction);
}

/******************************************************************************
 JSetState

 ******************************************************************************/

void
JSetState
	(
	ios&		stream,
	const int	flag
	)
{
#ifdef __SUNPRO_CC
	stream.clear(stream.rdstate() | flag);
#elif __GNUG__ >= 3
	stream.setstate((ios_base::iostate) flag);
#else
	stream.setstate(flag);
#endif
}
