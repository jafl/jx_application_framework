/******************************************************************************
 JUtf8ByteBuffer.h

	Copyright (C) 2018 by John Lindal.

 ******************************************************************************/

#ifndef _H_JUtf8ByteBuffer
#define _H_JUtf8ByteBuffer

#include "JArray.h"
#include <span>

class JUtf8ByteBuffer : public JArray<JUtf8Byte>
{
public:

	JUtf8ByteBuffer(const JSize minSize = 1024);

	~JUtf8ByteBuffer() override;

	void	Append(const std::span<const JUtf8Byte>& data);
	JString	ExtractCharacters();
};

#endif
