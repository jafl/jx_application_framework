/******************************************************************************
 JHistogram.h

	Interface for JHistogram class.

	Copyright © 1994 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JHistogram
#define _H_JHistogram

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JArray.h>		// template

class JProbDistr;

template <class T>
class JHistogram
{
public:

	JHistogram(const JSize binCount);
	JHistogram(const JHistogram<T>& source);

	virtual ~JHistogram();

	const JHistogram<T>& operator=(const JHistogram<T>& source);

	JSize	GetBinCount() const;

	T		GetCount(const JBinIndex index) const;
	void	SetCount(const JBinIndex index, const T count);
	void	IncrementCount(const JBinIndex index, const T delta);

	T		GetTotalCount() const;

	JProbDistr	ConvertToProbabilities() const;

	void	Clear();

	void	StreamIn(istream& input);
	void	StreamOut(ostream& output) const;

protected:

	JArray<T>*			GetBins();
	const JArray<T>*	GetBins() const;

private:

	JArray<T>*	itsBins;	// bins for counts
};

#endif
