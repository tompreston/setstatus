/* battery.c
 * Description: Gives infomation on the current battery
 * Author     : Thomas Preston
 */
#include <stdio.h>
#include <stdlib.h>
#include "battery.h"

static int get_max_battery(void)
{
	int max_capacity = -1;

	char thisline[100];
	FILE * batt_info  = fopen("/proc/acpi/battery/BAT0/info",  "r");
	while (fgets(thisline, 100, batt_info))
		if (!strncmp(thisline, "design capacity", 15))
		{
			sscanf(thisline+16, "%*[ ]%d%*[^\n]", &max_capacity);
			break;
		}
	
	fclose(batt_info);
	return max_capacity;
}

static int get_remaining_battery(void)
{
	int remaining_capacity = -1;

	char thisline[100];
	FILE * batt_state = fopen("/proc/acpi/battery/BAT0/state", "r");
	while (fgets(thisline, 100, batt_state))
		if (!strncmp(thisline, "remaining capacity", 18))
		{
			sscanf(thisline+19, "%*[ ]%d%*[^\n]", &remaining_capacity);
			break;
		}
	
	fclose(batt_state);
	return remaining_capacity;
}

static int get_battery_percent(void)
{
	return (int) ((float) get_remaining_battery() / (float) get_max_battery() * 100);
}

static enum chargestate get_battery_state(void)
{
	char chargingstate[12]; /* discharging\0 has 12 characters */

	char thisline[100];
	FILE * batt_state = fopen("/proc/acpi/battery/BAT0/state", "r");
	while (fgets(thisline, 100, batt_state))
		if (!strncmp(thisline, "charging state", 14))
		{
			sscanf(thisline+25, "%s%*[^\n]", &chargingstate);
			break;
		}
	
	fclose(batt_state);
	
	if (!strcmp(chargingstate, "charging"))
		return charging;
	else if (!strcmp(chargingstate, "charged"))
		return charged;
	else if (!strcmp(chargingstate, "discharging"))
		return discharging;
}

static int get_rate(void)
{
	int rate = -1;

	char thisline[100];
	FILE * batt_state = fopen("/proc/acpi/battery/BAT0/state", "r");
	while (fgets(thisline, 100, batt_state))
		if (!strncmp(thisline, "present rate", 12))
		{
			sscanf(thisline+13, "%*[ ]%d%*[^\n]", &rate);
			break;
		}
	
	fclose(batt_state);
	return rate;
}

static int get_seconds_remaining(void)
{
	float remaining  = (float) get_remaining_battery();
	float rate       = (float) get_rate();
	int seconds_left = (int) (remaining * 60.0 * 60.0 / rate);
	return seconds_left;
}

Battery init_battery(void)
{
	Battery newbattery = (Battery) malloc(sizeof(struct battery));

	newbattery->remaining  = get_remaining_battery();
	newbattery->max        = get_max_battery();
	newbattery->percent    = get_battery_percent();
	newbattery->state      = get_battery_state();
	newbattery->seconds_remaining = get_seconds_remaining();
	
	return newbattery;
}

void update_battery(Battery * battery)
{
	free(*battery);
	*battery = init_battery();
}
