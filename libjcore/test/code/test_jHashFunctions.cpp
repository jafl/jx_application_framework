/******************************************************************************
 test_jHashFunctions.cpp

	Test hash functions.

	Copyright (C) 1997 by Dustin Laurence.

 *****************************************************************************/

#include "JTestManager.h"
#include "jHashFunctions.h"
#include "jAssert.h"

const JHashValue dualHashArg[] =
{
	12345678,
	0x00,
	0xFFFFFFFF,
	0xabcdef00, // Test how much close hash values are broken up
	0xabcdef01,
	0xabcdef02,
	0xabcdef03,
	0xabcdef04,
	0xabcdef05,
	0xabcdef06,
	0xabcdef07,
	0xabcdef08,
	0xabcdef09,
	0xabcdef0a,
	0xabcdef0b,
	0xabcdef0c,
	0xabcdef0d,
	0xabcdef0e,
	0xabcdef0f
};

const JUtf8Byte* hashStringArg[] =
{
	"l",
	"m",
	"n",

	"AAAAAAAA",
	"AAAAAAAB",
	"AAAAAAA",
	"AAAAAAAAA",

	"ahashkey",
	"bhashkey",
	"chashkey",

	"Cachalot",
	"Cachalos",
	"Cachalor",

	"Fourscore and seven years ago, our forefathers brought forth on this "
	"continent a new nation, dedicated to the proposition that all men are "
	"created equal.  We are now engaged in a great struggle to determine "
	"whether government of the people, by the people, and for the people shall "
	"long endure.",

	"\xA3\x32\x01\x87\x16",
	"\xA3\x32\x02\x87\x16",

	nullptr
};

// 64 bit

const JDualHashValue dualHash64Result[] =
{
	-25,
	-127,
	-63,
	-127,
	65,
	1,
	-63,
	127,
	63,
	-1,
	-65,
	127,
	61,
	-3,
	-67,
	125,
	61,
	-5,
	-69,
	0x00 // Sentinel, zero is never returned
};

const JHashValue hash7Bit64Result[] =
{
	(JHashValue) 0x4293031c4f11a6fd,
	(JHashValue) 0x9ae4f7499ba7262a,
	(JHashValue) 0xf336eb76e83ca557,

	(JHashValue) 0x264c1c6d370dda01,
	(JHashValue) 0x7e9e109a83a3592e,
	(JHashValue) 0x19a646c7685ae6ee,
	(JHashValue) 0x6c9119aec1d4706e,

	(JHashValue) 0xbfc89db78bae5df1,
	(JHashValue) 0x599407676389bc44,
	(JHashValue) 0xf35f71173b651a97,

	(JHashValue) 0x39f3784c7f4571f8,
	(JHashValue) 0xe1a1841f32aff2cb,
	(JHashValue) 0x894f8ff1e61a739e,

	(JHashValue) 0x844892c3e4cda55b,

	(JHashValue) 0x60F39C7B395BD0AE,
	(JHashValue) 0xE45CB681503D62DB
};

const JHashValue hash8Bit64Result[] =
{
	(JHashValue) 0x4293031c4f11a6fd,
	(JHashValue) 0x9ae4f7499ba7262a,
	(JHashValue) 0xf336eb76e83ca557,

	(JHashValue) 0xe51692a9fe58cbf1,
	(JHashValue) 0x3d6886d74aee4b1e,
	(JHashValue) 0x80fd39b3abe7eb1a,
	(JHashValue) 0x9f0cc2cd3053ebe,

	(JHashValue) 0xc9b15198271f8365,
	(JHashValue) 0x398272787f099e7c,
	(JHashValue) 0xa9539358d6f3b993,

	(JHashValue) 0x9fa1c3b9b3e1f408,
	(JHashValue) 0x474fcf8c674c74db,
	(JHashValue) 0xeefddb5f1ab6f5ae,

	(JHashValue) 0x6ff44b4122e64237,

	(JHashValue) 0xE963AC690F2EA0B2,
	(JHashValue) 0x4A080C6D31FD4A17
};

// 32 bit

const JDualHashValue dualHash32Result[] =
{
	-3,
	-7,
	71,
	-117,
	63,
	-15,
	-91,
	87,
	11,
	-67,
	113,
	35,
	-41,
	-119,
	61,
	-17,
	-93,
	85,
	9,
	0x00 // Sentinel, zero is never returned
};

const JHashValue hash7Bit32Result[] =
{
	0x472600db,
	0x473f66e8,
	0x4758ccf5,

	0x7a1a8d5f,
	0x7a33f36c,
	0xf651292c,
	0xdb0342ac,

	0x27b7ab4f,
	0x319f32c2,
	0x3b86ba35,

	0xa26de3f6,
	0xa2547de9,
	0xa23b17dc,

	0xe1a50679,

	0x6fd5baec,
	0xb00c53f9
};

const JHashValue hash8Bit32Result[] =
{
	0x472600db,
	0x473f66e8,
	0x4758ccf5,

	0xb2cf594f,
	0xb2e8bf5c,
	0xd6e325d8,
	0x5365cefc,

	0x6b23443,
	0x93ecf9fa,
	0x2127bfb1,

	0x13c40c06,
	0x13aaa5f9,
	0x13913fec,

	0xf1197dd5,

	0x760dc870,
	0xe0993db5
};

const JDualHashValue* dualHashResult = sizeof(long) == 8 ? dualHash64Result : dualHash32Result;
const JHashValue* hash7BitResult     = sizeof(long) == 8 ? hash7Bit64Result : hash7Bit32Result;
const JHashValue* hash8BitResult     = sizeof(long) == 8 ? hash8Bit64Result : hash8Bit32Result;

int main()
{
	return JTestManager::Execute();
}

JTEST(DualHash)
{
	JIndex i=0;
	while (dualHashResult[i] != 0)
	{
		JDualHashValue result = JDualHash(dualHashArg[i]);
		JAssertEqualWithMessage(dualHashResult[i], result, "JDualHash");
		i++;
	}
}

JTEST(Hash7Bit)
{
	JIndex i=0;
	while (hashStringArg[i] != nullptr)
	{
		JHashValue result = JHash7Bit(hashStringArg[i]);
		JAssertEqualWithMessage(hash7BitResult[i], result, "JHash7Bit");
		i++;
	}
}

JTEST(Hash8Bit)
{
	JIndex i=0;
	while (hashStringArg[i] != nullptr)
	{
		JHashValue result = JHash8Bit(hashStringArg[i]);
		JAssertEqualWithMessage(hash8BitResult[i], result, "JHash8Bit");
		i++;
	}
}
