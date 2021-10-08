/******************************************************************************
 jTypes.h

	Defines portable types

	Copyright (C) 1994-2016 by John Lindal.

 ******************************************************************************/

#ifndef _H_jTypes
#define _H_jTypes

#include <limits.h>
#include <float.h>
#include <iostream> // templates

	// general types

using JInteger = long;

const JInteger kJIntegerMin					= LONG_MIN;
const JInteger kJIntegerMax					= LONG_MAX;

using JUInt = unsigned long;

const JUInt kJUIntMin						= 0;
const JUInt kJUIntMax						= ULONG_MAX;

using JFloat = double;

const JFloat kJFloatMin						= DBL_MIN;
const JFloat kJFloatMax						= DBL_MAX;

const long kJFloatExponentMin				= DBL_MIN_10_EXP;
const long kJFloatExponentMax				= DBL_MAX_10_EXP;

	// specialized types

using JSize = unsigned long;

const JSize kJSizeMin						= 0;
const JSize kJSizeMax						= ULONG_MAX;

using JIndex = unsigned long;

const JIndex kJIndexMin						= 0;
const JIndex kJIndexMax						= ULONG_MAX;

using JSignedIndex = long;

const JSignedIndex kJSignedIndexMin			= LONG_MIN;
const JSignedIndex kJSignedIndexMax			= LONG_MAX;

using JUnsignedOffset = unsigned long;

const JUnsignedOffset kJUnsignedOffsetMin	= 0;
const JUnsignedOffset kJUnsignedOffsetMax	= ULONG_MAX;

using JSignedOffset = long;

const JSignedOffset kJSignedOffsetMin		= LONG_MIN;
const JSignedOffset kJSignedOffsetMax		= LONG_MAX;

using JCoordinate = long;

const JCoordinate kJCoordinateMin			= LONG_MIN;
const JCoordinate kJCoordinateMax			= LONG_MAX;

using JFileVersion = unsigned long;

const JFileVersion kJFileVersionMin			= 0;
const JFileVersion kJFileVersionMax			= ULONG_MAX;

	// utf-8 characters

using JUtf8Byte = char;

const JUtf8Byte kJUtf8ByteMin				= CHAR_MIN;
const JUtf8Byte kJUtf8ByteMax				= CHAR_MAX;

	// iterators

using JCursorPosition = unsigned long;

enum JIteratorPosition
{
	kJIteratorStartAtBeginning,	// absolute
	kJIteratorStartAtEnd,		// absolute
	kJIteratorStartBefore,		// relative to given index
	kJIteratorStartAfter,		// relative to given index

	// strings only - use with caution - must be character aligned!

	kJIteratorStartBeforeByte,	// relative to given byte offset
	kJIteratorStartAfterByte	// relative to given byte offset
};

enum JIteratorAction
{
	kJIteratorStay = 0,
	kJIteratorMove = 1
};

	// boolean utilities

struct JBoolRefHolder
{
	bool& v;

	explicit JBoolRefHolder(bool& _v)
		:
		v(_v)
	{};
};

struct JBoolConstRefHolder
{
	bool const& v;

	explicit JBoolConstRefHolder(bool const& _v)
		:
		v(_v)
	{};
};

std::ostream& operator<<(std::ostream& output, JBoolConstRefHolder const& data);
std::istream& operator>>(std::istream& input, JBoolRefHolder const& data);

inline JBoolConstRefHolder
JBoolToString
	(
	bool const& v
	)
{
	return JBoolConstRefHolder(v);
}

inline JBoolRefHolder
JBoolFromString
	(
	bool& v
	)
{
	return JBoolRefHolder(v);
}

/******************************************************************************

 J?Int... types

	These types provide fixed and machine-specific sized signed and
	unsigned integral types for algorithms which require explicit knowledge
	of the size of their integers, or simply a guarantee that they are
	exactly one machine word.  The JInt... types are signed, while the
	JUInt... quantities are unsigned.  The suffix indicates the actual
	width of the type in bits.

	Specifically, JUInt32 provides an unsigned integer guaranteed to be 32
	bits on all supported machines, a bit like the FORTRAN (gasp!) *N style
	idiom; it is similar to an unsigned version of a FORTRAN INTEGER*4.
	Similarly, JUInt64 provides a 64-bit integer, but *only on machines
	which provide such a native type*.  Thus it does not always exist and
	must be tested for with the preprocessor.  Normally one only needs to
	do this to ensure that a variable is a machine word, which is provided
	automatically through the JUWord type and always exists.  JInt32,
	JInt64, and JWord are their signed bretheren.

	Considerations:

	All unix machines seem to define an int as 32 bits, regardless of
	wordsize, while a long is the machine's native wordsize, 32 bits on
	32-bit architectures and 64 on 64-bit processors like the Alpha.  This
	is consistent with the traditional (i.e. before 64-bit processor) C
	programmer's assumption that pointers and longs can convert back and
	forth without loss of information.

	MetroWerks uses 16-bit ints (perhaps the original Mac had 16-bit words?),
	so on the Mac (at least) we'll need to use a long.

	For strict ANSI compliance it can be a good idea to use a signed integer
	when the quantity may be interpreted as either a signed or unsigned value.
	The reason for this is that if a signed integral quantity is converted to
	an unsigned quantity ANSI guarantees that it behaves exactly as you think
	it should, i.e. as though the signed quantity was stored in two's complement
	representation and the bit pattern was simply re-interpreted as an unsigned
	integer.

	By contrast, ANSI does *not* guarantee that the reverse is true.  An unsigned
	quantity converted to a signed quantity overflows when its value is too large
	to fit into the signed quantity, *and the result is undefined*.  The fact
	that every C programmer on the planet behaves as though ANSI mandates the
	usual unix/two's complement behavior of re-interpreting the bit pattern as a
	two's complement value apparently did not sway the standards committee, and
	code which depends on such behavior is not strictly portable.  On the other
	hand, I wonder how many C compliers would dare to break this behavior.

 ******************************************************************************/

#if SIZEOF_LONG == 4

	using JUInt32 = unsigned long;
	using JInt32  = signed long;

	const JUInt32 kJUInt32Min = 0;
	const JUInt32 kJUInt32Max = ULONG_MAX;

	const JInt32 kJInt32Min = LONG_MIN;
	const JInt32 kJInt32Max = LONG_MAX;

#elif SIZEOF_INT == 4

	using JUInt32 = unsigned int;
	using JInt32  = signed int;

	const JUInt32 kJUInt32Min = 0;
	const JUInt32 kJUInt32Max = UINT_MAX;

	const JInt32 kJInt32Min = INT_MIN;
	const JInt32 kJInt32Max = INT_MAX;

#else
	*** Trouble; we must always have 32-bit types.  Generalize the test to
	*** your architecture before compiling, and contact the JX team so the
	*** next version does it correctly!
#endif

#if defined SIZEOF_LONGLONG && SIZEOF_LONGLONG == 8

	// Need a way to detect that the type exists
	#define JUInt64_EXISTS
	#define  JInt64_EXISTS

	using JUInt64 = unsigned long long;
	using JInt64  = signed long long;

	const JUInt64 kJUInt64Min = 0;
	const JUInt64 kJUInt64Max = ULLONG_MAX;

	const JInt64 kJInt64Min = LLONG_MIN;
	const JInt64 kJInt64Max = LLONG_MAX;

#elif SIZEOF_LONG == 8

	// Need a way to detect that the type exists
	#define JUInt64_EXISTS
	#define  JInt64_EXISTS

	using JUInt64 = unsigned long;
	using JInt64  = signed long;

	const JUInt64 kJUInt64Min = 0;
	const JUInt64 kJUInt64Max = ULONG_MAX;

	const JInt64 kJInt64Min = LONG_MIN;
	const JInt64 kJInt64Max = LONG_MAX;

#elif SIZEOF_INT == 8

	// I know, but gotta look ahead to 128-bit processors

	// Need a way to detect that the type exists
	#define JUInt64_EXISTS
	#define  JInt64_EXISTS

	using JUInt64 = unsigned int;
	using JInt64  = signed int;

	const JUInt64 kJUInt64Min = 0;
	const JUInt64 kJUInt64Max = UINT_MAX;

	const JInt64 kJInt64Min = INT_MIN;
	const JInt64 kJInt64Max = INT_MAX;

#endif

	// So far, all machines have word-sized longs
	#define JWORDSIZE SIZEOF_LONG
	using JUWord = unsigned long;
	using JWord  = signed long;

/******************************************************************************

	Defines standard hash value types, e.g. for use in JHashTable.

 ******************************************************************************/

// JHashValue must be unsigned to guarantee that >> shifts in zero bits

using JHashValue = JUWord;

// JDualHashValue must be signed so the step can go either way

using JDualHashValue = JWord;

/******************************************************************************

	Hides compiler dependent std::iostream information.

 ******************************************************************************/

#if defined __GNUG__

	using JFStreamOpenMode = std::ios_base::openmode;
	using JIOStreamSeekDir = std::ios_base::seekdir;

	const JFStreamOpenMode kJBinaryModifier = std::ios_base::binary;

#else

	figure out what to do!

#endif

const JFStreamOpenMode kJBinaryFile = std::ios::in | std::ios::out | kJBinaryModifier;
const JFStreamOpenMode kJTextFile   = std::ios::in | std::ios::out;

#endif
