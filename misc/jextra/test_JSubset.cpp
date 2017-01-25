/******************************************************************************
 test_JSubset.cc

	Program to test JSubset class.

	Written by John Lindal.

 ******************************************************************************/

#include <JUnitTestManager.h>
#include <JSubset.h>
#include <JHistogram.h>
#include <JProbDistr.h>
#include <JPtrArray.h>
#include <jassert_simple.h>

#define verify(subset, str)	VerifyContents(subset, str, __LINE__)

int main()
{
	return JUnitTestManager::Execute();
}

void
VerifyContents
	(
	const JSubset&		subset,
	const JUtf8Byte*	expectedStr,
	const int			line
	)
{
	std::ostringstream data;
	data << subset;
	JString s;
	s = data.str();
	s.TrimWhitespace();
	JAreEqual(expectedStr, s, __FILE__, line);
}

JTEST(Exercise)
{
	long i;

	JSubset s1(10);
	JAssertTrue(s1.IsEmpty());
	JAssertEqual(0, s1.GetElementCount());

	s1.AddRange(3,4);
	s1.Add(9);
	JAssertFalse(s1.IsEmpty());
	JAssertEqual(3, s1.GetElementCount());

	verify(s1, "10FFTTFFFFTF");
	{
	JBoolean expect[] = { kJFalse, kJFalse, kJTrue, kJTrue, kJFalse, kJFalse, kJFalse, kJFalse, kJTrue, kJFalse };
	long j            = 0;
	for (i=1; i<=10; i++)
		{
		JAssertEqual(expect[j], s1.Contains(i));
		j++;
		}
	}

	JSubset s2 = s1;
	s2.Remove(1);
	JAssertEqual(s1, s2);

	s2.Remove(4);
	s2.Add(2);
	verify(s2, "10FTTFFFFFTF");
	JAssertFalse(s1 == s2);

	JSubset s3 = s1 + s2;
	verify(s3, "10FTTTFFFFTF");

	JSubset s4 = s1 - s2;
	verify(s4, "10FFFTFFFFFF");

	JSubset s5 = s2 - s1;
	verify(s5, "10FTFFFFFFFF");

	JSubset s7 = JIntersection(s3, s4);
	verify(s7, "10FFFTFFFFFF");

	s3.RemoveAll();
	s3.AddRange(3,8);
	s3.RemoveRange(4,6);
	verify(s3, "10FFTFFFTTFF");

	JSubsetIterator iter = s3.NewIterator();
	{
	JIndex expect[] = { 3, 7, 8 };
	long j          = 0;
	JIndex index;
	while (iter.Next(&index))
		{
		JAssertEqual(expect[j], index);
		j++;
		}
	JAssertEqual(3, j);
	}

	JSubset s6 = s3.Complement();
	verify(s6, "10TTFTTTFFTT");

	s6 = s3;
	JAssertEqual(3, s6.GetElementCount());
	verify(s6, "10FFTFFFTTFF");

	for (i=1; i<=20; i++)
		{
		JAssertTrue(JGetRandomSample(&s1, 3));
		JAssertEqual(3, s1.GetElementCount());
		}

// JGetRandomSample()

	JIndex index;

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
			JSubsetIterator iter = (sampleList.GetElement(j))->NewIterator();
			while (iter.Next(&index))
				{
				hist[j-1]->IncrementCount(index, 1);
				assert( !(sampleList.GetElement(3-j))->Contains(index) );
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
			JSubsetIterator iter = (sampleList.GetElement(j))->NewIterator();
			while (iter.Next(&index))
				{
				hist[j-1]->IncrementCount(index, 1);
				assert( !(sampleList.GetElement(3-j))->Contains(index) );
				}
			}
		}
	sampleList.DeleteAll();

	cout << endl;
	cout << "Probability of each element (3,4 from sample of 7) being in the subset:" << endl;
	cout << "(averaged over ensemble of 5000 subsets)" << endl << endl;
	cout << "1) " << h1.ConvertToProbabilities() << endl;
	cout << "2) " << h2.ConvertToProbabilities() << endl;
}
