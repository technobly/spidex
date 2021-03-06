// ***************************************************************************
// Vectrex C programming file template
// ***************************************************************************

#include <vectrex.h>
#include <macro.h>
#include "generic.h"
#include "draw.h"
#include "player.h"
#include "projectile.h"
#include "enemy.h"

// ---------------------------------------------------------------------------

extern const signed char cicle[];
extern const signed char* const spiral[];
extern const signed char* const egg[];
extern const signed char* const spike[];
extern const signed char* const mosquito[];
extern const signed char* const fly[];
extern const signed char* const butterfly[];
extern const signed char* const bug[];
extern const signed char* const bee[];
extern const signed char* const mine[];
extern const signed char* const dragonfly[];
extern const signed char* const ant[];
extern const signed char* const swallow[];
extern const signed char* const spikey[];
extern const signed char* const stump[];

const struct enemy_race enemy_races[] =
{
	/*	h	w	scale	type					speed	max_hits	special				treshold	shapes	*/
	{	4,	4,	0x40/10,	ENEMY_TYPE_RANDOM,		2,		1,		ENEMY_SPECIAL_NONE,	1,		mosquito	},
	{	7,	7,	0x40/10,	ENEMY_TYPE_PATH,		1,		1,		ENEMY_SPECIAL_NONE,	1,		fly		},
	{	10,	10,	0x40/10,	ENEMY_TYPE_RANDOM,		2,		2,		ENEMY_SPECIAL_NONE,	4,		butterfly	},
	{	10,	10,	0x40/10,	ENEMY_TYPE_PATH,		2,		5,		ENEMY_SPECIAL_NONE,	2,		bee		},
	{	12,	12,	0x40/10,	ENEMY_TYPE_HOMING,		1,		-1,		ENEMY_SPECIAL_EGG,		3,		bug		},
	{	7,	7,	8,		ENEMY_TYPE_PATH,		2,		1,		ENEMY_SPECIAL_EXPLODE,	2,		mine		},
	{	12,	12,	0x40/10,	ENEMY_TYPE_RANDOM,		2,		12,		ENEMY_SPECIAL_NONE,	2,		dragonfly	},
	{	24,	24,	0x80/10,	ENEMY_TYPE_RANDOM,		4,		25,		ENEMY_SPECIAL_NONE,	2,		dragonfly	},
	{	14,	14,	0x40/10,	ENEMY_TYPE_HOMING,		1,		10,		ENEMY_SPECIAL_NONE,	2,		ant		},
	{	10,	10,	0x30/10,	ENEMY_TYPE_PATH,		2,		7,		ENEMY_SPECIAL_NONE,	2,		ant		},
	{	10,	10,	0x40/10,	ENEMY_TYPE_PATH,		3,		2,		ENEMY_SPECIAL_NONE,	10,		swallow	},
	{	12,	12,	0x40/10,	ENEMY_TYPE_RANDOM,		3,		1,		ENEMY_SPECIAL_EXPLODE,	4,		spikey	},
	{	40,	40,	0x80/10,	ENEMY_TYPE_PATH,		1,		127,		ENEMY_SPECIAL_HEAVY,	10,		stump	}
};

unsigned int enemy_status = 0;
struct object *enemy_list = 0;
struct object *enemy_free_list = 0;

__INLINE void set_random_dir_enemy(
	struct enemy *enemy
	)
{
	unsigned int rnd = random();

	if (rnd < ENEMY_RANDOM_RANGE_DOWN)
	{
		set_dir_character(&enemy->ch, DIR_DOWN);
	}
	else if (rnd < ENEMY_RANDOM_RANGE_DOWN_RIGHT)
	{
		set_dir_character(&enemy->ch, DIR_DOWN_RIGHT);
	}
	else if (rnd < ENEMY_RANDOM_RANGE_RIGHT)
	{
		set_dir_character(&enemy->ch, DIR_RIGHT);
	}
	else if (rnd < ENEMY_RANDOM_RANGE_UP_RIGHT)
	{
		set_dir_character(&enemy->ch, DIR_UP_RIGHT);
	}
	else if (rnd < ENEMY_RANDOM_RANGE_UP)
	{
		set_dir_character(&enemy->ch, DIR_UP);
	}
	else if (rnd < ENEMY_RANDOM_RANGE_UP_LEFT)
	{
		set_dir_character(&enemy->ch, DIR_UP_LEFT);
	}
	else if (rnd < ENEMY_RANDOM_RANGE_LEFT)
	{
		set_dir_character(&enemy->ch, DIR_LEFT);
	}
	else if (rnd < ENEMY_RANDOM_RANGE_DOWN_LEFT)
	{
		set_dir_character(&enemy->ch, DIR_DOWN_LEFT);
	}
}

__INLINE void set_follow_dir_enemy(
	struct enemy *enemy
	)
{
	signed int src_y = enemy->ch.obj.y;
	signed int src_x = enemy->ch.obj.x;
	signed int dest_y = player_1.ch.obj.y;
	signed int dest_x = player_1.ch.obj.x;

	if (src_y > dest_y && src_x == dest_x)
	{
		set_dir_character(&enemy->ch, DIR_DOWN);
	}
	else if (src_y > dest_y && src_x < dest_x)
	{
		set_dir_character(&enemy->ch, DIR_DOWN_RIGHT);
	}
	else if (src_y == dest_y && src_x < dest_x)
	{
		set_dir_character(&enemy->ch, DIR_RIGHT);
	}
	else if (src_y < dest_y && src_x < dest_x)
	{
		set_dir_character(&enemy->ch, DIR_UP_RIGHT);
	}
	else if (src_y < dest_y && src_x == dest_x)
	{
		set_dir_character(&enemy->ch, DIR_UP);
	}
	else if (src_y < dest_y && src_x > dest_x)
	{
		set_dir_character(&enemy->ch, DIR_UP_LEFT);
	}
	else if (src_y == dest_y && src_x > dest_x)
	{
		set_dir_character(&enemy->ch, DIR_LEFT);
	}
	else if (src_y > dest_y && src_x > dest_x)
	{
		set_dir_character(&enemy->ch, DIR_DOWN_LEFT);
	}
}

void init_enemy(
	struct enemy *enemy,
	signed int y,
	signed int x,
	const struct enemy_race *race, 
	unsigned int num_steps,
	const struct enemy_path *path,
	signed int param
	)
{
	take_object(&enemy->ch.obj, &enemy_free_list);
	init_character(
		&enemy->ch,
		y,
		x,
		race->h,
		race->w,
		race->scale,
		CHARACTER_WALL_PASS_OUT,
		race->speed,
		race->treshold,
		2,
		race->shapes,
		&enemy_list
		);

	enemy->race			= race;
	enemy->num_hits		= race->max_hits;
	enemy->path_counter	= 0;
	enemy->spawn_counter	= 0;
	enemy->step_counter	= 0;
	enemy->num_steps		= num_steps;
	enemy->path			= path;
	enemy->param			= param;

	if (enemy->race->type == ENEMY_TYPE_RANDOM)
	{
		set_random_dir_enemy(enemy);
	}
	else if (enemy->race->type == ENEMY_TYPE_PATH)
	{
		set_dir_character(&enemy->ch, enemy->path[0].dir);
	}
	else
	{
		set_dir_character(&enemy->ch, DIR_DOWN);
	}

	if (enemy->race->special == ENEMY_SPECIAL_EGG)
	{
		enemy->state = ENEMY_STATE_EGG;
	}
	else
	{
		enemy->state = ENEMY_STATE_SPAWN;
	}
}

void deinit_enemy(
	struct enemy *enemy
	)
{
	deinit_object(&enemy->ch.obj, &enemy_list);
	give_object(&enemy->ch.obj, &enemy_free_list);
}

void move_enemies(void)
{
	struct enemy *enemy;
	struct enemy *other;
	struct wall *wall;
	unsigned int proj_left;
	struct projectile *proj;
	struct enemy *rem_enemy = 0;
	unsigned int hit_wall = 0;

	enemy_status = 0;

	enemy = (struct enemy *) enemy_list;
	while(enemy != 0)
	{
		if (enemy->state == ENEMY_STATE_MOVE)
		{
			switch (enemy->race->type)
			{
				case ENEMY_TYPE_RANDOM:
					animate_character(&enemy->ch);

					if (++enemy->path_counter >= (unsigned int) enemy->param)
					{
						enemy->path_counter = 0;
						set_random_dir_enemy(enemy);
					}

					if (enemy->race->special == ENEMY_SPECIAL_EXPLODE)
					{
						wall = (struct wall *) wall_list;
						while (wall != 0)
						{
							if (quick_check_wall_character(&enemy->ch, wall))
							{
								hit_wall = hit_wall_character(&enemy->ch, wall);
								if (hit_wall)
								{
									break;
								}
							}
							wall = (struct wall *) wall->obj.next;
						}

						if (!hit_wall)
						{
							if (move_character(&enemy->ch))
							{
								set_random_dir_enemy(enemy);
							}
						}
						else
						{
							enemy->state = ENEMY_STATE_EXPLODE;
							enemy->state_counter = 0;
							enemy_status |= ENEMY_STATUS_EXPLODE;
						}
					}
					else
					{
						if (move_character(&enemy->ch))
						{
							set_random_dir_enemy(enemy);
						}
					}
					break;

				case ENEMY_TYPE_PATH:
					animate_character(&enemy->ch);

					if (++enemy->path_counter >= enemy->path[enemy->step_counter].treshold)
					{
						enemy->path_counter = 0;
						if (++enemy->step_counter >= enemy->num_steps)
						{
							enemy->step_counter = 0;
						}

						if (enemy->path[enemy->step_counter].action == ENEMY_ACTION_MOVE)
						{
							set_dir_character(&enemy->ch, enemy->path[enemy->step_counter].dir);
						}
						else if (enemy->path[enemy->step_counter].action == ENEMY_ACTION_SHOOT)
						{
							if (projectile_free_list != 0)
							{
								init_projectile(
									(struct projectile *) projectile_free_list,
									&enemy->ch.obj,
									enemy->ch.obj.y,
									enemy->ch.obj.x,
									ENEMY_PROJECTILE_HEIGHT,
									ENEMY_PROJECTILE_WIDTH,
									enemy->path[enemy->step_counter].dir,
									enemy->param,
									ENEMY_PROJECTILE_SCALE/10,
									spike
									);
								enemy_status |= ENEMY_STATUS_PROJECTILE;
							}
						}
					}

					if (enemy->race->special == ENEMY_SPECIAL_EXPLODE)
					{
						wall = (struct wall *) wall_list;
						while (wall != 0)
						{
							if (quick_check_wall_character(&enemy->ch, wall))
							{
								hit_wall = hit_wall_character(&enemy->ch, wall);
								if (hit_wall)
								{
									break;
								}
							}
							wall = (struct wall *) wall->obj.next;
						}

						if (!hit_wall)
						{
							if (move_character(&enemy->ch))
							{
								if (++enemy->step_counter >= enemy->num_steps)
								{
									enemy->step_counter = 0;
								}
								set_dir_character(&enemy->ch, enemy->path[enemy->step_counter].dir);
							}
						}
						else
						{
							enemy->state = ENEMY_STATE_EXPLODE;
							enemy->state_counter = 0;
							enemy_status |= ENEMY_STATUS_EXPLODE;
						}
					}
					else
					{
						move_character(&enemy->ch);
					}
					break;

				case ENEMY_TYPE_HOMING:
					if (player_1.state == PLAYER_STATE_NORMAL ||
						player_1.state == PLAYER_STATE_INVINSIBLE)
					{
						if (enemy->step_counter < DIR_NONE)
						{
							set_follow_dir_enemy(enemy);
						}
						else if (++enemy->path_counter >= (unsigned int) enemy->param)
						{
							enemy->path_counter = 0;
							enemy->step_counter = DIR_DOWN;
						}

						wall = (struct wall *) wall_list;
						while (wall != 0)
						{
							if (quick_check_wall_character(&enemy->ch, wall))
							{
								hit_wall = hit_wall_character(&enemy->ch, wall);
								if (hit_wall)
								{
									break;
								}
							}
							wall = (struct wall *) wall->obj.next;
						}

						if (!hit_wall)
						{
							animate_character(&enemy->ch);
							move_character(&enemy->ch);
						}
						else
						{
							if (enemy->race->special == ENEMY_SPECIAL_EXPLODE)
							{
								enemy->state = ENEMY_STATE_EXPLODE;
								enemy->state_counter = 0;
								enemy_status |= ENEMY_STATUS_EXPLODE;
							}
							else
							{
								enemy->step_counter = DIR_NONE;
								set_random_dir_enemy(enemy);
							}
						}
					}
					else
					{
						enemy->state = ENEMY_STATE_STOP;
						enemy->state_counter = 0;
					}
					break;						

				default:
					break;
			}
		}
		else if (enemy->state == ENEMY_STATE_SPAWN)
		{
			if (++enemy->ch.counter >= ENEMY_SPAWN_ANIM_TRESHOLD)
			{
				enemy->ch.counter = 0;
				if (++enemy->ch.frame >= ENEMY_SPAWN_ANIM_FRAMES)
				{
					enemy->ch.frame = 0;
				}
			}

			if (++enemy->state_counter >= ENEMY_SPAWN_TRESHOLD)
			{
				enemy->ch.counter = 0;
				enemy->ch.frame = 0;
				enemy->state = ENEMY_STATE_MOVE;
				enemy->state_counter = 0;
			}
		}
		else if (enemy->state == ENEMY_STATE_STOP)
		{
			if (++enemy->state_counter >= ENEMY_STOP_TRESHOLD)
			{
				enemy->state = ENEMY_STATE_MOVE;
				enemy->state_counter = 0;
			}
		}
		else if (enemy->state == ENEMY_STATE_EGG || enemy->state == ENEMY_STATE_HATCH)
		{
			if (enemy->state == ENEMY_STATE_EGG)
			{
				if (++enemy->state_counter >= ENEMY_EGG_TRESHOLD)
				{
					enemy->state = ENEMY_STATE_HATCH;
					enemy->state_counter = 0;
				}
			}
			else if (enemy->state == ENEMY_STATE_HATCH)
			{
				if (++enemy->state_counter >= ENEMY_HATCH_TRESHOLD)
				{
					enemy->state = ENEMY_STATE_MOVE;
					enemy->state_counter = 0;
					
				}
			}
		}
		else if (enemy->state == ENEMY_STATE_EXPLODE)
		{
			other = (struct enemy *) enemy_list;
			while (other)
			{
				if (other != enemy)
				{
					if (other->num_hits > 0 &&
						explosion_hit_object_enemy(enemy, &other->ch.obj))
					{
						// TODO: Other explode enemies shall explode instead of dissappearing
						other->num_hits = 0;
						other->state = ENEMY_STATE_DYING;
						other->state_counter = 0;
					}
				}
				other = (struct enemy *) other->ch.obj.next;
			}

			if (++enemy->state_counter >= ENEMY_EXPLODE_TRESHOLD)
			{
				enemy->state = ENEMY_STATE_DYING;
				enemy->state_counter = 0;
				rem_enemy = enemy;
			}
		}
		else if (enemy->state == ENEMY_STATE_DYING)
		{
			if (projectile_list != 0)
			{
				proj_left = 0;
				proj = (struct projectile *) projectile_list;
				while (proj != 0)
				{
					if (proj->owner == &enemy->ch.obj)
					{
						proj_left = 1;
						break;
					}

					proj = (struct projectile *) proj->obj.next;
				}

				if (!proj_left)
				{
					rem_enemy = enemy;
				}
			}
			else
			{
				rem_enemy = enemy;
			}
		}
		enemy = (struct enemy *) enemy->ch.obj.next;

		if (rem_enemy != 0)
		{
			rem_enemy->state = ENEMY_STATE_DEAD;
			rem_enemy->state_counter = 0;
			deinit_enemy(rem_enemy);
			rem_enemy = 0;
		}
	}
}

unsigned int hit_enemy(
	struct enemy *enemy
	)
{
	unsigned int result = 0;

	if (enemy->num_hits > 0)
	{
		if (--enemy->num_hits == 0)
		{
			if (enemy->race->special == ENEMY_SPECIAL_EXPLODE)
			{
				enemy->state = ENEMY_STATE_EXPLODE;
				enemy->state_counter = 0;
				enemy_status |= ENEMY_STATUS_EXPLODE;
			}
			else
			{
				enemy->state = ENEMY_STATE_DYING;
				enemy->state_counter = 0;
				result = 1;
			}
		}
	}

	if (enemy->state != ENEMY_STATE_EXPLODE && enemy->state != ENEMY_STATE_DYING)
	{
		if (enemy->race->special != ENEMY_SPECIAL_HEAVY)
		{
			retreat_character(&enemy->ch);
			enemy->state = ENEMY_STATE_STOP;
			enemy->state_counter = 0;
		}
		enemy_status |= ENEMY_STATUS_HIT;
	}

	return result;
}

unsigned int explosion_hit_object_enemy(
	struct enemy *enemy,
	struct object *obj
	)
{
	signed int dy, dx;
	unsigned int r;
	unsigned int result = 0;

	dy = enemy->ch.obj.y - obj->y;
	dx = enemy->ch.obj.x - obj->x;
	r = ENEMY_EXPLOSION_RADIUS + (enemy->state_counter << 2) + (enemy->state_counter << 1);
	if ((unsigned int) abs(dy) + (unsigned int) abs(dx) < r)
	{
		result = 1;
	}

	return result;
}

void draw_enemies(void)
{
	struct enemy *enemy;

	enemy = (struct enemy *) enemy_list;
	while (enemy != 0)
	{
		if (enemy->state == ENEMY_STATE_MOVE || enemy->state == ENEMY_STATE_STOP)
		{
			draw_synced_list_c(
				enemy->ch.shapes[enemy->ch.base_frame + enemy->ch.frame],
				enemy->ch.obj.y,
				enemy->ch.obj.x,
				OBJECT_MOVE_SCALE,
				enemy->ch.obj.scale
				);
		}
		else if (enemy->state == ENEMY_STATE_SPAWN)
		{
			draw_synced_list_c(
				spiral[enemy->ch.frame],
				enemy->ch.obj.y,
				enemy->ch.obj.x,
				OBJECT_MOVE_SCALE,
				0x01 + (enemy->state_counter >> 2)
				);
		}
		else if (enemy->state == ENEMY_STATE_EGG)
		{
			draw_synced_list_c(
				egg[0],
				enemy->ch.obj.y,
				enemy->ch.obj.x,
				OBJECT_MOVE_SCALE,
				0x40/10
				);
		}
		else if (enemy->state == ENEMY_STATE_HATCH)
		{
			draw_synced_list_c(
				egg[1],
				enemy->ch.obj.y,
				enemy->ch.obj.x,
				OBJECT_MOVE_SCALE,
				0x40/10
				);
		}
		else if (enemy->state == ENEMY_STATE_EXPLODE)
		{
			draw_synced_list_c(
				cicle,
				enemy->ch.obj.y,
				enemy->ch.obj.x,
				OBJECT_MOVE_SCALE,
				0x40/10 + (enemy->state_counter << 2)
				);
		}
		enemy = (struct enemy *) enemy->ch.obj.next;
	}
}

// ***************************************************************************
// end of file
// ***************************************************************************
