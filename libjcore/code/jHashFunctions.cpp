/******************************************************************************
 jHashFunctions.cpp

	JHashFunctions is a collection of useful hash functions.  They are mainly
	intended for use by the JHashTable suite (which is the only reason they
	take references to pointers, because the templates instantiate to take
	that argument).  They mostly use the 32-bit JHashValue, the standard JCore
	hash value type.

	Copyright (C) 1997 by Dustin Laurence.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

//Library Header
#include <jHashFunctions.h>

#include <jAssert.h>

/******************************************************************************
 JDualHash

	Produces an 8-bit dual hash value from another hash value, suitable for use
	as a dual hashing stepsize.  The dual hash value is always odd so that it
	is relatively prime with power-of-two tablesizes and so that the stepsize
	is never zero.

	Implementation:

	A good dual hashing function needs to produce small step-sizes, because
	large ones will tend to produce many page faults in large tables; however,
	values too small will not sufficiently reduce clustering.  This
	implementation arbitrarily chooses 0x80 as the maximum step size, but
	experimentation would no doubt reveal a better choice.  It also allows
	steps to be negative, doubling the number of possible steps without
	increasing the absolute maximum stepsize.

	Such a function should also produce 'uncorrelated' values from closely
	spaced arguments.  Experimentation suggests that both the high and low
	order bits of the result of a linear congruential generator are poor
	choices.  Choosing the byte starting before the sixth or after the tenth
	bits (starting with zero on the right using conceptual, not hardware byte
	ordering) performed badly in an eyeball test (admittedly a terrible way to
	test correlation), having serial ordering.  The value of nine used here
	seemed best.  This value isn't perfect (I think I see serial anti-
	correlation) but will do for now.  I suspect that a linear congruential
	generator may be a poor choice for this application, but hash tables don't
	need high-quality random numbers!

 *****************************************************************************/

/******************************************************************************
 JHash7Bit

	A generic hash function for 7-bit character data.  The key may not be
	NULL.  This function is based on Sedgewick's program 14.1, adapted to
	full word-size keys and given a makeover.  It also adds a final call to
	a fast random number generator to eliminate (I hope) some flaws in
	Sedgewick's version (otherwise very short strings always have very
	small hash values and strings which only differ in their final
	character have closely-spaced hash values, for example).

	Note that while it isn't a 'universal' hash function in Sedgewick's
	sense, it should be more than good enough for JHashTable (which will
	never have a tablesize which is a multiple of 127).

	It is listed as intended for 7-bit data only because it uses Sedgewick's
	base-127 trick.  It won't *fail* on binary data, but may not have ideal
	statistical properties.  Minimal eyeball testing shows no problems, though.

 *****************************************************************************/

JHashValue
JHash7Bit
	(
	const JUtf8Byte* const& key
	)
{
	assert(key != NULL);

	// Any optimizer worth it's salt ought to make this variable free
	const JUtf8Byte* thisChar = key;

	JHashValue hash=0;
	while (*thisChar != '\0')
		{
		hash = (127*hash + *thisChar); // mod (wordsize) is automatic for word-sized keys
		++thisChar;
		}

	return JRandWord(hash); // Extra "randomness" as discussed above
}

/******************************************************************************
 JHash8Bit

	Like JHash7Bit, but uses base-251 arithmetic so it is more suited for
	8-bit binary data.  However, on 7-bit data it might have hidden
	weaknesses because the "digits" are always in the first half of their
	range.

 *****************************************************************************/

JHashValue
JHash8Bit
	(
	const JUtf8Byte* const& key
	)
{
	assert(key != NULL);

	// Any optimizer worth it's salt ought to make this variable free
	const JUtf8Byte* thisChar = key;

	JHashValue hash=0;
	while (*thisChar != '\0')
		{
		hash = (251*hash + *thisChar); // mod (wordsize) is automatic for word-sized keys
		++thisChar;
		}

	return JRandWord(hash); // Extra "randomness" as discussed above
}
