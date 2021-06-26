#include <pebble.h>
#include <inttypes.h>
#include "main.h"
#include <math.h>
#include <stdio.h>
#include <string.h>

#define M_PI 3.14159265358979323846
#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define PBL_WIDTH 144
#define PBL_HEIGHT 168

static Window *s_main_window;
static TextLayer *s_time_layer, *s_num_label, *s_weather_layer;

//Layer for drawing hands
static Layer *s_canvas_layer, *s_hands_layer, *s_date_layer, *s_battery_layer;
// Write the current hours and minutes into a buffer
static char s_buffer[8];
static char s_num_buffer[4];

static int s_battery_level;

bool isWhite = false;

// A struct for our specific settings (see main.h)
ClaySettings settings;

// Initialize the default settings
static void prv_default_settings() {
  settings.BackgroundColor = GColorBlack;
  settings.batteryBar = true;
}

// Read settings from persistent storage
static void prv_load_settings() {
  // Load the default settings
  prv_default_settings();
  // Read settings from persistent storage, if they exist
  persist_read_data(SETTINGS_KEY, &settings, sizeof(settings));
}

// Save the settings to persistent storage
static void prv_save_settings() {
  persist_write_data(SETTINGS_KEY, &settings, sizeof(settings));
  // Update the display based on new settings
  prv_update_display();
}

// Update the display elements
static void prv_update_display() {
  // Background color
  window_set_background_color(s_main_window, settings.BackgroundColor);

  // Seconds
  if (settings.batteryBar) {
    layer_set_hidden(s_battery_layer, false);
  } else {
    layer_set_hidden(s_battery_layer, true);
  }
}

// Handle the response from AppMessage
static void prv_inbox_received_handler(DictionaryIterator *iter, void *context) {
 // Background Color
  Tuple *bg_color_t = dict_find(iter, MESSAGE_KEY_BackgroundColor);
  if (bg_color_t) {
    settings.BackgroundColor = GColorFromHEX(bg_color_t->value->int32);
    printf("%03" PRId32 "\n", bg_color_t->value->int32);
    // 000 is Black
    // 11184810 is gray
    // 16777215 is white
    if(bg_color_t->value->int32 == 16777215){
      // This is if its white
      isWhite = true;
      text_layer_set_text_color(s_weather_layer, GColorBlack);
      text_layer_set_text_color(s_time_layer, GColorBlack);
      text_layer_set_text_color(s_num_label, GColorBlack);
    }else {
      //This is for gray and black
      isWhite = false;
      text_layer_set_text_color(s_weather_layer, GColorWhite);
      text_layer_set_text_color(s_time_layer, GColorWhite);
      text_layer_set_text_color(s_num_label, GColorWhite);

    }

  }
  // Animations
  Tuple *animations_t = dict_find(iter, MESSAGE_KEY_batteryBar);
  if (animations_t) {
    settings.batteryBar = animations_t->value->int32 == 1;
  }

  // Save the new settings to persistent storage
  prv_save_settings();
}

// Declare globally
//static GFont s_time_font;

static void update_time()
{
  // Get a tm structure
  time_t temp = time(NULL);
  struct tm *tick_time = localtime(&temp);
  struct tm *tick_timeMin = localtime(&temp);

  // Getting full time
  strftime(s_buffer, sizeof(s_buffer), clock_is_24h_style() ? "%H:%M" : "%I:%M", tick_time);
  // Display this time on the TextLayer
  text_layer_set_text(s_time_layer, s_buffer);
}

static void canvas_update_proc(Layer *layer, GContext *ctx)
{
  //Dithering effect I made for fun
  // for (int i = 0; i < 144; i++)
  // {
  //   for (int j = 0; j < 168; j++)
  //   {
  //     if (i % 2 == 0 && j % 2 == 0)
  //     {
  //       graphics_draw_pixel(ctx, GPoint(i, j));
  //     }
  //     else if (i % 2 == 0 && !j % 2 == 0)
  //     {
  //       graphics_draw_pixel(ctx, GPoint(i + 1, j));
  //     }
  //   }
  // }
  //drawMarkers(*ctx);

  //I got SOOOOO lazy that I cant even figure out how to make function work and will just move code here... Please future me, figure this out
  GRect markTop_bounds = GRect(72, 0, 2, 20);
  GRect markBottom_bounds = GRect(71, 144, 2, 24);
  GRect markRight_bounds = GRect(120, 84, 24, 2);
  GRect markLeft_bounds = GRect(0, 84, 24, 2);

  if(isWhite){
    graphics_context_set_stroke_color(ctx, GColorBlack);
  }else {
    graphics_context_set_stroke_color(ctx, GColorWhite);
  }
  graphics_draw_rect(ctx, markTop_bounds);
  graphics_draw_rect(ctx, markRight_bounds);
  graphics_draw_rect(ctx, markBottom_bounds);
  graphics_draw_rect(ctx, markLeft_bounds);
}

static void hands_update_proc(Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);
  GPoint center = grect_center_point(&bounds);

  const int16_t second_hand_length = PBL_IF_ROUND_ELSE((bounds.size.w / 2) - 19, bounds.size.w / 2);
  int16_t maxLength = (MIN(bounds.size.w, bounds.size.h) - 20) / 2;

  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  int32_t minute_angle = TRIG_MAX_ANGLE * t->tm_min / 60;
  GPoint minute_hand = {
      .x = (int16_t)(sin_lookup(minute_angle) * (int32_t)maxLength / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(minute_angle) * (int32_t)maxLength / TRIG_MAX_RATIO) + center.y,
  };

  int32_t hour_angle = (TRIG_MAX_ANGLE * (((t->tm_hour % 12) * 6) + (t->tm_min / 10))) / (12 * 6);
  GPoint hour_hand = {
      .x = (int16_t)(sin_lookup(hour_angle) * (int32_t)(maxLength * 0.6) / TRIG_MAX_RATIO) + center.x,
      .y = (int16_t)(-cos_lookup(hour_angle) * (int32_t)(maxLength * 0.6) / TRIG_MAX_RATIO) + center.y,
  };

  if(isWhite){
    graphics_context_set_stroke_color(ctx, GColorBlack);
  }else {
    graphics_context_set_stroke_color(ctx, GColorWhite);
  }
  
  graphics_context_set_stroke_width(ctx, 4);

  graphics_draw_line(ctx, minute_hand, center);
  graphics_draw_line(ctx, hour_hand, center);
}

static void date_update_proc(Layer *layer, GContext *ctx)
{
  time_t now = time(NULL);
  struct tm *t = localtime(&now);

  strftime(s_num_buffer, sizeof(s_num_buffer), "%d", t);
  text_layer_set_text(s_num_label, s_num_buffer);
}

static void battery_update_proc(Layer *layer, GContext *ctx)
{
  GRect bounds = layer_get_bounds(layer);

  // Find the width of the bar (total width = 114px)
  int width = (s_battery_level * 70) / 100;

  // Draw the background
  graphics_context_set_fill_color(ctx, GColorLightGray);
  graphics_fill_rect(ctx, bounds, 0, GCornerNone);

  // Draw the bar
  if(isWhite){
    graphics_context_set_fill_color(ctx, GColorBlack);
  }else {
    graphics_context_set_fill_color(ctx, GColorWhite);
  }
  
  graphics_fill_rect(ctx, GRect(0, 0, width, bounds.size.h), 0, GCornerNone);
}

static void prv_window_load(Window *window)
{
  // Get information about the Window
  Layer *window_layer = window_get_root_layer(window);
  GRect bounds = layer_get_bounds(window_layer);

  // Create canvas layer
  s_canvas_layer = layer_create(bounds);

  //boundsTime = bounds;
  s_time_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 20), bounds.size.w, 50));
  // Create the TextLayer with specificaddddddddddddddddddd bounds

  s_hands_layer = layer_create(bounds);

  s_date_layer = layer_create(bounds);
  layer_set_update_proc(s_date_layer, date_update_proc);
  layer_add_child(window_layer, s_date_layer);

  // Create temperature Layer
  s_weather_layer = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(125, 50), bounds.size.w, 25));

  // Style the text
  text_layer_set_background_color(s_weather_layer, GColorClear);
  text_layer_set_text_color(s_weather_layer, GColorWhite);
  text_layer_set_text_alignment(s_weather_layer, GTextAlignmentCenter);
  text_layer_set_text(s_weather_layer, "Loading...");

  // Create GFont
  //s_time_font = fonts_load_custom_font(resource_get_handle(RESOURCE_ID_FONT_LECO_20));

  // Improve the layout to be more like a watchface
  text_layer_set_background_color(s_time_layer, GColorClear);
  text_layer_set_text_color(s_time_layer, GColorWhite);
  text_layer_set_text(s_time_layer, "00:00");
  text_layer_set_font(s_time_layer, fonts_get_system_font(FONT_KEY_LECO_26_BOLD_NUMBERS_AM_PM));
  //text_layer_set_font(s_time_layer, s_time_font);
  text_layer_set_text_alignment(s_time_layer, GTextAlignmentCenter);

  s_num_label = text_layer_create(
      GRect(0, PBL_IF_ROUND_ELSE(58, 72), 195, 50));
  text_layer_set_text(s_num_label, s_num_buffer);
  text_layer_set_background_color(s_num_label, GColorClear);
  text_layer_set_text_color(s_num_label, GColorWhite);
  text_layer_set_font(s_num_label, fonts_get_system_font(FONT_KEY_LECO_20_BOLD_NUMBERS));
  text_layer_set_text_alignment(s_num_label, GTextAlignmentCenter);

  // Create battery meter Layer
  s_battery_layer = layer_create(GRect((PBL_WIDTH / 2) - 35, 50, 70, 2));
  layer_set_update_proc(s_battery_layer, battery_update_proc);

  // Add to Window
  layer_add_child(window_get_root_layer(window), s_battery_layer);

  layer_add_child(s_date_layer, text_layer_get_layer(s_num_label));

  // Add it as a child layer to the Window's root layer
  layer_add_child(window_layer, text_layer_get_layer(s_time_layer));

  // Assign the custom drawing procedure
  layer_set_update_proc(s_canvas_layer, canvas_update_proc);

  //s_date_layer

  layer_set_update_proc(s_hands_layer, hands_update_proc);

  layer_add_child(window_layer, s_hands_layer);
  layer_add_child(window_get_root_layer(window), text_layer_get_layer(s_weather_layer));

  // Add to Window
  layer_add_child(window_get_root_layer(window), s_canvas_layer);
  prv_update_display();
  printf("%s", settings.batteryBar ? "true" : "false");
}

static void prv_window_unload(Window *window)
{
  // Destroy TextLayer
  layer_destroy(s_battery_layer);
  text_layer_destroy(s_weather_layer);
  text_layer_destroy(s_time_layer);
  layer_destroy(s_date_layer);
  text_layer_destroy(s_num_label);
}

static void tick_handler(struct tm *tick_time, TimeUnits units_changed)
{
  // Get weather update every 30 minutes
  if (tick_time->tm_min % 30 == 0)
  {
    // Begin dictionary
    DictionaryIterator *iter;
    app_message_outbox_begin(&iter);

    // Add a key-value pair
    dict_write_uint8(iter, 0, 0);

    // Send the message!
    app_message_outbox_send();
  }
  update_time();
  //redraw de layers
  layer_mark_dirty(s_canvas_layer);
  layer_mark_dirty(s_hands_layer);
}

//Getting tempurature stuff
static void inbox_received_callback(DictionaryIterator *iterator, void *context)
{
  // Store incoming information
  static char temperature_buffer[8];
  static char conditions_buffer[32];
  static char weather_layer_buffer[32];

  // Read tuples for data
  Tuple *temp_tuple = dict_find(iterator, MESSAGE_KEY_TEMPERATURE);
  Tuple *conditions_tuple = dict_find(iterator, MESSAGE_KEY_CONDITIONS);

  // If all data is available, use it
  if (temp_tuple && conditions_tuple)
  {
    snprintf(temperature_buffer, sizeof(temperature_buffer), "%dC", (int)temp_tuple->value->int32);
    snprintf(conditions_buffer, sizeof(conditions_buffer), "%s", conditions_tuple->value->cstring);
  }

  // Assemble full string and display
  snprintf(weather_layer_buffer, sizeof(weather_layer_buffer), "%s, %s", temperature_buffer, conditions_buffer);
  text_layer_set_text(s_weather_layer, weather_layer_buffer);
}

static void inbox_dropped_callback(AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Message dropped!");
}

static void outbox_failed_callback(DictionaryIterator *iterator, AppMessageResult reason, void *context)
{
  APP_LOG(APP_LOG_LEVEL_ERROR, "Outbox send failed!");
}

static void outbox_sent_callback(DictionaryIterator *iterator, void *context)
{
  APP_LOG(APP_LOG_LEVEL_INFO, "Outbox send success!");
}

static void battery_callback(BatteryChargeState state)
{
  // Record the new battery level
  s_battery_level = state.charge_percent;
  // Update meter
  layer_mark_dirty(s_battery_layer);
}

static void prv_init()
{
  prv_load_settings();
  // Create main Window element and assign to pointer
  s_main_window = window_create();

  // Set handlers to manage the elements inside the Window
  window_set_window_handlers(s_main_window, (WindowHandlers){
                                                .load = prv_window_load,
                                                .unload = prv_window_unload});

  // Show the Window on the watch, with animated=true
  window_stack_push(s_main_window, true);

  //window_set_background_color(s_main_window, GColorBlack);

  // Register with TickTimerService
  tick_timer_service_subscribe(MINUTE_UNIT, tick_handler);

  // Register callbacks
  app_message_register_inbox_received(inbox_received_callback);
  // Open AppMessage
  const int inbox_size = 128;
  const int outbox_size = 128;
  app_message_open(inbox_size, outbox_size);
  app_message_register_inbox_dropped(inbox_dropped_callback);
  app_message_register_outbox_failed(outbox_failed_callback);
  app_message_register_outbox_sent(outbox_sent_callback);

    // Open AppMessage connection
  app_message_register_inbox_received(prv_inbox_received_handler);
  app_message_open(128, 128);
  // Register for battery level updates
  battery_state_service_subscribe(battery_callback);

  // Ensure battery level is displayed from the start
battery_callback(battery_state_service_peek());

  // Make sure the time is displayed from the start
  update_time();
}

static void prv_deinit()
{
  // Destroy Window
  tick_timer_service_unsubscribe();
  window_destroy(s_main_window);
}

int main(void)
{
  prv_init();
  app_event_loop();
  prv_deinit();
}