/*  RetroArch - A frontend for libretro.
 *  Copyright (C) 2010-2014 - Hans-Kristian Arntzen
 *  Copyright (C) 2011-2015 - Daniel De Matteis
 * 
 *  RetroArch is free software: you can redistribute it and/or modify it under the terms
 *  of the GNU General Public License as published by the Free Software Found-
 *  ation, either version 3 of the License, or (at your option) any later version.
 *
 *  RetroArch is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY;
 *  without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 *  PURPOSE.  See the GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along with RetroArch.
 *  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __MENU_DRIVER_H__
#define __MENU_DRIVER_H__

#include <stddef.h>
#include <stdint.h>
#include <boolean.h>
#include <retro_miscellaneous.h>
#include <queues/message_queue.h>
#include "menu_animation.h"
#include "menu_displaylist.h"
#include "menu_list.h"
#include "menu_navigation.h"
#include "menu_database.h"
#include "../settings_list.h"
#include "../../libretro.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MENU_MAX_BUTTONS 219

#define MENU_MAX_AXES 32
#define MENU_MAX_HATS 4

#ifndef MAX_USERS
#define MAX_USERS 16
#endif

struct menu_bind_state_port
{
   bool buttons[MENU_MAX_BUTTONS];
   int16_t axes[MENU_MAX_AXES];
   uint16_t hats[MENU_MAX_HATS];
};

struct menu_bind_axis_state
{
   /* Default axis state. */
   int16_t rested_axes[MENU_MAX_AXES];
   /* Locked axis state. If we configured an axis,
    * avoid having the same axis state trigger something again right away. */
   int16_t locked_axes[MENU_MAX_AXES];
};

struct menu_bind_state
{
   struct retro_keybind *target;
   /* For keyboard binding. */
   int64_t timeout_end;
   unsigned begin;
   unsigned last;
   unsigned user;
   struct menu_bind_state_port state[MAX_USERS];
   struct menu_bind_axis_state axis_state[MAX_USERS];
   bool skip;
};

typedef struct menu_framebuf
{
   uint16_t *data;
   unsigned width;
   unsigned height;
   size_t pitch;
} menu_framebuf_t;

typedef struct
{
   void *userdata;

   /* Delta timing */
   float dt;
   retro_time_t cur_time;
   retro_time_t old_time;

   /* Used for key repeat */
   struct
   {
      float timer;
      float count;
   } delay;

   size_t begin;
   unsigned header_height;
   float scroll_y;

   menu_list_t *menu_list;
   menu_navigation_t navigation;

   struct
   {
      size_t selection_ptr;
      size_t size;
   } categories;

   bool need_refresh;
   bool nonblocking_refresh;
   bool msg_force;
   bool push_start_screen;

   bool defer_core;
   char deferred_path[PATH_MAX_LENGTH];

   /* This buffer can be used to display generic OK messages to the user.
    * Fill it and call
    * menu_list_push(driver->menu->menu_stack, "", "message", 0, 0);
    */
   char message_contents[PATH_MAX_LENGTH];

   msg_queue_t *msg_queue;

   char default_glslp[PATH_MAX_LENGTH];
   char default_cgp[PATH_MAX_LENGTH];

   menu_framebuf_t frame_buf;


   struct
   {
      void *buf;
      int size;

      const uint8_t *framebuf;
      bool alloc_framebuf;
   } font;

   bool load_no_content;

   struct video_shader *shader;

   struct menu_bind_state binds;

   struct
   {
      int16_t dx;
      int16_t dy;
      int16_t x;
      int16_t y;
      int16_t screen_x;
      int16_t screen_y;
      bool    left;
      bool    right;
      bool    oldleft;
      bool    oldright;
      bool    wheelup;
      bool    wheeldown;
      bool    hwheelup;
      bool    hwheeldown;
      bool    scrollup;
      bool    scrolldown;
      unsigned ptr;
   } mouse;

   struct
   {
      int16_t x;
      int16_t y;
      int16_t dx;
      int16_t dy;
      int16_t old_x;
      int16_t old_y;
      int16_t start_x;
      int16_t start_y;
      bool pressed[2];
      bool oldpressed[2];
      bool dragging;
      bool back;
      bool oldback;
      unsigned ptr;
   } pointer;

   struct
   {
      const char **buffer;
      const char *label;
      const char *label_setting;
      bool display;
      unsigned type;
      unsigned idx;
   } keyboard;

   struct
   {
      bool is_updated;
   } label;

   struct
   {
      bool dirty;
   } framebuf;

   struct
   {
      bool active;
   } action;

   struct
   {
      unsigned joypad;
      uint64_t mouse;
   } input;

   rarch_setting_t *list_settings;
   animation_t *animation;
   bool animation_is_active;

   content_playlist_t *playlist;
   char db_playlist_file[PATH_MAX_LENGTH];
} menu_handle_t;

typedef struct menu_ctx_driver
{
   void  (*set_texture)(void);
   void  (*render_messagebox)(const char *msg);
   void  (*render)(void);
   void  (*frame)(void);
   void* (*init)(void);
   void  (*free)(void*);
   void  (*context_reset)(void);
   void  (*context_destroy)(void);
   void  (*populate_entries)(const char *path, const char *label,
         unsigned k);
   void  (*toggle)(bool);
   void  (*navigation_clear)(bool);
   void  (*navigation_decrement)(void);
   void  (*navigation_increment)(void);
   void  (*navigation_set)(bool);
   void  (*navigation_set_last)(void);
   void  (*navigation_descend_alphabet)(size_t *);
   void  (*navigation_ascend_alphabet)(size_t *);
   void  (*list_insert)(file_list_t *list, const char *, const char *, size_t);
   void  (*list_delete)(file_list_t *list, size_t, size_t);
   void  (*list_clear)(file_list_t *list);
   void  (*list_cache)(bool, unsigned);
   void  (*list_set_selection)(file_list_t *list);
   bool  (*load_background)(void *data);
   const char *ident;
   bool  (*perform_action)(void* data, unsigned action);
} menu_ctx_driver_t;

extern menu_ctx_driver_t menu_ctx_rmenu;
extern menu_ctx_driver_t menu_ctx_rmenu_xui;
extern menu_ctx_driver_t menu_ctx_rgui;
extern menu_ctx_driver_t menu_ctx_glui;
extern menu_ctx_driver_t menu_ctx_xmb;
extern menu_ctx_driver_t menu_ctx_null;

/**
 * menu_driver_find_handle:
 * @index              : index of driver to get handle to.
 *
 * Returns: handle to menu driver at index. Can be NULL
 * if nothing found.
 **/
const void *menu_driver_find_handle(int index);

/**
 * menu_driver_find_ident:
 * @index              : index of driver to get handle to.
 *
 * Returns: Human-readable identifier of menu driver at index. Can be NULL
 * if nothing found.
 **/
const char *menu_driver_find_ident(int index);

/**
 * config_get_menu_driver_options:
 *
 * Get an enumerated list of all menu driver names,
 * separated by '|'.
 *
 * Returns: string listing of all menu driver names,
 * separated by '|'.
 **/
const char* config_get_menu_driver_options(void);

void find_menu_driver(void);

void init_menu(void);

menu_handle_t *menu_driver_get_ptr(void);

void menu_driver_navigation_increment(void);

void menu_driver_navigation_decrement(void);

void menu_driver_navigation_clear(bool pending_push);

void menu_driver_navigation_set(bool scroll);

void menu_driver_navigation_set_last(void);

void menu_driver_set_texture(void);

void menu_driver_frame(void);

void menu_driver_context_reset(void);

void menu_driver_free(menu_handle_t *menu);

void menu_driver_render(void);

void menu_driver_toggle(bool latch);

void menu_driver_render_messagebox(const char *msg);

void menu_driver_populate_entries(const char *path, const char *label,
         unsigned k);

bool menu_driver_load_background(void *data);

void  menu_driver_navigation_descend_alphabet(size_t *);

void  menu_driver_navigation_ascend_alphabet(size_t *);

void menu_driver_list_cache(bool state, unsigned action);

void  menu_driver_list_delete(file_list_t *list, size_t i, size_t list_size);

void  menu_driver_list_insert(file_list_t *list, const char *path,
      const char *label, unsigned type, size_t list_size);

void  menu_driver_list_clear(file_list_t *list);

void  menu_driver_list_set_selection(file_list_t *list);

const menu_ctx_driver_t *menu_ctx_driver_get_ptr(void);

void  menu_driver_context_destroy(void);

bool menu_driver_alive(void);

void menu_driver_set_alive(void);

void menu_driver_unset_alive(void);

#ifdef __cplusplus
}
#endif

#endif

