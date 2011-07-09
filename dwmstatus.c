/* dwmstatus
 * Author:      Thomas Preston
 * Description: This is basically a re-write of dwmstatus from the suckless.org
 *              website. I'm doing it my way so I understand everything and so
 *              I can get some much needed C practice. :-)
 */
#include <stdio.h>
#include <stdlib.h>
#define __USE_BSD
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <X11/Xlib.h>

#include "battery.h"

#define UPDATE_INTERVAL 90

static Display * display;

static void set_status(char * status)
{
	XStoreName(display, DefaultRootWindow(display), status);
	XSync(display, False);
}

static char * get_day(int weekday)
{
	switch (weekday)
	{
		case 0:  return "Sunday";
		case 1:  return "Monday";
		case 2:  return "Tuesday";
		case 3:  return "Wednesday";
		case 4:  return "Thursday";
		case 5:  return "Friday";
		case 6:  return "Saturday";
		default: return "err";
	}
}

static char * get_month(int month)
{
	switch (month)
	{
		case 0:  return "January";
		case 1:  return "February";
		case 2:  return "March";
		case 3:  return "April";
		case 4:  return "May";
		case 5:  return "June";
		case 6:  return "July";
		case 7:  return "August";
		case 8:  return "September";
		case 9:  return "October";
		case 10: return "November";
		case 11: return "December";
		default: return "err";
	}
}

static char * get_time(void)
{
	time_t epoch_time = time(NULL);
	struct tm * time = localtime(&epoch_time);
	
	char str_time[29]; /* 9day, 1sp, 2mday, 1sp, 9mon, 1sp, 5time, 1nullterm */
	sprintf(str_time, "%s %d %s %d:%02d",
		get_day(time->tm_wday),
		time->tm_mday,
		get_month(time->tm_mon),
		time->tm_hour,
		time->tm_min);
	
	return strdup(str_time);
}

static char * secs2min_str(int seconds)
{
	if (seconds <= 0) return "0";

	int hours = seconds / 3600;
	int seconds_after_hours = seconds % 3600;
	int mins  = seconds_after_hours / 60;

	/* 6 characters allows for three digit hours. That's some battery you have there... */
	char timestring[7];
	sprintf(timestring, "%d:%02d", hours, mins);
	return strdup(timestring);
}

static char * get_nice_batt_time(enum chargestate state, int seconds_remaining)
{
	switch (state)
	{
		case charging:
		case charged:
			return strdup("AC"); /* because we are going to be freeing later */
			break;
		case discharging:
			return secs2min_str(seconds_remaining);
			break;
		default:
			return "??";
	}
}

int main(void)
{
	char status[100];

	/* try to open the display */
	if (!(display = XOpenDisplay(NULL)))
	{
		fprintf(stderr, "dwmstatus: could not open the display :-(\n");
		return 1;
	}

	/* every interval update the status */
	for (;;sleep(UPDATE_INTERVAL))
	{
		char * time         = get_time();
		Battery battery     = init_battery();
		char * battery_time = get_nice_batt_time(battery->state, battery->seconds_remaining);

		sprintf(status, "%s | B: %d%% (%s) | %s", "music",
			battery->percent,
			battery_time,
			time);

		set_status(status);

		free(time);
		free(battery);
		free(battery_time);
	}
}
