/******************************************************************************
 jRand.cpp

	The jRand library is a collection of basic random number generators.  It
	is mainly intended for use by the various random number generator classes,
	but can also be useful elsewhere (such as in the JHashTable suite).

	Copyright (C) 1997 by Dustin Laurence.
	
	Base code generated by Codemill v0.1.0

 *****************************************************************************/

#include "jx-af/jcore/jRand.h"
#include "jx-af/jcore/jAssert.h"

/******************************************************************************
 JKLRandInt32 -- Knuth & Lewis 32-bit linear congruential pseudorandom number generator

	JKLRandint provides a simple, extremely fast "random" number generator
	for JCore, probably the fastest possible in C while retaining usable
	statistical properties (one inline multiplication and one inline
	addition per number generated).  Conceivably one could not do much
	better in assembly, at least on hardware with a native 32-bit wordsize
	and some commom arithmetic properties.

	Unlike rand()-style generators, the seed is stored by the caller, not the
	system.  This guarantees no change in behavior in reentrant or multi-
	threaded environments. Normally this function is only called through the
	JKLRand class methods, which provides a natural way to store and update the
	seed.

	The value is a signed quantity because of the conversion portability
	considerations discussed in jSizedTypes.h.

	Limitations:

	Note that JKLRandInt suffers from all the usual defects of simple
	linear congruential generators, such as correlated low-order bits.
	This means that if you want to use only some of the bits, such as to
	generate a random number between 0 and N-1, the obvious technique of
	using only the low-order bits via (seed % N) results in a broken
	generator.  Put all those C bitwise operators to good use and use the
	high-order bits instead.

	In addition, JKLRandInt suffers from one additional defect; if you are
	using random numbers in batches of q numbers at a time, and q is a
	multiple of more than a couple of powers of two, then the generator
	doesn't behave as well because gcd(q, period == 2^32) is "large."  See
	Knuth, Vol. 2, page 73 for details.  Because of this I will have to
	give JX a generator with a large prime modulus, similar to the
	Numerical Recipes generators.

	The price of using a fast generator like this is that you are responsible
	for knowing how to use it properly.  On the other hand, the rand() supplied
	by your compiler is probably worse, so ignorance is no reason to use rand()
	rather than JKLRandInt() (actually, a system-supplied rand() should never be
	used unless you have personally verified the implementation or don't care
	much about the quality of the generator).  In fact, you should reconsider
	blindly trusting me as well. :-)

	Note that I am more positive about the practical quality of JKLRandInt
	than the authors of Numerical Recipes are, mainly because I think I can
	avoid using them in batches of, say, eight at a time if I need the
	speed.  Since the problem of a non-prime modulus is the only defect I
	know of for this generator, I assume this was their main reason in not
	recommending it.  However, their own tests show that this generator is
	so fast it would pay to generate an extra one and discard it rather
	than use a non-wordsize modulus, so forgetting this little 'gotcha' is
	the main danger.  If you seriously care about these issues, you should
	read either their recommendations or those of some other good source.
	Even better, you might subject it to some real testing and tell me
	about the results.  :-)

	It's an easy theorem to prove that any sequence in which the (n+1)th
	value is a fixed function on a finite set of the nth value must be
	periodic.  JKLRandInt has a period of 2^32 ~ 4.29*10^9.  However,
	statistical problems due to period exhaustion set in long before all
	the possible values have been generated; Numerical Recipes suggests
	(period)/20 ~ 2*20^8 ~ (a few hundred million) is the most that should
	be used (technically, the context was of a different, specific
	generator), while Knuth offers a rather more conservative figure of
	(period)/10^3 ~ 4*10^6 ~ (a few million).  Take your pick.  If you need
	more than that on a 32 bit machine, you need either a generator which
	depends on more than one previous value or you need to combine distinct
	generators; as always Knuth will tell you more than you want to know
	about this and Numerical Recipes will summarize most of the bottom-line
	info.

	Implementation:

	This function is a fast linear congruential random number generator for
	32-bit integers; the modulus is implicitly 2^32 because C guarantees
	that integer arithmetic is implicitly modulus the size of the data
	type.  The generator constants are those of Knuth & Lewis, so this
	generator is equivalent to the Numerical Recipes 'quick and dirty'
	randqd1() generator implemented as a C++ inline function.  (However, in
	the JCore context this is NOT regarded as being more 'dirty' than any
	other linear congruential method (except for the
	batches-of-powers-of-two problem mentioned above) because C++
	guarantees its behavior on all conforming implementations and unlike
	the authors of Numerical Recipes we only care about architecture
	independence, not language independence.  It's still quick, though. :-) )

 *****************************************************************************/

/******************************************************************************
 JKHRandInt64 -- Knuth & Haynes 64-bit linear congruential pseudorandom number generator

	The same considerations as for JKLRandInt32 apply, except that this
	function generates 64-bit random integers using Hayne's multiplier
	(blessed by Knuth) and an additive constant of 1 as Knuth recommends.

	Of course this function only exists if we have 64-bit variables.

 *****************************************************************************/

/******************************************************************************
 JRandWord -- word sized linear congruential pseudorandom number generator

	Uses either JKLRandInt32 or JKHRandInt64 to produce a random machine
	word (so far all supported architectures have either 32 or 64 bit
	words).  Note that generators with wordsize moduli are required in this
	case since this function is intended for applications where the range
	of possible values must cover the entire range of a word; the
	'portable' style prime-number modulus generators would not work.

 *****************************************************************************/
