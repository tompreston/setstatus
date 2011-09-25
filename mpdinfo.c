#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mpd/client.h>
#include <mpd/status.h>
#include <mpd/entity.h>
#include <mpd/search.h>
#include <mpd/tag.h>

#include "mpdinfo.h"

#undef DEBUG

MPDinfo init_mpdinfo(void)
{
	/* connect to mpd */
	struct mpd_connection * mpdconn = mpd_connection_new("localhost", 6600, 0);
	if (mpd_connection_get_error(mpdconn) != MPD_ERROR_SUCCESS)
	{
		#ifdef DEBUG
		fprintf(stderr, "%s\n", mpd_connection_get_error_message(mpdconn));
		#endif
		mpd_connection_free(mpdconn);
		return NULL;
	}
	
	/* connection was successful, make room for some data! */
	struct mpd_info * mpd_information = malloc(sizeof(struct mpd_info));
	mpd_information->state  = stopped;
	mpd_information->artist = NULL;
	mpd_information->title  = NULL;

	
	/* send commands to mpd */
	mpd_command_list_begin(mpdconn, true);
		mpd_send_status(mpdconn);
		mpd_send_current_song(mpdconn);
	mpd_command_list_end(mpdconn);
	
	/* get the status */
	struct mpd_status * status = mpd_recv_status(mpdconn);
	if (status == NULL)
	{
		#ifdef DEBUG
		fprintf(stderr,"%s\n", mpd_connection_get_error_message(mpdconn));
		#endif
		mpd_connection_free(mpdconn);
		free(mpd_information);
		return NULL;
	}
	
	#ifdef DEBUG
	/* If we recieved the status, but there was still an error, print it */
	if (mpd_status_get_error(status) != NULL)
		fprintf(stderr, "error: %s\n", mpd_status_get_error(status));
	#endif
	
	/* set the state */
	if (mpd_status_get_state(status) == MPD_STATE_PLAY)
		mpd_information->state = playing;
	else if (mpd_status_get_state(status) == MPD_STATE_PAUSE)
		mpd_information->state = paused;
	else
		mpd_information->state = stopped;
	
	/* print any errors */
	if (mpd_connection_get_error(mpdconn) != MPD_ERROR_SUCCESS)
	{
		#ifdef DEBUG
		fprintf(stderr,"%s\n", mpd_connection_get_error_message(mpdconn));
		#endif
		mpd_connection_free(mpdconn);
		free(mpd_information);
		return NULL;
	}
	
	/* move onto the response from the second command we sent */
	mpd_response_next(mpdconn);

	/* song information */
	struct mpd_song * song = mpd_recv_song(mpdconn);
	if (song != NULL)
	{
		const char * artist = mpd_song_get_tag(song, MPD_TAG_ARTIST, 0);
		if (artist == NULL)
			mpd_information->artist = strdup("?");
		else
			mpd_information->artist = strdup(artist);

		const char * title = mpd_song_get_tag(song, MPD_TAG_TITLE,  0);
		if (title == NULL)
			mpd_information->title = strdup("?");
		else
			mpd_information->title  = strdup(title);

		mpd_song_free(song);
	}
	
	/* more error handling... */
	if (mpd_connection_get_error(mpdconn) != MPD_ERROR_SUCCESS)
	{
		#ifdef DEBUG
		fprintf(stderr,"%s\n", mpd_connection_get_error_message(mpdconn));
		#endif
		mpd_connection_free(mpdconn);
		free(mpd_information);
		return NULL;
	}
	
	/* ...this is not sexy code */
	if (!mpd_response_finish(mpdconn))
	{
		#ifdef DEBUG
		fprintf(stderr,"%s\n", mpd_connection_get_error_message(mpdconn));
		#endif
		mpd_connection_free(mpdconn);
		free(mpd_information);
		return NULL;
	}

	mpd_status_free(status);
	mpd_connection_free(mpdconn);
	
	return mpd_information;
}
