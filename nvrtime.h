#ifndef __NVRTIME_H
#define __NVRTIME_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/* 2020 -> 1992 era sync */
#define DEFAULT_YEAR_OFFSET -28

/* RTC register and bit definitions
 * excerpt from 86Box/src/nvr_at.c
 */

#define RTC_SECONDS 0
#define RTC_MINUTES 2
#define RTC_HOURS   4
#define RTC_AMPM    0x80 /* PM flag if 12h format in use */
#define RTC_DOW     6
#define RTC_DOM     7
#define RTC_MONTH   8
#define RTC_YEAR    9
#define RTC_REGB    11
#define REGB_DM     0x04
#define REGB_2412   0x02

#endif