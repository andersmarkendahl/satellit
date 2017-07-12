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

object_list * object_list_init() {
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
		LOG("ERR: Unable to init position %f,%f", x, y);
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

enum objectReturnCode object_add(object_list *objl, int id, SDL_Texture *image,
	double x, double y, double m, double vx, double vy) {

	object *obj;
	obj = object_init(id, image, x, y, m, vx, vy);
	if (obj == NULL) {
		LOG("WARN: Unable to init object number");
		return OBJECT_ADD;
	}
	DL_APPEND(objl->head, obj);
	objl->n_objs++;
	return OBJECT_OK;
}

enum objectReturnCode object_remove(object_list *objl, object *obj) {
	DL_DELETE(objl->head,obj);
	if(object_destroy(obj) != OBJECT_OK) {
		return OBJECT_REM;
	}
	objl->n_objs--;
	return OBJECT_OK;
}

enum objectReturnCode object_remove_mult(object_list *objl) {
	object *obj, *tmp;

	if(objl == NULL) {
		return OBJECT_OK;
	}

	DL_FOREACH_SAFE(objl->head,obj,tmp) {
		if(object_remove(objl, obj) != OBJECT_OK) {
			return OBJECT_REM;
		}
    }
	return OBJECT_OK;
}

enum objectReturnCode object_position_update(struct object_list *objl, struct object *obj) {

	position *p = obj->pos;
	p->x = p->x + p->vx/UPDATE_FREQ;
	p->y = p->y + p->vy/UPDATE_FREQ;
	if (position_validate(p->x, p->y) != 0) {
		return OBJECT_OOB;
	}

	/* Update velocity for next timestep */
	velocity_update(objl, obj);
	if (velocity_validate(p->vx, p->vy) != 0) {
		return OBJECT_OOB;
	}
	return OBJECT_OK;
}

double object_get_angle(object *obj) {

	double ret;
	ret = atan(obj->pos->vy/obj->pos->vx)*(180/M_PI);
	return ret;

}
