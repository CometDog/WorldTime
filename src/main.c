#include "main.h"
#include "@pebble-libraries/debug-tick-timer-service/debug-tick-timer-service.h"
#include "@pebble-libraries/pbl-stdlib/pbl-stdlib.h"
#include "@pebble-libraries/pbl-display/pbl-display.h"
#include "@pebble-libraries/pbl-math/pbl-math.h"

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

void draw_hours(uint64_t inset, uint8_t digit_shift, GColor color, GContext *context)
{
  GFont font = fonts_get_system_font(FONT_KEY);

  uint8_t square_face_positions_buffer[12][2];
  square_face_positions(FONT_SIZE, inset, inset, square_face_positions_buffer);

  for (uint8_t i = 0; i < 12; i++)
  {
    uint8_t x_coord = square_face_positions_buffer[i][0];
    uint8_t y_coord = square_face_positions_buffer[i][1];

    char digits[3];
    pbl_itoa(((i + digit_shift) % 12) + 1, digits, 10);
    graphics_context_set_text_color(context, color);
    graphics_draw_text(context, digits, font, GRect(x_coord, y_coord, FONT_SIZE, FONT_SIZE), GTextOverflowModeFill, GTextAlignmentCenter, NULL);
  }
}

void layer_update_inner_dial(Layer *layer, GContext *context)
{
  graphics_context_set_fill_color(context, GColorWhite);
  graphics_fill_rect(context, GRect(30, 30, DISPLAY_WIDTH - 56, DISPLAY_HEIGHT - 56), 6, GCornersAll);

  graphics_context_set_stroke_color(context, GColorBlack);
  graphics_context_set_stroke_width(context, 3);
  graphics_draw_line(context, GPoint(DISPLAY_CENTER_X, DISPLAY_CENTER_Y), GPoint(DISPLAY_CENTER_X, DISPLAY_CENTER_Y - 25));
  graphics_context_set_stroke_width(context, 3);
  graphics_draw_line(context, GPoint(DISPLAY_CENTER_X, DISPLAY_CENTER_Y), GPoint(DISPLAY_CENTER_X - 40, DISPLAY_CENTER_Y));
}

/**
 * Update the border hours
 * @param layer Layer to update
 * @param context Graphics context to use during update
 */
void layer_update_tz_2(Layer *layer, GContext *context)
{
  draw_hours(0, 9, GColorWhite, context);
}

/**
 * Update the border hours
 * @param layer Layer to update
 * @param context Graphics context to use during update
 */
void layer_update_tz_1(Layer *layer, GContext *context)
{
  draw_hours(14, 11, GColorWhite, context);
}

/**
 * Update the border hours
 * @param layer Layer to update
 * @param context Graphics context to use during update
 */
void layer_update_main_tz(Layer *layer, GContext *context)
{
  draw_hours(28, 0, GColorBlack, context);
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

  tz1 = layer_create(bounds);
  layer_set_update_proc(tz1, layer_update_tz_1);

  tz1_label = text_layer_create(GRect(0, 14, 28, 14));
  text_layer_set_background_color(tz1_label, GColorBlack);
  text_layer_set_text_color(tz1_label, GColorWhite);
  text_layer_set_text(tz1_label, "JST");
  text_layer_add_to_layer(tz1_label, tz1);

  tz2 = layer_create(bounds);
  layer_set_update_proc(tz2, layer_update_tz_2);

  tz2_label = text_layer_create(GRect(0, 0, 28, 14));
  text_layer_set_background_color(tz2_label, GColorBlack);
  text_layer_set_text_color(tz2_label, GColorWhite);
  text_layer_set_text(tz2_label, "PST");
  text_layer_add_to_layer(tz2_label, tz2);

  inner_dial = layer_create(bounds);
  layer_set_update_proc(inner_dial, layer_update_inner_dial);

  outer_dial = layer_create(bounds);
  layer_add_to_layer(main_tz, inner_dial);
  layer_add_to_layer(inner_dial, outer_dial);

  layer_add_to_layer(tz1, outer_dial);
  layer_add_to_layer(tz2, outer_dial);
  layer_add_to_layer(outer_dial, background);

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