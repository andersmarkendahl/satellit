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

#include "object.h"

const char* object_enum2str(enum objectReturnCode ret) {
	switch (ret) {
		case OBJECT_OK:		return "OBJECT_OK";
		case OBJECT_ADD:	return "OBJECT_ADD";
		case OBJECT_REM:	return "OBJECT_REM";
		case OBJECT_OOB:	return "OBJECT_OOB";
		case OBJECT_COL:	return "OBJECT_COL";
		case OBJECT_NFD:	return "OBJECT_NFD";
		default:			return "NULL";
	}
}

object_list * object_list_init(void) {
	object_list *objl;
	objl = calloc(1, sizeof(object_list));
	objl->n_objs = 0;
	objl->head = NULL;
	return objl;
}

int object_list_destroy(object_list *objl) {
	free(objl);
	return 0;
}

object * object_init(int id, SDL_Texture *image,
	double x, double y, double m, double vx, double vy) {
	object *obj;
	obj = calloc(1, sizeof(object));
	obj->id = id;
	obj->dead = 0;
	obj->mass = m;
	obj->next = NULL;
	obj->prev = NULL;

	obj->pos = position_init(x, y, vx, vy);
	if (obj->pos == NULL) {
		free(obj);
		return NULL;
	}

	obj->image = image;

	return obj;
}

int object_destroy(object *obj) {

	if(obj == NULL) {
		return 0;
	}

	position_destroy(obj->pos);
	free(obj);
	return 0;
}

enum objectReturnCode object_list_add(object_list *objl, object *obj) {

	if (objl == NULL || obj == NULL) {
		return OBJECT_ADD;
	}
	DL_APPEND(objl->head, obj);
	objl->n_objs++;
	return OBJECT_OK;
}

enum objectReturnCode object_list_remove(object_list *objl, object *obj) {

	if (objl == NULL || obj == NULL) {
		return OBJECT_REM;
	}
	DL_DELETE(objl->head,obj);
	objl->n_objs--;
	return OBJECT_OK;
}

enum objectReturnCode object_list_clean_all(object_list *objl) {
	object *obj, *tmp;

	if(objl == NULL) {
		return OBJECT_OK;
	}

	DL_FOREACH_SAFE(objl->head,obj,tmp) {
		if(object_list_remove(objl, obj) != OBJECT_OK) {
			return OBJECT_REM;
		}
		if(object_destroy(obj) != OBJECT_OK) {
			return OBJECT_REM;
		}

    }
	return OBJECT_OK;
}

