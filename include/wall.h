#ifndef _WALL_H
#define _WALL_H

#define WALL_CHECK_DELTA	3

struct wall
{
	unsigned int active;
	unsigned int index;

	signed int y1, x1;
	signed int y2, x2;

	const signed char *coords;
 
	signed int pos[2];
	signed int vec[3];
};

void init_wall(
	struct wall *wall,
	unsigned int index
	);

void deinit_wall(
	struct wall *wall
	);

unsigned int check_point_on_wall(
	struct wall *wall,
	signed int y,
	signed int x
	);

void move_wall(
	struct wall *wall
	);

void draw_wall(
	struct wall *wall
	);

#endif