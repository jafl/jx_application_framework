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

	cout << "subset s1 created" << endl << endl;

	cout << "s1 itemCount should be 0" << endl;
	cout << "s1 itemCount = " << s1.GetElementCount() << endl << endl;

	s1.AddRange(3,4);
	s1.Add(9);

	cout << "s1 itemCount should be 3" << endl;
	cout << "s1 itemCount = " << s1.GetElementCount() << endl << endl;

	cout << "s1 should contain:         10FFTTFFFFTF" << endl;
	cout << "s1 contains      : " << s1 << endl;
	cout << "using Contains() :           ";

	for (i=1; i<=10; i++)
		{
		cout << s1.Contains(i);
		}
	cout << endl;

JSubset s2 = s1;

	cout << endl;
	cout << "subset s2 created" << endl << endl;

	s2.Remove(1);

	cout << "s1 should equal s2" << endl;
	cout << "s1 equals s2? " << (s1 == s2) << endl;

	s2.Remove(4);
	s2.Add(2);

	cout << "s2 should contain:         10FTTFFFFFTF" << endl;
	cout << "s2 contains      : " << s2 << endl;

	cout << "s1 should not equal s2" << endl;
	cout << "s1 equals s2? " << (s1 == s2) << endl;

	JWaitForReturn();

JSubset s3 = s1 + s2;
JSubset s4 = s1 - s2;
JSubset s5 = s2 - s1;
JSubset s7 = JIntersection(s4, s5);

	cout << "s3 should contain:         10FTTTFFFFTF" << endl;
	cout << "s3 contains      : " << s3 << endl << endl;

	cout << "s4 should contain:         10FFFTFFFFFF" << endl;
	cout << "s4 contains      : " << s4 << endl << endl;

	cout << "s5 should contain:         10FTFFFFFFFF" << endl;
	cout << "s5 contains      : " << s5 << endl << endl;

	cout << "s7 should contain:         10FFFTFFFFFF" << endl;
	cout << "s7 contains      : " << s4 << endl << endl;

	JWaitForReturn();

	s3.RemoveAll();
	s3.AddRange(3,8);
	s3.RemoveRange(4,6);

	cout << "s3 should contain:         10FFTFFFTTFF" << endl;
	cout << "s3 contains      : " << s3 << endl << endl;

JSubsetIterator iter = s3.NewIterator();

	JIndex index;

	cout << "s3 contains: ";
	while (iter.Next(&index))
		{
		cout << ' ' << index;
		}
	cout << endl << endl;

JSubset s6 = s3.Complement();

	cout << "s6 should contain:         10TTFTTTFFTT" << endl;
	cout << "s6 contains      : " << s6 << endl << endl;

	s6 = s3;											// assignment operator

	cout << "s6 assigned from s3" << endl << endl;

	cout << "s6 itemCount should be 3" << endl;
	cout << "s6 itemCount=" << s6.GetElementCount() << endl << endl;

	cout << "s6 should contain:         10FFTFFFTTFF" << endl;
	cout << "s6 contains      : " << s6 << endl << endl;

	JWaitForReturn();

	cout << "Twenty random samples of size 3:" << endl << endl;

	for (i=1; i<=20; i++)
		{
		if (JGetRandomSample(&s1, 3))
			{
			assert( s1.GetElementCount() == 3 );
			cout << s1 << endl;
			}
		else
			{
			cerr << "Unable to generate random subset of that size." << endl;
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

	cout << endl;
	cout << "Probability of each element (2-8) being in the subset:" << endl;
	cout << "(averaged over ensemble of 5000 subsets)" << endl << endl;
	cout << p << endl;

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

	cout << endl;
	cout << "Probability of each element (sample of 7) being in the subset:" << endl;
	cout << "(averaged over ensemble of 5000 subsets)" << endl << endl;
	cout << p << endl;

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

	cout << endl;
	cout << "Probability of each element (2 from sample of 7) being in the subset:" << endl;
	cout << "(averaged over ensemble of 5000 subsets)" << endl << endl;
	cout << "1) " << h1.ConvertToProbabilities() << endl;
	cout << "2) " << h2.ConvertToProbabilities() << endl;

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

	cout << endl;
	cout << "Probability of each element (3,4 from sample of 7) being in the subset:" << endl;
	cout << "(averaged over ensemble of 5000 subsets)" << endl << endl;
	cout << "1) " << h1.ConvertToProbabilities() << endl;
	cout << "2) " << h2.ConvertToProbabilities() << endl;

	return 0;
}
