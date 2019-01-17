// ***************************************************************************
// Vectrex C programming file template
// ***************************************************************************

#include <vectrex.h>
#include "object.h"
#include "controller.h"
#include "input.h"

// ---------------------------------------------------------------------------

void init_single_input(void)
{
	enable_controller_1_x();
	enable_controller_1_y();
	disable_controller_2_x();
	disable_controller_2_y();
}

void init_dual_input(void)
{
	enable_controller_1_x();
	enable_controller_1_y();
	enable_controller_2_x();
	enable_controller_2_y();
}

unsigned int get_dir_input_1(
	unsigned int *dir
	)
{
	unsigned int trigger = 0;

	if (Vec_Joy_1_Y > INPUT_JOY_SENSE)
	{
		if (Vec_Joy_1_X > INPUT_JOY_SENSE)
		{
			*dir = DIR_UP_RIGHT;
			trigger = 1;
		}
		else if (Vec_Joy_1_X < -INPUT_JOY_SENSE)
		{
			*dir = DIR_UP_LEFT;
			trigger = 1;
		}
		else
		{
			*dir = DIR_UP;
			trigger = 1;
		}
	}
	else if (Vec_Joy_1_Y < -INPUT_JOY_SENSE)
	{
		if (Vec_Joy_1_X > INPUT_JOY_SENSE)
		{
			*dir =  DIR_DOWN_RIGHT;
			trigger = 1;
		}
		else if (Vec_Joy_1_X < -INPUT_JOY_SENSE)
		{
			*dir = DIR_DOWN_LEFT;
			trigger = 1;
		}
		else
		{
			*dir = DIR_DOWN;
			trigger = 1;
		}
	}
	else
	{
		if (Vec_Joy_1_X > INPUT_JOY_SENSE)
		{
			*dir = DIR_RIGHT;
			trigger = 1;
		}
		else if (Vec_Joy_1_X < -INPUT_JOY_SENSE)
		{
			*dir = DIR_LEFT;
			trigger = 1;
		}
	}

	return trigger;
}

unsigned int get_dir_input_2(
	unsigned int *dir
	)
{
	unsigned int trigger = 0;

	if (Vec_Joy_2_Y > INPUT_JOY_SENSE)
	{
		if (Vec_Joy_2_X > INPUT_JOY_SENSE)
		{
			*dir = DIR_UP_RIGHT;
			trigger = 1;
		}
		else if (Vec_Joy_2_X < -INPUT_JOY_SENSE)
		{
			*dir = DIR_UP_LEFT;
			trigger = 1;
		}
		else
		{
			*dir = DIR_UP;
			trigger = 1;
		}
	}
	else if (Vec_Joy_2_Y < -INPUT_JOY_SENSE)
	{
		if (Vec_Joy_2_X > INPUT_JOY_SENSE)
		{
			*dir =  DIR_DOWN_RIGHT;
			trigger = 1;
		}
		else if (Vec_Joy_2_X < -INPUT_JOY_SENSE)
		{
			*dir = DIR_DOWN_LEFT;
			trigger = 1;
		}
		else
		{
			*dir = DIR_DOWN;
			trigger = 1;
		}
	}
	else
	{
		if (Vec_Joy_2_X > INPUT_JOY_SENSE)
		{
			*dir = DIR_RIGHT;
			trigger = 1;
		}
		else if (Vec_Joy_2_X < -INPUT_JOY_SENSE)
		{
			*dir = DIR_LEFT;
			trigger = 1;
		}
	}

	return trigger;
}

// ***************************************************************************
// end of file
// ***************************************************************************
