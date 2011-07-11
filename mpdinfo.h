typedef struct mpd_info * MPDinfo;
enum mpd_playing_state {playing, paused, stopped};

struct mpd_info
{
	enum mpd_playing_state state;
	char * title;
	char * artist;
};

MPDinfo init_mpdinfo(void);
