typedef struct battery * Battery;
enum chargestate {charging, charged, discharging};

struct battery
{
	int remaining;
	int max;
	int percent;
	int seconds_remaining;
	enum chargestate state;
};

Battery init_battery(void);
void update_battery(Battery *);
