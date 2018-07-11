/******************************************************************************
 JUtf8ByteBuffer.h

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUtf8ByteBuffer
#define _H_JUtf8ByteBuffer

#include "JArray.h"

class JUtf8ByteBuffer : public JArray<JUtf8Byte>
{
public:

	JUtf8ByteBuffer(const JSize blockSize);

	virtual ~JUtf8ByteBuffer();

	void	Append(const JUtf8Byte* data, const JSize count);
	JString	ExtractCharacters();
};

#endif
