/******************************************************************************
 GPMSystemStats.cpp

	Displays the CPU history and memory usage.

	BASE CLASS = JXWidget

	Copyright (C) 2011 by John Lindal.

 ******************************************************************************/

#include "GPMSystemStats.h"
#include "GPMProcessList.h"
#include "gpmGlobals.h"
#include <JXWindowPainter.h>
#include <JXColorManager.h>
#include <JRegex.h>
#include <jStreamUtil.h>
#include <unistd.h>
#include <jAssert.h>

const JCoordinate kCPUHistoryBarWidth    = 2;
const JCoordinate kCPUHistoryMarginWidth = 1;
const JCoordinate kDisplayMarginWidth    = 5;

/******************************************************************************
 Constructor

 ******************************************************************************/

GPMSystemStats::GPMSystemStats
	(
	GPMProcessList*		processList,
	JXContainer*		enclosure,
	const HSizingOption	hSizing,
	const VSizingOption	vSizing,
	const JCoordinate	x,
	const JCoordinate	y,
	const JCoordinate	w,
	const JCoordinate	h
	)
	:
	JXWidget(enclosure, hSizing, vSizing, x,y, w,h),
	itsUID(getuid()),
	itsProcessList(processList),
	itsMaxCPU(100)
{
	itsCPUHistory = jnew JArray<CPU>();
	assert( itsCPUHistory != nullptr );

	ListenTo(itsProcessList);
}

/******************************************************************************
 Destructor

	We don't deallocate our colors because we always need the same ones
	and we will probably be used again later.

 ******************************************************************************/

GPMSystemStats::~GPMSystemStats()
{
	jdelete itsCPUHistory;
}

/******************************************************************************
 DrawBorder (virtual protected)

 ******************************************************************************/

void
GPMSystemStats::DrawBorder
	(
	JXWindowPainter&	p,
	const JRect&		frame
	)
{
}

/******************************************************************************
 Draw (virtual protected)

 ******************************************************************************/

void
GPMSystemStats::Draw
	(	
	JXWindowPainter&	p,
	const JRect&		clip
	)
{
	// update stats

	CPU cpuPercentage;
	JFloat userMemoryPercentage, otherMemoryPercentage;
	ComputeStats(&cpuPercentage.user, &cpuPercentage.other,
				 &userMemoryPercentage, &otherMemoryPercentage);

	itsCPUHistory->AppendElement(cpuPercentage);
	itsCPUHistory->RemoveElement(1);

	// detect multiple CPU's by looking for > 100% usages

	while (cpuPercentage.user + cpuPercentage.other > itsMaxCPU + 5)
		{
		itsMaxCPU += 100;
		}

	// render memory usage

	const JColorID otherMem = JColorManager::GetDarkRedColor();
	const JColorID userMem  = JColorManager::GetRedColor();
	const JColorID freeMem  = JColorManager::GetGreenColor();
	const JColorID cpuBack  = JColorManager::GetBlackColor();
	const JColorID userCPU  = JColorManager::GetGreenColor();
	const JColorID otherCPU = JColorManager::GetRedColor();

	const JRect bounds = GetBounds();

	JRect r = bounds;
	r.left  = r.right - bounds.height();
	r.Shift(-kDisplayMarginWidth, 0);

	const JFloat angle1 = 360 * otherMemoryPercentage / 100;
	const JFloat angle2 = 360 * userMemoryPercentage / 100;

	p.SetFilling(true);
	p.SetPenColor(otherMem);
	p.JPainter::Arc(r, 90, -360);
	p.SetPenColor(userMem);
	p.JPainter::Arc(r, 90 - angle1, -360 + angle1);
	p.SetPenColor(freeMem);
	p.JPainter::Arc(r, 90 - angle1 - angle2, - 360 + angle1 + angle2);

	// render cpu history

	r        = bounds;
	r.left  += kDisplayMarginWidth;
	r.right -= bounds.height() + 2*kDisplayMarginWidth;

	p.SetPenColor(cpuBack);
	p.JPainter::Rect(r);

	r.Shrink(1,1);

	const JSize historyCount = r.width() / (kCPUHistoryBarWidth + kCPUHistoryMarginWidth);
	if (itsCPUHistory->GetElementCount() > historyCount)
		{
		itsCPUHistory->RemoveNextElements(1, itsCPUHistory->GetElementCount() - historyCount);
		}
	else
		{
		CPU blank;
		blank.user = blank.other = 0;
		while (itsCPUHistory->GetElementCount() < historyCount)
			{
			itsCPUHistory->PrependElement(blank);
			}
		}

	r.left              = r.right - kCPUHistoryBarWidth;
	const JCoordinate h = r.height();

	for (JIndex i=historyCount; i>=1; i--)
		{
		const CPU cpu = itsCPUHistory->GetElement(i);

		p.SetPenColor(userCPU);
		r.top = r.bottom - JRound(h * (cpu.user + cpu.other) / itsMaxCPU);
		p.JPainter::Rect(r);

		p.SetPenColor(otherCPU);
		r.top = r.bottom - JRound(h * cpu.other / itsMaxCPU);
		p.JPainter::Rect(r);

		r.Shift(-kCPUHistoryBarWidth-kCPUHistoryMarginWidth, 0);
		}
}

/******************************************************************************
 Receive (virtual protected)

 ******************************************************************************/

void
GPMSystemStats::Receive
	(
	JBroadcaster*	sender,
	const Message&	message
	)
{
	if (sender == itsProcessList && message.Is(GPMProcessList::kListChanged))
		{
		Refresh();
		}
	else
		{
		JXWidget::Receive(sender, message);
		}
}

/******************************************************************************
 ComputeStats (private)

 ******************************************************************************/

static const JRegex freeMemoryPattern   = "^MemFree:\\s*([0-9]+)";
static const JRegex bufferMemoryPattern = "^Buffers:\\s*([0-9]+)";
static const JRegex cacheMemoryPattern  = "^Cached:\\s*([0-9]+)";

void
GPMSystemStats::ComputeStats
	(
	JFloat* userCPUPercentage,
	JFloat* otherCPUPercentage,
	JFloat* userMemoryPercentage,
	JFloat* otherMemoryPercentage
	)
	const
{
	*userCPUPercentage     = 0;
	*otherCPUPercentage    = 0;
	*userMemoryPercentage  = 0;
	*otherMemoryPercentage = 0;

	JSize count = itsProcessList->GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const GPMProcessEntry* e = itsProcessList->GetProcessEntry(i);
		if (e->GetUID() == itsUID)
			{
			*userCPUPercentage += e->GetPercentCPU();
			}
		else
			{
			*otherCPUPercentage += e->GetPercentCPU();
			}
		}

	const JPtrArray<GPMProcessEntry>& hidden = itsProcessList->GetHiddenProcesses();
	count = hidden.GetElementCount();
	for (JIndex i=1; i<=count; i++)
		{
		const GPMProcessEntry* e = hidden.GetElement(i);
		*otherCPUPercentage     += e->GetPercentCPU();
		}

#ifdef _J_HAS_PROC
	{
	JSize totalMem, freeMem = 0, otherMem = 0, foundCount = 0;
	if (GPMGetSystemMemory(&totalMem))
		{
		std::ifstream ms("/proc/meminfo");
		JString line;
		while (ms.good() && !ms.eof())
			{
			line = JReadLine(ms);

			const JStringMatch m1 = freeMemoryPattern.Match(line, JRegex::kIncludeSubmatches),
							   m2 = bufferMemoryPattern.Match(line, JRegex::kIncludeSubmatches),
							   m3 = cacheMemoryPattern.Match(line, JRegex::kIncludeSubmatches);
			if (!m1.IsEmpty())
				{
				m1.GetSubstring(1).ConvertToUInt(&freeMem);	// usually kB
				foundCount++;
				}
			else if (!m2.IsEmpty() || !m3.IsEmpty())
				{
				const JString s = m3.IsEmpty() ? m2.GetSubstring(1) : m3.GetSubstring(1);
				JSize mem;
				s.ConvertToUInt(&mem);	// usually kB
				otherMem += mem;
				foundCount++;
				}

			if (foundCount >= 3)
				{
				break;
				}
			}

		*userMemoryPercentage  = 100 * (1 - (freeMem + otherMem) / JFloat(totalMem));
		*otherMemoryPercentage = 100 * otherMem / JFloat(totalMem);
		}
	}
#endif
}
