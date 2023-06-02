#ifndef ENGINE_CONTEXT_H_
#define ENGINE_CONTEXT_H_

#include "e_util.h"
#include <stdbool.h>
#include <stdint.h>

/* GENERAL CONTEXT */
void context_init(uint width, uint height, const char *title);
bool context_is_running(void);
float context_get_time(void);
void context_set_cursor_locked(bool toggle);
void context_get_mpos(vec2 pos);
void context_poll_and_swap_buffers(void);
void context_terminate(void);

/* INPUT */

struct input {
	uint8_t w_last : 1;
	uint8_t a_last : 1;
	uint8_t s_last : 1;
	uint8_t d_last : 1;
	uint8_t jump_last : 1;
	uint8_t run_last : 1;
	uint8_t w_diff : 1;
	uint8_t a_diff : 1;
	uint8_t s_diff : 1;
	uint8_t d_diff : 1;
	uint8_t jump_diff : 1;
	uint8_t run_diff : 1;
	uint8_t pad : 4;
	vec2 mpos_last;
	vec2 mpos_diff;
};

struct input context_get_input(struct input i);
void context_print_input(struct input i);

#endif /* ENGINE_CONTEXT_H_ */
