#include "nvrtime.h"

static bool is_bcd;
static bool is_24hr;

static uint8_t get_byte_pos(FILE *f, long pos)
{
	uint8_t ret = 0;
	/* TODO: error handling */
	if (fseek(f, pos, SEEK_SET) == 0) {
		fread(&ret, sizeof(ret), 1, f);
	}
	return ret;
}

static void set_byte_pos(FILE *f, long pos, uint8_t val)
{
	/* TODO: error handling */
	if (fseek(f, pos, SEEK_SET) == 0) {
		fwrite(&val, sizeof(val), 1, f);
	}
}

static void get_flags(FILE *f)
{
	uint8_t reg_b = get_byte_pos(f, RTC_REGB);
	is_bcd = !(reg_b & REGB_DM);
	is_24hr = !!(reg_b & REGB_2412);
}

#define BCD_ADJUST(x) (is_bcd ? (((x) / 10 << 4) + (x) % 10) : (x))

static void set_time(FILE *f, const struct tm *tm)
{
	set_byte_pos(f, RTC_SECONDS, BCD_ADJUST(tm->tm_sec));
	set_byte_pos(f, RTC_MINUTES, BCD_ADJUST(tm->tm_min));
	if (is_24hr) {
		set_byte_pos(f, RTC_HOURS, BCD_ADJUST(tm->tm_hour));
	} else {
		int hour12 = tm->tm_hour % 12;
		if (!hour12) hour12 = 12;
		if (tm->tm_hour >= 12) hour12 |= RTC_AMPM;
		set_byte_pos(f, RTC_HOURS, BCD_ADJUST(hour12));
	}
	set_byte_pos(f, RTC_DOW, BCD_ADJUST(tm->tm_wday + 1));
	set_byte_pos(f, RTC_DOM, BCD_ADJUST(tm->tm_mday));
	set_byte_pos(f, RTC_MONTH, BCD_ADJUST(tm->tm_mon + 1));
	set_byte_pos(f, RTC_YEAR, BCD_ADJUST(tm->tm_year % 100));
	/* logic for setting century deliberately left out */
}

static void usage(const char *progname)
{
	printf("usage: %s [-c YEAR_OFFSET] NVR_FILE\n", progname);
	exit(1);
}

int main(int argc, char **argv)
{
	int year_offset = DEFAULT_YEAR_OFFSET;
	const char *fn;
	time_t now;
	struct tm past;
	FILE *f;

	if (argc > 3 && strcmp(argv[1], "-o") == 0) {
		char *begin = argv[1];
		char *end;
		int y = strtol(begin, &end, 10);
		if (begin != end) year_offset = y;
	} else if (argc < 2) {
		usage(argv[0]);
	}
	fn = argv[argc - 1];
	f = fopen(fn, "r+b");
	if (!f) {
		printf("%s: open failed\n", fn);
		return 1;
	}
	get_flags(f);
	time(&now);
	{
		struct tm *now_s = localtime(&now);
		if (!now_s) return 1;
		else memcpy(&past, now_s, sizeof(struct tm));
	}
	past.tm_year += year_offset;
	if (past.tm_year >= 100) {
		printf("warning: offset %d leads to post-Y2K date\n", year_offset);
	}
	set_time(f, &past);
	fclose(f);
	return 0;
}
