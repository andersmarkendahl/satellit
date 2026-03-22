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

#include "position.h"

int space_w_min = SPACE_W_MIN;
int space_w_max = SPACE_W_MAX;
int space_h_min = SPACE_H_MIN;
int space_h_max = SPACE_H_MAX;

enum positionReturnCode position_validate(double x, double y) {
	if ((x >= space_w_min && x <= space_w_max) && (y >= space_h_min && y <= space_h_max)) {
		return POSITION_OK;
	}
	return POSITION_ERR_OOB;
}

position * position_init(double x, double y, double vx, double vy) {
	position *p;
	if (position_validate(x, y) != 0) {
		return NULL;
	}
	p = calloc(1, sizeof(position));
	p->x = x;
	p->y = y;
	p->vx = vx;
	p->vy = vy;
	return p;
}

int position_destroy(position *p) {
	free(p);
	return 0;
}

enum positionReturnCode position_update(position *p) {

	p->x = p->x + p->vx/UPDATE_FREQ;
	p->y = p->y + p->vy/UPDATE_FREQ;

	if (position_validate(p->x, p->y) != POSITION_OK) {
		return POSITION_ERR_OOB;
	}

	return POSITION_OK;

}

double position_get_angle(position *p) {

	return atan2(p->vy, p->vx)*(180/M_PI);

}
