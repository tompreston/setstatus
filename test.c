#include <stdio.h>
#include <stdlib.h>
#include "battery.h"

int main(void)
{
	Battery battery = init_battery();
	printf("remaining: %d\n", battery->remaining);
	printf("max: %d\n", battery->max);
	printf("percent: %d%%\n", battery->percent);
	printf("seconds left: %d\n", battery->seconds_remaining);
	printf("state: %d\n", battery->state);

	update_battery(&battery);
	free(battery);
}
