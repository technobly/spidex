#ifndef _FOOD_H
#define _FOOD_H

#include "object.h"

#define FOOD_SCALE				0x40
#define FOOD_MAX_SCALE_INCREASE	32

#define FOOD_HEIGHT	8
#define FOOD_WIDTH		8
#define FOOD_TRESHOLD	255

struct food
{
	struct object obj;
	unsigned int counter;
	signed int value;
};

void init_food(
	struct food *food,
	signed int y,
	signed int x,
	signed int value
	);

void deinit_food(
	struct food *food
	);

void move_food(void);
void draw_food(void);

extern struct object *food_list;
extern struct object *food_free_list;

#endif
