/******************************************************************************
 TanhLinNetwork.h

	Interface for the TanhLinNetwork class.

	Copyright © 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_TanhLinNetwork
#define _H_TanhLinNetwork

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <FFNetwork.h>

class TanhLinNetwork : public FFNetwork
{
public:

	TanhLinNetwork(const FFNetworkSize& unitCount);
	TanhLinNetwork(istream& input);

	virtual ~TanhLinNetwork();

protected:

	virtual JFloat	g(const JIndex layerIndex, const JFloat h) const;
	virtual JVector	g(const JIndex layerIndex, const JVector& h) const;

	virtual JFloat	gprime(const JIndex layerIndex, const JFloat g) const;
	virtual JVector	gprime(const JIndex layerIndex, const JVector& g) const;

private:

	// not allowed

	TanhLinNetwork(const TanhLinNetwork& source);
	const TanhLinNetwork& operator=(const TanhLinNetwork& source);
};

#endif
