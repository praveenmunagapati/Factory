/*
	sec == 0 は 1/1/1 月曜日 00:00:00
*/

#include "all.h"

#define START_TIME (0 * 86400 + 7 * 3600 +  0 * 60) // 月曜日(0日後) 07:00:00
#define END_TIME   (5 * 86400 + 5 * 3600 + 50 * 60) // 土曜日(5日後) 05:50:00
#define TIME_CYCLE (7 * 86400) // 1週間

#define TRADING_TIME (END_TIME - START_TIME) // 取引可能期間
#define INTERVAL_TIME (TIME_CYCLE - TRADING_TIME) // お休み期間

/*
	sec が、お休み期間の場合、直後の取引可能期間の最初の時間
*/
uint64 Sec2FxTime(uint64 sec)
{
	uint64 count = sec / TIME_CYCLE;
	uint64 rem   = sec % TIME_CYCLE;

	m_range(rem, START_TIME, END_TIME);
	rem -= START_TIME;

	return count * TRADING_TIME + rem;
}
uint64 FxTime2Sec(uint64 fxTime)
{
	uint64 count;
	uint64 rem;

	errorCase(FXTIME_MAX < fxTime);

	count = fxTime / TRADING_TIME;
	rem   = fxTime % TRADING_TIME;

	rem += START_TIME;

	return count * TIME_CYCLE + rem;
}
