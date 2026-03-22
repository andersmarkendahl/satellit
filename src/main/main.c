/*
 Copyright [2018] [Anders Markendahl]

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

  http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
*/

#include "main.h"

void set_log_path(char *buf) {

	char log_name[64] = "/satellit.log";
    strcpy(buf, getenv("SAT_WS"));
	strcat(buf, log_name);
}

int main(int argc, char *argv[])
{
	gholder *gh;
	int start_map = 1;

#ifdef DEBUG
	char log_path[128];
	if (getenv("SAT_WS") == NULL) {
		printf( "SAT_WS not set, source envsetting\n");
		exit(1);
	}
	set_log_path(log_path);
	if (log_init(log_path) != 0) {
		printf("ERR: Initialization of logging failed, quitting, err = %s\n", strerror(errno));
		exit(1);
	}
	if (argc == 2) {
		start_map = atoi(argv[1]);
		LOG("DEBUG: start map set to %s", argv[1]);
	}
#endif

	LOG("####### Starting a new game of %s", argv[0]);

	/* Initialize the global holder */
	gh = gholder_init();

	/* Start up SDL */
	if(gfx_screen_init("Satellit", &gh->res_w, &gh->res_h, &gh->window, &gh->renderer) != 0) {
		LOG("ERR: Could not init screen");
		exit(1);
	}

	/* Set space boundaries based on actual screen resolution (5% margin) */
	space_w_min = gh->res_w * 0.05;
	space_w_max = gh->res_w * 0.95;
	space_h_min = gh->res_h * 0.05;
	space_h_max = gh->res_h * 0.95;

	/* Initialize images */
	gh->imgl = gfx_image_list_init();
	if (gh->imgl == NULL) {
		LOG("ERR: Could not init image list");
		exit(1);
	}
	if (gfx_populate_list_folder(gh->renderer, gh->imgl, "src/graphics/images/") != 0) {
		LOG("ERR: Could not load one or more images");
		exit(1);
	}

	/* Initialize Texts */
	gh->header = gfx_text_init("src/graphics/fonts/Amatic-Bold.ttf", 50);
	if (gh->header == NULL) {
		LOG("ERR: Could not init texts");
		exit(1);
	}

	/* Initialize Audio */
	gh->audiodev = audio_init();
	if (gh->audiodev == NULL) {
		LOG("ERR: Could not init audio");
		exit(1);
	}

	/* Loading Map 1 (unless debug overwrite) */
	if(map_load_next(gh, start_map) != 0) {
		LOG("ERR: Loading map failed");
		exit(1);
	}

intro:
	gholder_state_intro(gh);

	gholder_state_runtime(gh);

	gholder_state_finish(gh);

	if(gh->state == STATE_INTRO) {
		goto intro;
	}

	/* Exit the program and cleanup */
	audio_destroy(gh->audiodev);
	gfx_text_destroy(gh->header);
	gfx_screen_destroy(gh->window, gh->renderer);
	gholder_destroy(gh);

#ifdef DEBUG
	if (log_destroy() != 0) {
		printf("ERR: Shutting down logging failed, err = %s\n", strerror(errno));
	}
#endif

	exit(0);
}
