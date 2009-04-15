/******************************************************************************
 JDynamicHistogram.h

	Interface for JDynamicHistogram class.

	Copyright © 1995 by John Lindal. All rights reserved.

 ******************************************************************************/

#ifndef _H_JDynamicHistogram
#define _H_JDynamicHistogram

#if !defined _J_UNIX && !defined ACE_LACKS_PRAGMA_ONCE
#pragma once
#endif

#include <JHistogram.h>

template <class T>
class JDynamicHistogram : public JHistogram<T>
{
public:

	JDynamicHistogram(const JSize binCount = 0);
	JDynamicHistogram(const JDynamicHistogram<T>& source);

	virtual ~JDynamicHistogram();

	const JDynamicHistogram<T>& operator=(const JDynamicHistogram<T>& source);

	void	InsertBin(const JBinIndex index, const T count = 0);
	void	PrependBin(const T count = 0);
	void	AppendBin(const T count = 0);
	void	RemoveBin(const JBinIndex index);
	void	RemoveAllBins();
	void	MoveBinToIndex(const JBinIndex currentIndex, const JBinIndex newIndex);

	void	CreateSetCount(const JBinIndex index, const T count);
	void	CreateIncrementCount(const JBinIndex index, const T delta);

	JSize	GetBinBlockSize() const;
	void	SetBinBlockSize(const JSize blockSize);
};

#endif
