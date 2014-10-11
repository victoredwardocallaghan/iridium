/*
 * Copyright Edward O'Callaghan, All rights reserved.
 */

#ifndef __WAYLAND_H
#define __WAYLAND_H

struct window;
struct screen;

/**
 * Window Structure
 */

/*
 * struct swc_window
 *   char * title;
 *   char * class;
 *   struct swc_window * parent;
 *
 */
struct window
{
  struct swc_window * swc;
  struct screen * screen;
  struct wl_list link;
};

/* ===================================== */

/**
 * Screen Structure
 */

/*
 * swc_screen
 *    > The total area of the screen.
 * struct swc_rectangle geometry;
 *
 *    > The area of the screen available for placing windows.
 * struct swc_rectangle usable_geometry;
 *
 * where,
 *
 * struct swc_rectangle
 *   int32_t x, y;
 *   uint32_t width, height;
 *
 */
struct screen
{
  struct swc_screen * swc; //< equiv of IRFrame * frames;
  struct wl_list windows;
  unsigned num_windows;
};

/* ===================================== */

int waylandInit();

#endif /* __WAYLAND_H */
