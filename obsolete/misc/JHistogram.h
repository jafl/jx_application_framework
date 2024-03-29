/******************************************************************************
 JHistogram.h

	Interface for JHistogram class.

	Copyright (C) 1994 by John Lindal.

 ******************************************************************************/

#ifndef _H_JHistogram
#define _H_JHistogram

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

	void	StreamIn(std::istream& input);
	void	StreamOut(std::ostream& output) const;

protected:

	JArray<T>*			GetBins();
	const JArray<T>*	GetBins() const;

private:

	JArray<T>*	itsBins;	// bins for counts
};

#include <JHistogram.tmpl>

#endif
