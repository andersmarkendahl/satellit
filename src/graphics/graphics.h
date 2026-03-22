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

/**
 * @file
 * @brief Graphics handling
 */

#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "common.h"

/**
 * @brief Return codes for the object API.
 */
enum graphicsReturnCode {
	GRAPHICS_OK = 0,	/**< Graphics call OK. */
	GRAPHICS_NOK,		/**< Generic error */
	GRAPHICS_ARG,		/**< Argument error */
	GRAPHICS_SDL,		/**< SDL error, use SDL_GetError() */
	GRAPHICS_TTF,		/**< TTF error, use TTF_GetError() */
};

/**
 * @brief Structure for an image.
 * @see gfx_image_list
 */
typedef struct gfx_image {
	char *name;				/**< Name of image. */
	SDL_Texture *image;		/**< Pointer to actual image. */
	struct gfx_image *next;	/**< List pointer to next object. */
	struct gfx_image *prev; /**< List pointer to previous object. */
} gfx_image;

/**
 * @brief Structure for a text.
 *
 */
typedef struct gfx_text {
	SDL_Texture *text;		/**< Pointer to actual text. */
	TTF_Font* font;			/**< Font used. */
	SDL_Color fontcolor;	/**< Font color. */
	int fontsize;			/**< Font size. */
} gfx_text;

/**
 * @brief Structure for an image list.
 * @see gfx_image
 */
typedef struct gfx_image_list {
	int n_images;			/**< Number of images in list */
	struct gfx_image *head;	/**< Pointer to head of list */
} gfx_image_list;

/**
 * @brief Initalize video and set screen, also init ttf.
 * @param title Title of the video screen.
 * @param width Pointer to int, writes resolution width
 * @param height Pointer to int, writes resolution height
 * @param window Pointer to window pointer to set
 * @param renderer Pointer to renderer pointer to set
 * @return GRAPHICS_OK if passed, other graphicsReturnCode if failed
 * @see graphicsReturnCode
 *
 */
enum graphicsReturnCode gfx_screen_init(char *title, int *width, int *height,
	SDL_Window **window, SDL_Renderer **renderer);

/**
 * @brief Destroy video and screen, also shutdown ttf.
 * @param window Pointer to window to be destroyed.
 * @param renderer Pointer to renderer to be destroyed.
 *
 */
void gfx_screen_destroy(SDL_Window *window, SDL_Renderer *renderer);

/**
 * @brief Allocate an image list.
 * @return Pointer to allocated image list
 *
 */
gfx_image_list *gfx_image_list_init(void);

/**
 * @brief Destroy an image list.
 * @param imgl Pointer to image list to be destroyed.
 *
 */
void gfx_image_list_destroy(gfx_image_list *imgl);

/**
 * @brief Initialize all texts as an image list.
 * If failed use TTF_GetError() to show error.
 * @param font_path Full path to font to be used.
 * @param font_size Size of font.
 * @return Pointer text object if passed, NULL if failed.
 *
 */
gfx_text *gfx_text_init(char *font_path, int font_size);

/**
 * @brief Destroy a text object.
 * @param text Pointer to text to be destroyed
 *
 */
void gfx_text_destroy(gfx_text *text);

/**
 * @brief Initialize an image.
 * @param name ID to use for image.
 * @return Pointer to created image object, NULL if failed.
 * @see gfx_image_destroy
 *
 */
gfx_image *gfx_image_init(char *name);

/**
 * @brief Destroy an image.
 * @param img Pointer to image to destroy.
 * @return GRAPHICS_OK if passed, something else if failed :-).
 * @see gfx_image_init
 *
 */
enum graphicsReturnCode gfx_image_destroy(gfx_image *img);

/**
 * @brief Load image into an gfx_image.
 * @param renderer Pointer to renderer.
 * @param img Pointer to image struct to load image onto.
 * @param path Path to image file to be loaded.
 * @return GRAPHICS_OK if passed, other graphicsReturnCode if failed
 * @see gfx_image_init
 * @see gfx_image_destroy
 * @see graphicsReturnCode
 *
 */
enum graphicsReturnCode gfx_image_load(SDL_Renderer *renderer, gfx_image *img, char* path);

/**
 * @brief Initialize a folder of images and add to image list.
 * @param renderer Pointer to active renderer.
 * @param imgl Pointer to image list to add images to.
 * @param folder Full path to folder.
 * @return GRAPHICS_OK if passed, other graphicsReturnCode if failed.
 * @see gfx_image_destroy
 * @see graphicsReturnCode
 *
 * ID for images will be file name.
 */
enum graphicsReturnCode gfx_populate_list_folder(SDL_Renderer *renderer, gfx_image_list *imgl, char *folder);

/**
 * @brief Destroy a list of images.
 * @param imgl Pointer to image list to destroy.
 * @see gfx_populate_list_folder
 *
 */
void gfx_destroy_list(gfx_image_list *imgl);

/**
 * @brief Get image from image list.
 * @param imgl Pointer to image list.
 * @param id Unique identifier for image object.
 * @return Pointer to image, NULL if failed.
 *
 */
gfx_image * gfx_image_get(gfx_image_list *imgl, char *id);

/**
 * @brief Set text content of text object.
 * @param renderer Pointer to active renderer.
 * @param text Pointer to text object.
 * @param new_txt New text to be used in text object.
 * @return GRAPHICS_OK if passed, other graphicsReturnCode if failed
 * @see graphicsReturnCode
 *
 */
enum graphicsReturnCode gfx_text_set(SDL_Renderer *renderer, gfx_text *text, char *new_txt);

/**
 * @brief Draw image to screen according to coordinates.
 * @param renderer Pointer to active renderer.
 * @param s_x Start x coordinate of line.
 * @param s_y Start y coordinate of line.
 * @param e_x End x coordinate of line.
 * @param e_y End y coordinate of line.
 * @return GRAPHICS_OK if passed, other graphicsReturnCode if failed
 * @see graphicsReturnCode
 *
 */
enum graphicsReturnCode gfx_line_draw(SDL_Renderer *renderer, int s_x, int s_y, int e_x, int e_y);

/**
 * @brief Draw image to screen according to coordinates.
 * @param renderer Pointer to active renderer.
 * @param image Pointer to image.
 * @param x Coordinate X.
 * @param y Coordinate Y.
 * @param angle Rotation angle, 0 to disable.
 * @return GRAPHICS_OK if passed, other graphicsReturnCode if failed
 *
 */
enum graphicsReturnCode gfx_surface_draw(SDL_Renderer *renderer, SDL_Texture *image, int x, int y, double angle);

#endif
