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

#include "graphics.h"

SDL_Texture *sdl_image_load(SDL_Renderer *renderer, char *name) {

	/* Load the image using SDL Image */
	SDL_Surface *temp = IMG_Load(name);
	SDL_Texture *image;

	if (temp == NULL) 	{
		return NULL;
	}

	image = SDL_CreateTextureFromSurface(renderer, temp);
	if (image == NULL) 	{
		return NULL;
	}

	SDL_FreeSurface(temp);

	/* Return the processed image */
	return image;
}


enum graphicsReturnCode gfx_screen_init(char *title, int *width, int *height,
	SDL_Window **window, SDL_Renderer **renderer) {

	SDL_DisplayMode current;

	/* Initialise SDL Video */
	if (SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO) < 0) {
		return GRAPHICS_SDL;
	}

	if (SDL_GetCurrentDisplayMode(0, &current) != 0) {
		/* Unable to get native resolution*/
		return GRAPHICS_SDL;
	} else {
		*width = current.w;
		*height = current.h;
	}

#ifdef __APPLE__
	*window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		*width, *height, SDL_WINDOW_FULLSCREEN_DESKTOP);
#else
	*window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
		*width, *height, SDL_WINDOW_FULLSCREEN);
#endif
	if (*window == NULL) {
		return GRAPHICS_SDL;
	}

	*renderer = SDL_CreateRenderer(*window, -1, 0);
	if (*renderer == NULL) {
		SDL_DestroyWindow(*window);
		return GRAPHICS_SDL;
	}

	if (SDL_SetRenderDrawColor(*renderer, 0, 0, 0, 255) != 0) {
		SDL_DestroyWindow(*window);
		SDL_DestroyRenderer(*renderer);
		return GRAPHICS_SDL;
	}

	/* Initialise text management */
	if(TTF_Init() != 0) {
		SDL_DestroyWindow(*window);
		SDL_DestroyRenderer(*renderer);
		return GRAPHICS_TTF;
	}

	return GRAPHICS_OK;
}

void gfx_screen_destroy(SDL_Window *window, SDL_Renderer *renderer) {

	/* Shut dow text management */
	TTF_Quit();

	/* Destroy the renderer */
	if (renderer != NULL) {
		SDL_DestroyRenderer(renderer);
	}
	/* Destroy the window */
	if (window != NULL) {
		SDL_DestroyWindow(window);
	}

	/* Shut down SDL */
	SDL_Quit();
}

gfx_image_list *gfx_image_list_init(void) {

	gfx_image_list *imgl = calloc(1, sizeof(gfx_image_list));

	return imgl;
}

void gfx_image_list_destroy(gfx_image_list *imgl) {

	free(imgl);
}

gfx_text *gfx_text_init(char *font_path, int font_size) {

	gfx_text *text = calloc(1, sizeof(gfx_text));
	text->font = TTF_OpenFont(font_path, font_size);
	if (text->font == NULL) {
		return NULL;
	}
	text->fontcolor.r = 255;
	text->fontcolor.b = 255;
	text->fontcolor.g = 255;
	text->text = NULL;

	return text;
}

void gfx_text_destroy(gfx_text *text) {

	if(text->text != NULL) {
		SDL_DestroyTexture(text->text);
	}
	TTF_CloseFont(text->font);
	free(text);
}

gfx_image *gfx_image_init(char *name) {

	gfx_image *img = calloc(1,sizeof(gfx_image));
	if (img == NULL) {
		return NULL;
	}
	img->name = strdup(name);
	if (img->name == NULL) {
		free(img);
		return NULL;
	}
	return img;
}

enum graphicsReturnCode gfx_image_destroy(gfx_image *img) {

	if(img == NULL) {
		return GRAPHICS_OK;
	}

	if(img->image != NULL) {
		SDL_DestroyTexture(img->image);
	}

	free(img->name);
	free(img);

	return GRAPHICS_OK;
}

enum graphicsReturnCode gfx_image_load(SDL_Renderer *renderer, gfx_image *img, char* path) {

	if(renderer == NULL || img == NULL) {
		return GRAPHICS_ARG;
	}

	if(img->image != NULL) {
		SDL_DestroyTexture(img->image);
	}

	img->image = sdl_image_load(renderer, path);
	if (img->image == NULL) {
		return GRAPHICS_SDL;
	}

	return GRAPHICS_OK;
}

gfx_image * gfx_image_get(gfx_image_list *imgl, char *image) {

	gfx_image *img;

	DL_FOREACH(imgl->head,img) {
		if (strcmp(img->name, image) == 0) {
			return img;
		}
	}
	return NULL;
}

enum graphicsReturnCode gfx_populate_list_folder(SDL_Renderer *renderer, gfx_image_list *imgl, char *folder) {

	gfx_image *img;
	DIR *dir;
	struct dirent *file;
	char img_path[128];

	strcpy(img_path, folder);
	if ((dir = opendir(img_path)) != NULL) {
		while ((file = readdir(dir)) != NULL) {
			if (strcmp(file->d_name,".") == 0 || strcmp(file->d_name,"..") == 0) {
				continue;
			}
			strcat(img_path, file->d_name);
			img = gfx_image_init(file->d_name);
			if (img == NULL) {
				gfx_destroy_list(imgl);
				return GRAPHICS_NOK;
			}
			if (gfx_image_load(renderer, img, img_path) != GRAPHICS_OK) {
				/* Skipping images that could not be loaded */
				gfx_image_destroy(img);
				/* Reset img_dir to top */
				strcpy(img_path, folder);
				continue;
			}
			DL_APPEND(imgl->head,img);
			imgl->n_images++;

			/* Reset img_dir to top */
			strcpy(img_path, folder);
		}
		closedir (dir);
	} else {
		return GRAPHICS_NOK;
	}

	return GRAPHICS_OK;
}

void gfx_destroy_list(gfx_image_list *imgl) {

	gfx_image *img,*tmp;

	DL_FOREACH_SAFE(imgl->head,img,tmp) {
		DL_DELETE(imgl->head,img);
		gfx_image_destroy(img);
		imgl->n_images--;
	}
}

enum graphicsReturnCode gfx_text_set(SDL_Renderer *renderer, gfx_text *text, char *new_txt) {

	SDL_Surface *surf;

	if(strcmp(new_txt, "") == 0 || text == NULL) {
		return GRAPHICS_ARG;
	}

	if(text->text != NULL) {
		SDL_DestroyTexture(text->text);
	}

	surf = TTF_RenderText_Blended(text->font, new_txt, text->fontcolor);
	if(surf == NULL) {
		return GRAPHICS_TTF;
	}

	text->text = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);
	if(text->text == NULL) {
		return GRAPHICS_SDL;
	}

	return GRAPHICS_OK;

}

enum graphicsReturnCode gfx_line_draw(SDL_Renderer *renderer, int s_x, int s_y, int e_x, int e_y) {

	if ((s_x > RES_WIDTH || s_x < 0) ||
		(e_x > RES_WIDTH || e_x < 0) ||
		(s_y > RES_HEIGHT || s_y < 0) ||
		(e_y > RES_HEIGHT || e_y < 0)) {
		return GRAPHICS_ARG;
	}
	if(SDL_SetRenderDrawColor(renderer, 255, 255, 255, SDL_ALPHA_OPAQUE) != 0) {
		return GRAPHICS_SDL;
	}

	if(SDL_RenderDrawLine(renderer, s_x, s_y, e_x, e_y) != 0) {
		return GRAPHICS_SDL;
	}

	return GRAPHICS_OK;

}

enum graphicsReturnCode gfx_surface_draw(SDL_Renderer *renderer, SDL_Texture *image, int x, int y, double angle) {

	SDL_Rect dest;
	int w, h;

	if ((x > RES_WIDTH || x < 0) || (y > RES_HEIGHT || y < 0)) {
		return GRAPHICS_ARG;
	}

	if(image == NULL) {
		return GRAPHICS_OK;
	}

	if(SDL_QueryTexture(image, NULL, NULL, &w, &h) != 0) {
		return GRAPHICS_SDL;
	}

	/* Set the blitting rectangle to the size of the src image */
	dest.x = x-(w/2);
	dest.y = y-(h/2);
	dest.w = w;
	dest.h = h;

	/* Blit the entire image onto the screen at coordinates x and y */
	if(SDL_RenderCopyEx(renderer, image, NULL, &dest, angle, NULL, SDL_FLIP_NONE) != 0) {
		return GRAPHICS_SDL;
	}

	return GRAPHICS_OK;
}
