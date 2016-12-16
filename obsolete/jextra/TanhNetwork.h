/******************************************************************************
 TanhNetwork.h

	Interface for the TanhNetwork class.

	Copyright (C) 1996 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_TanhNetwork
#define _H_TanhNetwork

#include <FFNetwork.h>

class TanhNetwork : public FFNetwork
{
public:

	TanhNetwork(const FFNetworkSize& unitCount);
	TanhNetwork(std::istream& input);

	virtual ~TanhNetwork();

protected:

	virtual JFloat	g(const JIndex layerIndex, const JFloat h) const;
	virtual JVector	g(const JIndex layerIndex, const JVector& h) const;

	virtual JFloat	gprime(const JIndex layerIndex, const JFloat g) const;
	virtual JVector	gprime(const JIndex layerIndex, const JVector& g) const;

private:

	// not allowed

	TanhNetwork(const TanhNetwork& source);
	const TanhNetwork& operator=(const TanhNetwork& source);
};

#endif
