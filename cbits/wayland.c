/*
 * Copyright Edward O'Callaghan, All rights reserved.
 */

#include <stdlib.h>
#include <swc.h>
#include <unistd.h>
#include <wayland-server.h>
#include <xkbcommon/xkbcommon.h>

/* ===================================== */

/**
 * Globals
 */

static struct screen * active_screen;
static struct window * focused_window;
static struct wl_display * display;
static struct wl_event_loop * event_loop;

/**
 * XXX make these configurable??
 */
static const uint32_t border_width        = 1;
static const uint32_t border_color_active = 0xff333388;
static const uint32_t border_color_normal = 0xff888888;

/* ===================================== */

/* ===================================== */
/*   Main dispatch structure initiation  */
/* ===================================== */

// Window
static const struct swc_window_handler window_handler = {
  /**
   * Called when the window is about to be destroyed.
   *
   * After this is called, the window is no longer valid.
   */
  .destroy = &window_destroy,
   /**
    * Called when the pointer enters the window.
    */
  .entered = &window_entered,
};

// Screen
static const struct swc_screen_handler screen_handler = {
  /**
   * Called when the geometry of the screen available for laying out windows
   * has changed.
   *
   * A window manager should respond by making sure all visible windows are
   * within this area.
   */
  .usable_geometry_changed = &screen_usable_geometry_changed,
  /**
   * Called when the pointer enters the screen.
   */
  .entered                 = &screen_entered,
};

// Manager
/**
 * This is a user-provided structure that swc will use to notify the display
 * server of new windows and screens.
 */
static const struct swc_manager manager = {
  /**
   * Called when a new screen is created.
   */
  &new_screen,
  /**
   * Called when a new window is created.
   */
  &new_window
};

/* ===================================== */

/* ===================================== */
/*              Entry point              */
/* ===================================== */
int waylandInit()
{
  display = wl_display_create();

  if (!display)
    return EXIT_FAILURE;

  if (wl_display_add_socket(display, NULL) != 0)
    return EXIT_FAILURE;

/* Initializes the compositor using the specified display, event_loop,
 * and manager. */
  if (!swc_initialize(display, NULL, &manager))
    return EXIT_FAILURE;

  // XXX bind some keys???
  // e.g.
  swc_add_binding(SWC_BINDING_KEY, SWC_MOD_LOGO, XKB_KEY_q,
      &quit, NULL);

  event_loop = wl_display_get_event_loop(display);
  wl_display_run(display);
  wl_display_destroy(display);

  return EXIT_SUCCESS;
}

void waylandWindowSetRect(struct window * window, int x, int y, int w, int h)
{
  struct swc_rectangle rect = { .x = x, .y = y, .width = w, .height = h };
  window->screen->swc->usable_geometry = &rect;
  // ??
  // screen_usable_geometry_changed(void * data) {}
}

void waylandWindowSetFocus(struct window * window)
{
  focus(window);
}

/* ===================================== */

/* ===================================== */
/*           Window Primitives           */
/* ===================================== */
static void window_entered(void * data)
{
  focus((struct window *) data);
}

static void window_destroy(void * data) {} // screen_remove_window(window->screen, window);

/* Window helper functions */
static void focus(struct window * window)
{
  if (focused_window)
  {
    swc_window_set_border(focused_window->swc,
        border_color_normal, border_width);
  }
  if (window)
  {
    swc_window_set_border(window->swc, border_color_active, border_width);
/**
 * Set the keyboard focus to the specified window.
 *
 * If window is NULL, the keyboard will have no focus.
 */
    swc_window_focus(window->swc);
  }
  else
    swc_window_focus(NULL);
  focused_window = window;
}

/* ===================================== */

/* ===================================== */
/*           Screen Primitives           */
/* ===================================== */
static void screen_usable_geometry_changed(void * data) {}

static void screen_entered(void * data)
{
  active_screen = (struct screen *) data;
}

// Screen Helper functions
static void screen_add_window(struct screen * screen, struct window * window)
{
  window->screen = screen;
  wl_list_insert(&screen->windows, &window->link);
  ++screen->num_windows;
  swc_window_show(window->swc);
//  arrange(screen);
}

static void screen_remove_window(struct screen * screen, struct window * window)
{
  window->screen = NULL;
  wl_list_remove(&window->link);
  --screen->num_windows;
  swc_window_hide(window->swc);
//  arrange(screen);
}

/* ===================================== */

/* ===================================== */
/*           Manager Primitives          */
/* ===================================== */
static void new_screen(struct swc_screen * swc) {}
static void new_window(struct swc_window * swc) {} // screen_add_window(active_screen, window);

/* ===================================== */

/* ===================================== */
/*           Key-bound Handlers          */
/* ===================================== */

static void quit(void * data, uint32_t time, uint32_t value, uint32_t state)
{
  if (state != WL_KEYBOARD_KEY_STATE_PRESSED)
    return;

  wl_display_terminate(display);
}

/* vim: set fdm=marker : */
