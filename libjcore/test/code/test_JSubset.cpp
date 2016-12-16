/******************************************************************************
 test_JSubset.cc

	Program to test JSubset class.

	Written by John Lindal.

 ******************************************************************************/

#include <JSubset.h>
#include <JHistogram.h>
#include <JProbDistr.h>
#include <JPtrArray.h>
#include <jCommandLine.h>
#include <jAssert.h>

int main()
{
long i;

JSubset s1(10);													// constructor

	std::cout << "subset s1 created" << std::endl << std::endl;

	std::cout << "s1 itemCount should be 0" << std::endl;
	std::cout << "s1 itemCount = " << s1.GetElementCount() << std::endl << std::endl;

	s1.AddRange(3,4);
	s1.Add(9);

	std::cout << "s1 itemCount should be 3" << std::endl;
	std::cout << "s1 itemCount = " << s1.GetElementCount() << std::endl << std::endl;

	std::cout << "s1 should contain:         10FFTTFFFFTF" << std::endl;
	std::cout << "s1 contains      : " << s1 << std::endl;
	std::cout << "using Contains() :           ";

	for (i=1; i<=10; i++)
		{
		std::cout << s1.Contains(i);
		}
	std::cout << std::endl;

JSubset s2 = s1;

	std::cout << std::endl;
	std::cout << "subset s2 created" << std::endl << std::endl;

	s2.Remove(1);

	std::cout << "s1 should equal s2" << std::endl;
	std::cout << "s1 equals s2? " << (s1 == s2) << std::endl;

	s2.Remove(4);
	s2.Add(2);

	std::cout << "s2 should contain:         10FTTFFFFFTF" << std::endl;
	std::cout << "s2 contains      : " << s2 << std::endl;

	std::cout << "s1 should not equal s2" << std::endl;
	std::cout << "s1 equals s2? " << (s1 == s2) << std::endl;

	JWaitForReturn();

JSubset s3 = s1 + s2;
JSubset s4 = s1 - s2;
JSubset s5 = s2 - s1;
JSubset s7 = JIntersection(s4, s5);

	std::cout << "s3 should contain:         10FTTTFFFFTF" << std::endl;
	std::cout << "s3 contains      : " << s3 << std::endl << std::endl;

	std::cout << "s4 should contain:         10FFFTFFFFFF" << std::endl;
	std::cout << "s4 contains      : " << s4 << std::endl << std::endl;

	std::cout << "s5 should contain:         10FTFFFFFFFF" << std::endl;
	std::cout << "s5 contains      : " << s5 << std::endl << std::endl;

	std::cout << "s7 should contain:         10FFFTFFFFFF" << std::endl;
	std::cout << "s7 contains      : " << s4 << std::endl << std::endl;

	JWaitForReturn();

	s3.RemoveAll();
	s3.AddRange(3,8);
	s3.RemoveRange(4,6);

	std::cout << "s3 should contain:         10FFTFFFTTFF" << std::endl;
	std::cout << "s3 contains      : " << s3 << std::endl << std::endl;

JSubsetIterator iter = s3.NewIterator();

	JIndex index;

	std::cout << "s3 contains: ";
	while (iter.Next(&index))
		{
		std::cout << ' ' << index;
		}
	std::cout << std::endl << std::endl;

JSubset s6 = s3.Complement();

	std::cout << "s6 should contain:         10TTFTTTFFTT" << std::endl;
	std::cout << "s6 contains      : " << s6 << std::endl << std::endl;

	s6 = s3;											// assignment operator

	std::cout << "s6 assigned from s3" << std::endl << std::endl;

	std::cout << "s6 itemCount should be 3" << std::endl;
	std::cout << "s6 itemCount=" << s6.GetElementCount() << std::endl << std::endl;

	std::cout << "s6 should contain:         10FFTFFFTTFF" << std::endl;
	std::cout << "s6 contains      : " << s6 << std::endl << std::endl;

	JWaitForReturn();

	std::cout << "Twenty random samples of size 3:" << std::endl << std::endl;

	for (i=1; i<=20; i++)
		{
		if (JGetRandomSample(&s1, 3))
			{
			assert( s1.GetElementCount() == 3 );
			std::cout << s1 << std::endl;
			}
		else
			{
			std::cerr << "Unable to generate random subset of that size." << std::endl;
			}
		}

// JGetRandomSample()

	JHistogram<JSize> h(10);
	for (i=1; i<=5000; i++)
		{
		JGetRandomSample(&s3, 3, 2,8);

		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		while (iter.Next(&index))
			{
			h.IncrementCount(index, 1);
			}
		}

	JProbDistr p = h.ConvertToProbabilities();

	std::cout << std::endl;
	std::cout << "Probability of each element (2-8) being in the subset:" << std::endl;
	std::cout << "(averaged over ensemble of 5000 subsets)" << std::endl << std::endl;
	std::cout << p << std::endl;

// JSubset::GetRandomSample()

	JBoolean ok = JGetRandomSample(&s1, 7);
	assert( ok );

	h.Clear();
	for (i=1; i<=5000; i++)
		{
		s1.GetRandomSample(&s3, 3);

		iter.MoveTo(kJIteratorStartAtBeginning, 0);
		while (iter.Next(&index))
			{
			h.IncrementCount(index, 1);
			}
		}

	p = h.ConvertToProbabilities();

	std::cout << std::endl;
	std::cout << "Probability of each element (sample of 7) being in the subset:" << std::endl;
	std::cout << "(averaged over ensemble of 5000 subsets)" << std::endl << std::endl;
	std::cout << p << std::endl;

// JSubset::GetRandomDisjointSamples()

	JPtrArray<JSubset> sampleList(JPtrArrayT::kDeleteAll);

	JArray<JSize> sampleSizeList;
	sampleSizeList.AppendElement(2);
	sampleSizeList.AppendElement(2);

	JHistogram<JSize> h1(10), h2(10);
	JHistogram<JSize>* hist[] = { &h1, &h2 };
	for (i=1; i<=5000; i++)
		{
		s1.GetRandomDisjointSamples(&sampleList, sampleSizeList);

		for (JIndex j=1; j<=2; j++)
			{
			JSubsetIterator iter = (sampleList.NthElement(j))->NewIterator();
			while (iter.Next(&index))
				{
				hist[j-1]->IncrementCount(index, 1);
				assert( !(sampleList.NthElement(3-j))->Contains(index) );
				}
			}
		}

	std::cout << std::endl;
	std::cout << "Probability of each element (2 from sample of 7) being in the subset:" << std::endl;
	std::cout << "(averaged over ensemble of 5000 subsets)" << std::endl << std::endl;
	std::cout << "1) " << h1.ConvertToProbabilities() << std::endl;
	std::cout << "2) " << h2.ConvertToProbabilities() << std::endl;

// JSubset::GetRandomDisjointSamples() -- partitioning

	sampleSizeList.SetElement(1, 3);
	sampleSizeList.SetElement(2, 4);

	h1.Clear();
	h2.Clear();
	for (i=1; i<=5000; i++)
		{
		s1.GetRandomDisjointSamples(&sampleList, sampleSizeList);

		for (JIndex j=1; j<=2; j++)
			{
			JSubsetIterator iter = (sampleList.NthElement(j))->NewIterator();
			while (iter.Next(&index))
				{
				hist[j-1]->IncrementCount(index, 1);
				assert( !(sampleList.NthElement(3-j))->Contains(index) );
				}
			}
		}
	sampleList.DeleteAll();

	std::cout << std::endl;
	std::cout << "Probability of each element (3,4 from sample of 7) being in the subset:" << std::endl;
	std::cout << "(averaged over ensemble of 5000 subsets)" << std::endl << std::endl;
	std::cout << "1) " << h1.ConvertToProbabilities() << std::endl;
	std::cout << "2) " << h2.ConvertToProbabilities() << std::endl;

	return 0;
}
