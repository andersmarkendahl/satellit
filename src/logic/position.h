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
 * @brief Position handling
 */

#ifndef POSITION_H
#define POSITION_H
#include "common.h"

/**
 * @brief Structure for position and velocity.
 */
typedef struct position {
	double x;	/**< Position pixel in X. */
	double y;	/**< Position pixel in Y. */
	double vx;	/**< Velocity in X. */
	double vy;	/**< Velocity in Y. */
} position;

/**
 * @brief Return codes for position handling.
 */
enum positionReturnCode {
    POSITION_OK = 0,	/**< Position OK. */
    POSITION_ERR_OOB,	/**< Position out of bounds. */
    POSITION_ERR_VEL,	/**< Faulty Velocity. */
};

/** Runtime space boundaries — set at startup based on actual screen resolution. */
extern int space_w_min, space_w_max, space_h_min, space_h_max;

/**
 * @brief Initalize and allocate position.
 * @param x Initial position in X.
 * @param y Initial position in Y.
 * @param vx Initial velocity in X.
 * @param vy Initial velocity in Y.
 * @return Pointer to created position, NULL if failed.
 *
 */
position * position_init(double x, double y, double vx, double vy);

/**
 * @brief Destroy and free position.
 * @param p Pointer to position to destroy.
 * @return 0 if passed, non-zero if failed.
 *
 */
int position_destroy(position *p);

/**
 * @brief Validate position towards boundaries of game.
 * @param x Position in X.
 * @param y Position in Y.
 * @return POSITION_OK if pass, POSITION_ERR_OOB if out if bounds.
 *
 */
enum positionReturnCode position_validate(double x, double y);

/**
 * @brief Update position of obj based on gravity from objl.
 * @param p Pointer to position to be updated.
 * @return return POSITION_OK if passed, POSITION_ERR_OOB if failed
 * @see positionReturnCode
 *
 */
enum positionReturnCode position_update(position *p);

/**
 * @brief Return the velocity angle of object.
 * @param p Pointer to position struct.
 * @return return angle of velocity vector
 *
 */
double position_get_angle(position *p);

#endif
