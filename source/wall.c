// ***************************************************************************
// Vectrex C programming file template
// ***************************************************************************

#include <vectrex.h>
#include "generic.h"
#include "draw.h"
#include "wall.h"

// ---------------------------------------------------------------------------

extern const signed char* const web_walls[];
extern const signed char* const web_wall_coords[];

struct object *wall_list = 0;
struct object *wall_free_list = 0;

void init_wall(
	struct wall *wall,
	unsigned int index
	)
{
	signed int y1, x1, y2, x2, temp;
	unsigned int wall_id;
	const signed char *web_wall;

	wall->index = index;

	if (index < WALL_SOLID_INDEX_START)
	{
		wall_id		= index;
		wall->type	= WALL_TYPE_DASHED;
	}
	else
	{
		wall_id		= index - WALL_SOLID_INDEX_START;
		wall->type	= WALL_TYPE_SOLID;
	}

	web_wall	= web_walls[wall_id];

	y1 = web_wall[0];
	x1 = web_wall[1];

	y2 = y1 + web_wall[3];
	x2 = x1 + web_wall[4];

	if (y1 > y2)
	{
		temp = y1;
		y1 = y2;
		y2 = temp; 
	}

	if (x1 > x2)
	{
		temp = x1;
		x1 = x2;
		x2 = temp; 
	}

	take_object(&wall->obj, &wall_free_list);
	init_object(&wall->obj, y1, x1, y2 - y1, x2 - x1, OBJECT_MOVE_SCALE, 0, &wall_list);

	wall->y2 = y2;
	wall->x2 = x2;

	wall->coords =	web_wall_coords[wall_id];
	wall->pos_vlist =	web_wall;
}

void deinit_wall(
	struct wall *wall
	)
{
	deinit_object(&wall->obj, &wall_list);
	give_object(&wall->obj, &wall_free_list);
}

unsigned int check_point_on_wall(
	struct wall *wall,
	signed int y,
	signed int x
	)
{
	const signed char *coord;
	signed int y1, x1, y2, x2;
	unsigned int result = 0;

	y1 = wall->obj.y;
	x1 = wall->obj.x;

	y2 = wall->y2;
	x2 = wall->x2;

	if (y1 == y2)
	{
		if (y >= y1 - WALL_CHECK_DELTA && y <= y1 + WALL_CHECK_DELTA)
		{
			if (x >= x1 && x <= x2)
			{
				result = 1;
			}
		} 
	}
	else if (x1 == x2)
	{
		if (x >= x1 - WALL_CHECK_DELTA && x <= x1 + WALL_CHECK_DELTA)
		{
			if (y >= y1 && y <= y2)
			{
				result = 1;
			}
		}
	}
	else// if (y > y1 && y < y2 && x > x1 && x < x2)
	{
		for (coord = wall->coords; coord[0] != 127; coord += 2)
		{
			if (y > coord[0] - WALL_CHECK_DELTA && y < coord[0] + WALL_CHECK_DELTA &&
				x > coord[1] - WALL_CHECK_DELTA && x < coord[1] + WALL_CHECK_DELTA)
			{
				result = 1;
				break;
			}
		}
	}

	return result;
}

void draw_walls(void)
{
	struct wall *wall;

	wall = (struct wall *) wall_list;
	while (wall != 0)
	{
		reset0ref();
		moveto(wall->pos_vlist[0], wall->pos_vlist[1]);
		if (wall->type == WALL_TYPE_DASHED)
		{
			// $aa = 10101010 pattern
			// $cc = 11001100 pattern
			// $f0 = 11110000 pattern
			draw_vlist_c_pattern(&wall->pos_vlist[2], (signed char) 0xf0);
		}
		else
		{
			draw_vlist_c(&wall->pos_vlist[2]);
		}
		wall = (struct wall *) wall->obj.next;
	}
}

// ***************************************************************************
// end of file
// ***************************************************************************
