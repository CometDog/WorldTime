#include "main.h"
#include "@pebble-libraries/debug-tick-timer-service/debug-tick-timer-service.h"
#include "@pebble-libraries/pbl-stdlib/pbl-stdlib.h"
#include "@pebble-libraries/pbl-display/pbl-display.h"
#include "@pebble-libraries/pbl-math/pbl-math.h"

#define RADIUS 6
#define FONT_KEY "FONT_KEY_GOTHIC_14"
#define FONT_SIZE 14
#define DIAL_INSET FONT_SIZE

/**
 * Handle time tick event
 * @param tick_time Pointer to the time structure
 * @param units_changed The units that have changed.
 */
static void
tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
}

/**
 * Update the border hours
 * @param layer Layer to update
 * @param context Graphics context to use during update
 */
void layer_update_main_tz(Layer *layer, GContext *context)
{
  GFont font = fonts_get_system_font(FONT_KEY);

  uint8_t square_face_positions_buffer[12][2];
  square_face_positions(FONT_SIZE, 0, 0, square_face_positions_buffer);

  for (uint8_t i = 1; i <= 12; i++)
  {
    uint8_t x_coord = square_face_positions_buffer[i - 1][0];
    uint8_t y_coord = square_face_positions_buffer[i - 1][1];

    char digits[3];
    pbl_itoa(i, digits, 10);
    graphics_draw_text(context, digits, font, GRect(x_coord, y_coord, FONT_SIZE, FONT_SIZE), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }
}

/**
 * Main window load handler
 * @param window The window being loaded
 */
static void main_window_load(Window *window)
{
  window_set_background_color(window, GColorBlack);
  GRect bounds = window_get_bounds(window);
  background = layer_create(bounds);
  // Load the background stuff here
  main_tz = layer_create(bounds);
  layer_set_update_proc(main_tz, layer_update_main_tz);

  dial = layer_create(bounds);
  layer_add_to_layer(main_tz, dial);
  layer_add_to_layer(dial, background);

  layer_add_to_window(background, window);
}

/**
 * Main window unload handler
 * @param window The window being unloaded
 */
static void main_window_unload(Window *window)
{
  layer_destroy_safe(background);
}

/**
 * Initialize the app
 */
static void init()
{
  main_window = window_create();
  window_handlers(main_window, main_window_load, main_window_unload);
  window_stack_push(main_window, true);

  debug_tick_timer_service_subscribe(MINUTE_UNIT, tick_handler, REAL);
}

/**
 * Deinitialize the app
 */
static void deinit()
{
  debug_tick_timer_service_unsubscribe();
  window_destroy_safe(main_window);
}

/**
 * Main entry point
 */
int main(void)
{
  init();
  app_event_loop();
  deinit();
}