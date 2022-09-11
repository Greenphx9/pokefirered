#include "global.h"
#include "event_data.h"
#include "rtc.h"

struct Clock gClock;

static u16 sRtcErrorStatus;
static u16 sSavedIme;
static u8 sRtcFrameCount;
static struct SiiRtcInfo sRtc;

static const struct SiiRtcInfo sRtcDummy = // 2000 Jan 1, 10:00 AM
{
    .year = 0,
    .month = MONTH_JAN,
    .day = 1,
    .hour = 0x10, // this is in BCD
};

static const s32 sNumDaysInMonths[12] =
{
    [MONTH_JAN - 1] = 31,
    [MONTH_FEB - 1] = 28,
    [MONTH_MAR - 1] = 31,
    [MONTH_APR - 1] = 30,
    [MONTH_MAY - 1] = 31,
    [MONTH_JUN - 1] = 30,
    [MONTH_JUL - 1] = 31,
    [MONTH_AUG - 1] = 31,
    [MONTH_SEP - 1] = 30,
    [MONTH_OCT - 1] = 31,
    [MONTH_NOV - 1] = 30,
    [MONTH_DEC - 1] = 31,
};

void RtcDisableInterrupts(void)
{
	sSavedIme = REG_IME;
	REG_IME = 0;
}

void RtcRestoreInterrupts(void)
{
	REG_IME = sSavedIme;
}

u32 ConvertBcdToBinary(u8 bcd)
{
	if (bcd > 0x9F)
		return 0xFF;

	if ((bcd & 0xF) <= 9)
		return (10 * ((bcd >> 4) & 0xF)) + (bcd & 0xF);
	else
		return 0xFF;
}

static bool8 IsLeapYear(u32 year)
{
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

void RtcInit(void)
{
    u8 probeResult;
	sRtcErrorStatus = 0;

	RtcDisableInterrupts();
	SiiRtcUnprotect();
	probeResult = SiiRtcProbe();
	RtcRestoreInterrupts();

	if ((probeResult & 0xF) != 1)
	{
		sRtcErrorStatus = RTC_INIT_ERROR;
		return;
	}

	if (probeResult & 0xF0)
		sRtcErrorStatus = RTC_INIT_WARNING;
	else
		sRtcErrorStatus = 0;

	RtcGetRawInfo(&sRtc);
	sRtcErrorStatus = RtcCheckInfo(&sRtc);
}

u16 RtcGetErrorStatus(void)
{
	return sRtcErrorStatus;
}

void RtcGetInfo(struct SiiRtcInfo *rtc)
{
	if (sRtcErrorStatus & RTC_ERR_FLAG_MASK)
		*rtc = sRtcDummy;
	else
		RtcGetRawInfo(rtc);
}

void RtcGetDateTime(struct SiiRtcInfo *rtc)
{
	RtcDisableInterrupts();
	SiiRtcGetDateTime(rtc);
	RtcRestoreInterrupts();
}

void RtcGetStatus(struct SiiRtcInfo *rtc)
{
	RtcDisableInterrupts();
	SiiRtcGetStatus(rtc);
	RtcRestoreInterrupts();
}

void RtcGetRawInfo(struct SiiRtcInfo *rtc)
{
	RtcGetStatus(rtc);
	RtcGetDateTime(rtc);
}

u16 RtcCheckInfo(struct SiiRtcInfo *rtc)
{
	u16 errorFlags = 0;
	s32 year;
	s32 month;
	s32 value;

	if (rtc->status & SIIRTCINFO_POWER)
		errorFlags |= RTC_ERR_POWER_FAILURE;

	if (!(rtc->status & SIIRTCINFO_24HOUR))
		errorFlags |= RTC_ERR_12HOUR_CLOCK;

	year = ConvertBcdToBinary(rtc->year);

	if (year == 0xFF)
		errorFlags |= RTC_ERR_INVALID_YEAR;

	month = ConvertBcdToBinary(rtc->month);

	if (month == 0xFF || month == 0 || month > 12)
		errorFlags |= RTC_ERR_INVALID_MONTH;

	value = ConvertBcdToBinary(rtc->day);

	if (value == 0xFF)
		errorFlags |= RTC_ERR_INVALID_DAY;

	if (month == MONTH_FEB)
	{
		if (value > IsLeapYear(year) + sNumDaysInMonths[month - 1])
			errorFlags |= RTC_ERR_INVALID_DAY;
	}
	else
	{
		if (value > sNumDaysInMonths[month - 1])
			errorFlags |= RTC_ERR_INVALID_DAY;
	}

	value = ConvertBcdToBinary(rtc->hour);

	if (value > 24)
		errorFlags |= RTC_ERR_INVALID_HOUR;

	value = ConvertBcdToBinary(rtc->minute);

	if (value > 60)
		errorFlags |= RTC_ERR_INVALID_MINUTE;

	value = ConvertBcdToBinary(rtc->second);

	if (value > 60)
		errorFlags |= RTC_ERR_INVALID_SECOND;

	return errorFlags;
}

static void UpdateClockFromRtc(struct SiiRtcInfo *rtc)
{
	gClock.year = ConvertBcdToBinary(rtc->year) + 2000;
	gClock.month = ConvertBcdToBinary(rtc->month);
	gClock.day = ConvertBcdToBinary(rtc->day);
	gClock.dayOfWeek = ConvertBcdToBinary(rtc->dayOfWeek);
	gClock.hour = ConvertBcdToBinary(rtc->hour);
	gClock.minute = ConvertBcdToBinary(rtc->minute);
	gClock.second = ConvertBcdToBinary(rtc->second);
}

void RtcCalcLocalTime(void)
{
    if (sRtcFrameCount++ >= 60)
    {
        RtcGetInfo(&sRtc);
        UpdateClockFromRtc(&sRtc);
        sRtcFrameCount = 0;
    }
}

void ForceClockUpdate(void)
{
	sRtcFrameCount = 0;
}

void DirectClockUpdate(void)
{
	sRtcFrameCount = 0;
	RtcCalcLocalTime();
}
