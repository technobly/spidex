// ***************************************************************************
// main
// ***************************************************************************

#include <vectrex.h>
#include "generic.h"
#include "controller.h"
#include "input.h"
#include "draw.h"
#include "DrawWebToC.h"
#include "player.h"
#include "bullet.h"
#include "enemy.h"
#include "projectile.h"
#include "food.h"
#include "wall.h"
#include "wave.h"
#include "draw.h"
#include "game.h"
#include "text.h"
#include "ayfxPlayer.h"
#include "fire_snd.h"
#include "fall_snd.h"

extern const signed char web[];
extern const signed char web1[];
extern const signed char web2[];
extern const signed char web3[];
extern const signed char web4[];
extern const signed char web5[];

static unsigned int fire_status = 0;
static unsigned int new_wave_index = 0;
static struct enemy *slain_enemy = 0;

#ifdef PLAYER_GO_HOME
static unsigned int player_home;
#endif

// ---------------------------------------------------------------------------
// cold reset: the vectrex logo is shown, all ram data is cleared
// warm reset: skip vectrex logo and keep ram data
// ---------------------------------------------------------------------------
// at system startup, when powering up the vectrex, a cold reset is performed
// if the reset button is pressed, then a warm reset is performed
// ---------------------------------------------------------------------------
// after each reset, the cartridge title is shown and then main() is called
// ---------------------------------------------------------------------------

int main(void)
{
	init_single_input();
	init_random(5, 27, 3, 19);
	init_game();
////DEBUG
	//game_wave.wave_index = 11;
////END DEBUG
	while(1)
	{
		check_joysticks();
		check_buttons();

		if (game_state == GAME_STATE_PAUSE)
		{
			if (button_1_1_pressed())
			{
				game_state = GAME_STATE_NORMAL;
			}

			if (button_1_4_pressed())
			{
				toggle_control_method_game();
			}
		}
		else if (game_state == GAME_STATE_WAVE_DONE || game_state == GAME_STATE_FINNISH)
		{
#ifdef PLAYER_GO_HOME
			if (!player_home)
			{
				player_home = goto_player_1(GAME_PLAYER_1_START_Y, GAME_PLAYER_1_START_X);
			}

			if (player_home)
			{
				set_fire_dir_player(&player_1, DIR_UP);
			}
#endif

			move_bullets();
			move_projectiles();
		}
		else
		{
			if (game_options & GAME_OPTIONS_DUAL_JOYSTICKS)
			{
				fire_status = move_dual_joystick_player_1();
			}
			else
			{
				fire_status = move_single_joystick_player_1();
			}

			move_bullets();
			move_enemies();
			move_projectiles();
			move_food();

			slain_enemy = interaction_enemies_player_1();
			if (slain_enemy)
			{
				init_food_game(slain_enemy);
			}

			interaction_projectiles_player_1();
			interaction_food_player_1();
			new_wave_index = move_wave(&game_wave);
			if (new_wave_index)
			{
				if (new_wave_index < 255)
				{
					game_wave_index = new_wave_index;
					game_state = GAME_STATE_WAVE_DONE;
				}
				else
				{
					game_wave_index = 0;
					game_state = GAME_STATE_FINNISH;
				}
#ifdef PLAYER_GO_HOME
				player_home = 0;
#endif
				new_wave_index = 0;
				Vec_Music_Flag = 1;
			}
			else
			{
				if (fire_status)
				{
					sfx_pointer_1 = (long unsigned int) (&fire_snd_data);
					sfx_status_1 = 1;
				}
			}

			if (player_1.state_changed)
			{
				if (player_1.state == PLAYER_STATE_DYING)
				{
					if (game_state != GAME_STATE_WAVE_DONE)
					{
						sfx_pointer_1 = (long unsigned int) (&fall_snd_data);
						sfx_status_1 = 1;
					}
				}
				else if (player_1.state == PLAYER_STATE_DEAD)
				{
					if (player_1.num_lives > 0)
					{
						game_state = GAME_STATE_DEAD;
					}
					else
					{
						game_state = GAME_STATE_OVER;
					}
				}
				else if (player_1.state == PLAYER_STATE_INVINSIBLE)
				{
					game_state = GAME_STATE_NORMAL;
				}
			}

			if (game_state == GAME_STATE_NORMAL)
			{
				if (button_1_1_pressed())
				{
					game_state = GAME_STATE_PAUSE;
				}
			}
			else if (game_state == GAME_STATE_OVER)
			{
				if (button_1_4_pressed())
				{
					restart_game();
					game_state = GAME_STATE_NORMAL;
				}
			}
		}

		if (new_frame_game() == 0)
		{
			if (game_state == GAME_STATE_WAVE_DONE || game_state == GAME_STATE_FINNISH)
			{
				game_state = GAME_STATE_NORMAL;
			}
		}

		Intensity_5F();
		print_3digit_number(127, -16, player_1.score);

		if (game_state == GAME_STATE_WAVE_DONE || game_state == GAME_STATE_FINNISH)
		{
			Intensity_a(game_flashing_intensity << 1);
			drawWeb();
			dp_VIA_t1_cnt_lo = 0x80;

			Intensity_a(0x6f);
			draw_player_1();
			draw_food();

			Intensity_7F();
			draw_bullets();
			draw_projectiles();
		}
		else
		{
			Intensity_a(0x2f);
			drawWeb();
			dp_VIA_t1_cnt_lo = 0x80;

			Intensity_5F();
			draw_walls();

			Intensity_a(0x6f);
			draw_player_1();
			draw_enemies();
			draw_food();

			Intensity_7F();
			draw_bullets();
			draw_projectiles();
		}

		Intensity_5F();
		print_info_text();
	}
	
	// if return value is <= 0, then a warm reset will be performed,
	// otherwise a cold reset will be performed
	return 0;
}

// ***************************************************************************
// end of file
// ***************************************************************************
