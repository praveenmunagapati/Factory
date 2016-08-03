/*
	tSec == 0 �� 1/1/1 ���j�� 00:00:00
*/

#include "all.h"

#define START_TIME (0 * 86400 + 7 * 3600 +  0 * 60) // ���j��(0����) 07:00:00
#define END_TIME   (5 * 86400 + 5 * 3600 + 50 * 60) // �y�j��(5����) 05:50:00
#define TIME_CYCLE (7 * 86400 + 0 * 3600 +  0 * 60) // �P�T��

#define TRADING_TIME (END_TIME - START_TIME) // �������
#define INTERVAL_TIME (TIME_CYCLE - TRADING_TIME) // ���x�ݎ���

uint64 FxTime2TSec(uint64 fxTime)
{
	uint64 count = fxTime / TRADING_TIME;
	uint64 rem   = fxTime % TRADING_TIME;

	return count * TIME_CYCLE + rem + START_TIME;
}
uint64 TSec2FxTime(uint64 tSec)
{
	uint64 count = tSec / TIME_CYCLE;
	uint64 rem   = tSec % TIME_CYCLE;

	return count * TRADING_TIME + rem - START_TIME;
}
