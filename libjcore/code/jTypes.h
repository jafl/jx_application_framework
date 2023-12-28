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
#include <compare>

	// general types

using JInteger = long;

constexpr JInteger kJIntegerMin					= LONG_MIN;
constexpr JInteger kJIntegerMax					= LONG_MAX;

using JUInt = unsigned long;

constexpr JUInt kJUIntMin						= 0;
constexpr JUInt kJUIntMax						= ULONG_MAX;

using JFloat = double;

constexpr JFloat kJFloatMin						= DBL_MIN;
constexpr JFloat kJFloatMax						= DBL_MAX;

constexpr long kJFloatExponentMin				= DBL_MIN_10_EXP;
constexpr long kJFloatExponentMax				= DBL_MAX_10_EXP;

	// specialized types

using JSize = unsigned long;

constexpr JSize kJSizeMin						= 0;
constexpr JSize kJSizeMax						= ULONG_MAX;

using JIndex = unsigned long;

constexpr JIndex kJIndexMin						= 0;
constexpr JIndex kJIndexMax						= ULONG_MAX;

using JSignedIndex = long;

constexpr JSignedIndex kJSignedIndexMin			= LONG_MIN;
constexpr JSignedIndex kJSignedIndexMax			= LONG_MAX;

using JUnsignedOffset = unsigned long;

constexpr JUnsignedOffset kJUnsignedOffsetMin	= 0;
constexpr JUnsignedOffset kJUnsignedOffsetMax	= ULONG_MAX;

using JSignedOffset = long;

constexpr JSignedOffset kJSignedOffsetMin		= LONG_MIN;
constexpr JSignedOffset kJSignedOffsetMax		= LONG_MAX;

using JCoordinate = long;

constexpr JCoordinate kJCoordinateMin			= LONG_MIN;
constexpr JCoordinate kJCoordinateMax			= LONG_MAX;

using JFileVersion = unsigned long;

constexpr JFileVersion kJFileVersionMin			= 0;
constexpr JFileVersion kJFileVersionMax			= ULONG_MAX;

	// utf-8 characters

using JUtf8Byte = char;

constexpr JUtf8Byte kJUtf8ByteMin				= CHAR_MIN;
constexpr JUtf8Byte kJUtf8ByteMax				= CHAR_MAX;

	// iterators

using JCursorPosition = unsigned long;

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

	// comparison

inline std::weak_ordering
JIntToWeakOrdering
	(
	const int r
	)
{
	return (r < 0 ? std::weak_ordering::less :
			r > 0 ? std::weak_ordering::greater : std::weak_ordering::equivalent);
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

static_assert(sizeof(unsigned long) == 4 || sizeof(unsigned int) == 4,
			  "unable to find a 4 byte unsigned integer type");

using JUInt32 = std::conditional_t<sizeof(unsigned long) == 4, unsigned long, unsigned int>;
using JInt32  = std::conditional_t<sizeof(  signed long) == 4,   signed long,   signed int>;

constexpr JUInt32 kJUInt32Min = 0;
constexpr JUInt32 kJUInt32Max = sizeof(unsigned long) == 4 ? JUInt32(ULONG_MAX) : UINT_MAX;

constexpr JInt32 kJInt32Min = sizeof(signed long) == 4 ? JInt32(LONG_MIN) : INT_MIN;
constexpr JInt32 kJInt32Max = sizeof(signed long) == 4 ? JInt32(LONG_MAX) : INT_MAX;

static_assert(sizeof(unsigned long long) == 8 || sizeof(unsigned long) == 8,
			  "unable to find a 8 byte unsigned integer type");

using JUInt64 = std::conditional_t<sizeof(unsigned long long) == 8, unsigned long long, unsigned long>;
using JInt64  = std::conditional_t<sizeof(  signed long long) == 8,   signed long long,   signed long>;

constexpr JUInt64 kJUInt64Min = 0;
constexpr JUInt64 kJUInt64Max = sizeof(unsigned long long) == 8 ? JUInt64(ULLONG_MAX) : ULONG_MAX;

constexpr JInt64 kJInt64Min = sizeof(signed long long) == 8 ? JInt64(LLONG_MIN) : LONG_MIN;
constexpr JInt64 kJInt64Max = sizeof(signed long long) == 8 ? JInt64(LLONG_MAX) : LONG_MAX;

// So far, all machines have word-sized longs
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

	constexpr JFStreamOpenMode kJBinaryModifier = std::ios_base::binary;

#else

	figure out what to do!

#endif

constexpr JFStreamOpenMode kJBinaryFile = std::ios::in | std::ios::out | kJBinaryModifier;
constexpr JFStreamOpenMode kJTextFile   = std::ios::in | std::ios::out;

#endif
