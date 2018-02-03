/******************************************************************************
 TanhLinNetwork.h

	Interface for the TanhLinNetwork class.

	Copyright (C) 1996 by John Lindal.

 ******************************************************************************/

#ifndef _H_TanhLinNetwork
#define _H_TanhLinNetwork

#include <FFNetwork.h>

class TanhLinNetwork : public FFNetwork
{
public:

	TanhLinNetwork(const FFNetworkSize& unitCount);
	TanhLinNetwork(std::istream& input);

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
