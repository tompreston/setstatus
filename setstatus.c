#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <X11/Xlib.h>
#include <mpd/client.h>
#include <pthread.h>

#include "battery.h"
#include "mpdinfo.h"

#define UPDATE_INTERVAL 1

static void set_status(char * status, Display * display)
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

static void * get_time_str()
{
	time_t epoch_time = time(NULL);
	struct tm * time  = localtime(&epoch_time);
	
	char * str_time;
	asprintf(&str_time, "%s %d %s %d:%02d",
		get_day(time->tm_wday),
		time->tm_mday,
		get_month(time->tm_mon),
		time->tm_hour,
		time->tm_min);
	
	return str_time;
}

static char * secs2min_str(int seconds)
{
	if (seconds <= 0) return strdup("0");

	int hours = seconds / 3600;
	int seconds_after_hours = seconds % 3600;
	int mins  = seconds_after_hours / 60;

	char * timestring;
	asprintf(&timestring, "%d:%02d", hours, mins);
	return timestring;
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
			return strdup("??");
	}
}

static void * get_battery_info_str()
{
	Battery battery = init_battery();

	if (battery == NULL)
		return strdup("");

	char *  battery_time = get_nice_batt_time(battery->state, battery->seconds_remaining);

	/* set the color of the battery */
	const char normal_color = '';
	char bat_color = normal_color;

	if (battery->percent <= 5)
		bat_color = '';
	else if (battery->percent <= 10)
		bat_color = '';

	char * battery_info_str;
	asprintf(&battery_info_str, "%cB: %d%% (%s)%c|",
		bat_color,
		battery->percent,
		battery_time,
		normal_color);

	free(battery_time);
	free(battery);

	return battery_info_str;
}

static void free_mpd_info(MPDinfo mpd_info)
{
	if (mpd_info->artist != NULL)
		free(mpd_info->artist);
	
	if (mpd_info->title != NULL)
		free(mpd_info->title);
	
	free(mpd_info);
}

static char * get_playing_state_str(enum mpd_playing_state state)
{
	switch (state)
	{
		case playing:
			return strdup("playing");
			break;
		case paused:
			return strdup("paused");
			break;
		case stopped:
			return strdup("stopped");
			break;
		default:
			return strdup("err");
	}
}

static void * get_mpd_info_str()
{
	MPDinfo mpd_info = init_mpdinfo();

	if (mpd_info == NULL)
		return strdup("");
	
	char * playing_state_str = get_playing_state_str(mpd_info->state);
	
	char * info_str;
	if (mpd_info->state == stopped)
		asprintf(&info_str, "[%s] |", playing_state_str);
	else
		asprintf(&info_str, "%s by %s [%s] |", mpd_info->title, mpd_info->artist, playing_state_str);

	free_mpd_info(mpd_info);
	free(playing_state_str);
	
	return info_str;
}

int main(void)
{
	for (;;sleep(UPDATE_INTERVAL)) /* every interval update the status */
	{
		Display * display = XOpenDisplay(NULL);
		if (display == NULL)
		{
			fprintf(stderr, "setstatus: could not open the display :-(\n");
			return -1;
		}

		pthread_t mpd_info_thread, battery_info_thread, time_thread;
		pthread_create(&mpd_info_thread,     NULL, get_mpd_info_str,     NULL);
		pthread_create(&battery_info_thread, NULL, get_battery_info_str, NULL);
		pthread_create(&time_thread,         NULL, get_time_str,         NULL);

		void * mpd_info_str, * battery_info_str, * time_str;
		pthread_join(mpd_info_thread,     &mpd_info_str);
		pthread_join(battery_info_thread, &battery_info_str);
		pthread_join(time_thread,         &time_str);

		char * status;
		asprintf(&status, "%s%s %s",
			(char *) mpd_info_str,
			(char *) battery_info_str,
			(char *) time_str);
		
		set_status(status, display);
		
		free(mpd_info_str);
		free(battery_info_str);
		free(time_str);
		free(status);
		
		XCloseDisplay(display);
	}
}
