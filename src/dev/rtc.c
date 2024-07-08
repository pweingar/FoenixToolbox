/*
 * Definitions for access the bq4802LY real time clock
 */

#include "log_level.h"
#define DEFAULT_LOG_LEVEL LOG_ERROR

#include "log.h"
#include "interrupt.h"
#include "gabe_reg.h"
#include "rtc.h"
#include "rtc_reg.h"
#include "simpleio.h"
#include "timers.h"

static long rtc_ticks;

/*
 * Interrupt handler for the real time clock
 */
void rtc_handle_int() {
    unsigned char flags;

    /* Periodic interrupt: increment the ticks counter */
    flags = *RTC_FLAGS;
    rtc_ticks++;
}

/*
 * Initialize the RTC
 */
void rtc_init() {
    unsigned char flags;
    unsigned char rates;
    unsigned char enables;

    log(LOG_TRACE, "rtc_init");

    int_disable(INT_RTC);

    /* Make sure the RTC is on */
    *RTC_CTRL = (*RTC_CTRL & 0x07) | RTC_STOP;

    /*
     * For the moment: Every so often, the RTC interrupt gets acknowledged
     * without clearing the flags. Until I can sort out why, I will use
     * the SOF A interrupt as a surrogate for the RTC jiffie timer
     */

    // /* Set the periodic interrupt to 15 millisecs */
    // *RTC_RATES = RTC_RATE_15ms;
    //
    // int_register(INT_RTC, rtc_handle_int);
    //
    // /* Enable the periodic interrupt */
    // flags = *RTC_FLAGS;
    // *RTC_ENABLES = RTC_PIE;
    // rtc_ticks = 0;
    //
    // int_enable(INT_RTC);
}

/*
 * Make sure the RTC tick counter is enabled
 */
void rtc_enable_ticks() {
    /* Set the periodic interrupt to 15 millisecs */
    *RTC_RATES = RTC_RATE_15ms;

    unsigned char flags = *RTC_FLAGS;

    *RTC_ENABLES = RTC_PIE;

    int_enable(INT_RTC);
}

/**
 * Register a function to be called periodically
 *
 * @param rate the rate at which the function should be called using the bq4802LY periodic rate values (0 to disable)
 * @param handler a pointer to a function from void to void to be called
 * @return 0 on success, any other number is an error
 */
short rtc_register_periodic(short rate, FUNC_V_2_V handler) {
    if (rate == 0) {
        int_disable(INT_RTC);
        *RTC_RATES = 0;
        *RTC_ENABLES &= ~RTC_PIE;
        
    } else {
        int_register(INT_RTC, handler);
        *RTC_RATES = rate;
        unsigned char flags = *RTC_FLAGS;
        *RTC_ENABLES = RTC_PIE;
        int_enable(INT_RTC);
    }

	return 0;
}

/*
 * Set the time on the RTC
 *
 * Inputs:
 * time = pointer to a t_time record containing the correct time
 */
SYSTEMCALL void rtc_set_time(p_time time) {
    unsigned char ctrl;
    unsigned char century_bcd, year_bcd, month_bcd, day_bcd;
    unsigned char hour_bcd, minute_bcd, second_bcd;
    unsigned short century;
    unsigned short year;

    century = time->year / 100;
    year = time->year - (century * 100);

    /* Compute the BCD values for the time */

    century_bcd = i_to_bcd(century);
    year_bcd = i_to_bcd(year);
    month_bcd = i_to_bcd(time->month);
    day_bcd = i_to_bcd(time->day);
    hour_bcd = i_to_bcd(time->hour);
    minute_bcd = i_to_bcd(time->minute);
    second_bcd = i_to_bcd(time->second);

	INFO1("Century: %02d", century_bcd);
    INFO1("Year: %04d", year_bcd);
    INFO1("Month: %02d", month_bcd);
    INFO1("Day: %02d", day_bcd);
    INFO1("Hour: %02d", hour_bcd);
    INFO1("Minute: %02d", minute_bcd);
    INFO1("Second: %02d", second_bcd);

    if (!time->is_24hours) {
        if (time->is_pm) {
            hour_bcd = hour_bcd | 0x80;
        }
    }
    minute_bcd = i_to_bcd(time->minute);
    second_bcd = i_to_bcd(time->second);

    /* Temporarily disable updates to the clock */
    ctrl = *RTC_CTRL;
    *RTC_CTRL = ctrl | RTC_UTI;
    INFO("RTC Disabled");
    INFO1("RTC Rates: %02x", *RTC_RATES);
    INFO1("RTC Enables: %02x", *RTC_ENABLES);
    INFO1("RTC Flags: %02x", *RTC_FLAGS);
    INFO1("RTC Control: %02x", *RTC_CTRL);

    /* Set the time in the RTC */

    *RTC_CENTURY = century_bcd;
    *RTC_YEAR = year_bcd;
    *RTC_MONTH = month_bcd;
    *RTC_DAY = day_bcd;
    *RTC_HOUR = hour_bcd;
    *RTC_MIN = minute_bcd;
    *RTC_SEC = second_bcd;

    /* Verify */

    century_bcd = *RTC_CENTURY;
    year_bcd = *RTC_YEAR;
    month_bcd = *RTC_MONTH;
    day_bcd = *RTC_DAY;
    hour_bcd = *RTC_HOUR;
    minute_bcd = *RTC_MIN;
    second_bcd = *RTC_SEC;
    INFO1("REG Century: %02d", century_bcd);
    INFO1("REG Year: %02d", year_bcd);
    INFO1("REG Month: %02d", month_bcd);
    INFO1("REG Day: %02d", day_bcd);
    INFO1("REG Hour: %02d", hour_bcd);
    INFO1("REG Minute: %02d", minute_bcd);
    INFO1("REG Second: %02d", second_bcd);

    /* Set the 24/12 hour control bit if needed */
    if (time->is_24hours) {
        ctrl = ctrl | RTC_2412;
    }

    /* Re-enable updates to the clock */
    *RTC_CTRL = (ctrl & 0x07) | RTC_STOP;
    INFO("RTC Enabled");
    INFO1("RTC Rates: %02x", *RTC_RATES);
    INFO1("RTC Enables: %02x", *RTC_ENABLES);
    INFO1("RTC Flags: %02x", *RTC_FLAGS);
    INFO1("RTC Control: %02x", *RTC_CTRL);
}

/*
 * Get the time on the RTC
 *
 * Inputs:
 * time = pointer to a t_time record in which to put the current time
 */
SYSTEMCALL void rtc_get_time(p_time time) {
    unsigned char ctrl;
    unsigned char century_bcd, year_bcd, month_bcd, day_bcd;
    unsigned char hour_bcd, minute_bcd, second_bcd;

    /* Temporarily disable updates to the clock */
    ctrl = *RTC_CTRL;
    *RTC_CTRL = ctrl | RTC_UTI;
    INFO("RTC Disabled");
    INFO1("RTC Rates: %02x", *RTC_RATES);
    INFO1("RTC Enables: %02x", *RTC_ENABLES);
    INFO1("RTC Flags: %02x", *RTC_FLAGS);
    INFO1("RTC Control: %02x", *RTC_CTRL);

    if (*RTC_CTRL & RTC_2412) {
        time->is_24hours = 1;
    } else {
        time->is_24hours = 0;
    }

    century_bcd = *RTC_CENTURY;
    year_bcd = *RTC_YEAR;
    month_bcd = *RTC_MONTH;
    day_bcd = *RTC_DAY;
    hour_bcd = *RTC_HOUR;
    minute_bcd = *RTC_MIN;
    second_bcd = *RTC_SEC;

    /* Re-enable updates to the clock */
    *RTC_CTRL = (ctrl & 0x07) | RTC_STOP;
    INFO("RTC Enabled");
    INFO1("RTC Rates: %02x", *RTC_RATES);
    INFO1("RTC Enables: %02x", *RTC_ENABLES);
    INFO1("RTC Flags: %02x", *RTC_FLAGS);
    INFO1("RTC Control: %02x", *RTC_CTRL);

    INFO1("Century: %02d", century_bcd);
    INFO1("Year: %02d", year_bcd);
    INFO1("Month: %02d", month_bcd);
    INFO1("Day: %02d", day_bcd);
    INFO1("Hour: %02d", hour_bcd);
    INFO1("Minute: %02d", minute_bcd);
    INFO1("Second: %02d", second_bcd);

    /* Fill out the time record */

    time->year = bcd_to_i(century_bcd) * 100 + bcd_to_i(year_bcd);
    time->month = bcd_to_i(month_bcd);
    time->day = bcd_to_i(day_bcd);
    time->hour = bcd_to_i(hour_bcd & 0x7f);
    time->is_pm = ((hour_bcd & 0x80) == 0x80) ? 1 : 0;
    time->minute = bcd_to_i(minute_bcd);
    time->second = bcd_to_i(second_bcd);
}

/*
 * Get the number of jiffies since the system last booted.
 *
 * NOTE: a jiffie is 1/60 of a second. This timer will not be
 *       100% precise, so it should be used for timeout purposes
 *       where precision is not critical.
 *
 * Returns:
 * the number of jiffies since the last reset
 */
long rtc_get_jiffies() {
    return 0; // timers_jiffies();
}

