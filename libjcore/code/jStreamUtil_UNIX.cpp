/******************************************************************************
 jStreamUtil_UNIX.cpp

	UNIX System routines to do what streams ought to be able to do
	in the first place.

	Copyright (C) 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#include <jStreamUtil.h>

/******************************************************************************
 JTellg

 ******************************************************************************/

JSize
JTellg
	(
	std::istream& stream
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
	std::istream&	stream,
	long		position
	)
{
	stream.seekg((std::streamoff) position);
}

void
JSeekg
	(
	std::istream&			stream,
	long				offset,
	JIOStreamSeekDir	direction
	)
{
	stream.seekg((std::streamoff) offset, direction);
}

#endif

void
JSeekg
	(
	std::istream&	stream,
	std::streampos	position
	)
{
	stream.seekg(position);
}

void
JSeekg
	(
	std::istream&			stream,
	std::streamoff			offset,
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
	std::ostream& stream
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
	std::ostream&	stream,
	long		position
	)
{
	stream.seekp((std::streamoff) position);
}

void
JSeekp
	(
	std::ostream&			stream,
	long				offset,
	JIOStreamSeekDir	direction
	)
{
	stream.seekp((std::streamoff) offset, direction);
}

#endif

void
JSeekp
	(
	std::ostream&	stream,
	std::streampos	position
	)
{
	stream.seekp(position);
}

void
JSeekp
	(
	std::ostream&			stream,
	std::streamoff			offset,
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
	std::ios&		stream,
	const int	flag
	)
{
	stream.setstate((std::ios_base::iostate) flag);
}
