#include "config_ui.h"

#include "lvgl.h"
#include "drivers/sdl/lv_sdl_mouse.h"
#include "drivers/sdl/lv_sdl_keyboard.h"

#include <stdio.h>

typedef struct {
    MpmDeviceConfigStore* store;
    lv_obj_t* hr_spin;
    lv_obj_t* pr_spin;
    lv_obj_t* spo2_spin;
    lv_obj_t* resp_spin;
    lv_obj_t* temp_spin;
    lv_obj_t* sys_spin;
    lv_obj_t* dia_spin;
    lv_obj_t* map_label;
    lv_obj_t* temp_label;
    lv_group_t* group;
} ConfigUiState;

static ConfigUiState g_ui;

static void update_map_label(void)
{
    const int32_t sys = lv_spinbox_get_value(g_ui.sys_spin);
    const int32_t dia = lv_spinbox_get_value(g_ui.dia_spin);
    const uint32_t map = mpm_config_calc_map((uint32_t)sys, (uint32_t)dia);
    lv_label_set_text_fmt(g_ui.map_label, "MAP: %u", map);
}

static void update_temp_label(void)
{
    const int32_t temp = lv_spinbox_get_value(g_ui.temp_spin);
    lv_label_set_text_fmt(g_ui.temp_label, "(%d.%d C)",
                          temp / 10,
                          temp % 10);
}

static void on_hr_changed(lv_event_t* e)
{
    (void)e;
    const int32_t value = lv_spinbox_get_value(g_ui.hr_spin);
    if (!mpm_config_set_hr(g_ui.store, (uint32_t)value)) {
        lv_spinbox_set_value(g_ui.hr_spin, (int32_t)mpm_config_get_hr(g_ui.store));
    }
}

static void on_pr_changed(lv_event_t* e)
{
    (void)e;
    const int32_t value = lv_spinbox_get_value(g_ui.pr_spin);
    if (!mpm_config_set_pr(g_ui.store, (uint32_t)value)) {
        lv_spinbox_set_value(g_ui.pr_spin, (int32_t)mpm_config_get_pr(g_ui.store));
    }
}

static void on_spo2_changed(lv_event_t* e)
{
    (void)e;
    const int32_t value = lv_spinbox_get_value(g_ui.spo2_spin);
    if (!mpm_config_set_spo2(g_ui.store, (uint32_t)value)) {
        lv_spinbox_set_value(g_ui.spo2_spin, (int32_t)mpm_config_get_spo2(g_ui.store));
    }
}

static void on_resp_changed(lv_event_t* e)
{
    (void)e;
    const int32_t value = lv_spinbox_get_value(g_ui.resp_spin);
    if (!mpm_config_set_resp_rate(g_ui.store, (uint32_t)value)) {
        lv_spinbox_set_value(g_ui.resp_spin, (int32_t)mpm_config_get_resp_rate(g_ui.store));
    }
}

static void on_temp_changed(lv_event_t* e)
{
    (void)e;
    const int32_t value = lv_spinbox_get_value(g_ui.temp_spin);
    if (!mpm_config_set_temperature(g_ui.store, (uint32_t)value)) {
        lv_spinbox_set_value(g_ui.temp_spin, (int32_t)mpm_config_get_temperature(g_ui.store));
    }
    update_temp_label();
}

static void on_nibp_changed(lv_event_t* e)
{
    (void)e;
    const int32_t sys = lv_spinbox_get_value(g_ui.sys_spin);
    const int32_t dia = lv_spinbox_get_value(g_ui.dia_spin);
    if (!mpm_config_set_nibp(g_ui.store, (uint32_t)sys, (uint32_t)dia)) {
        lv_spinbox_set_value(g_ui.sys_spin, (int32_t)mpm_config_get_nibp_sys(g_ui.store));
        lv_spinbox_set_value(g_ui.dia_spin, (int32_t)mpm_config_get_nibp_dia(g_ui.store));
    }
    update_map_label();
}

static void on_spin_dec_clicked(lv_event_t* e)
{
    lv_obj_t* spinbox = (lv_obj_t*)lv_event_get_user_data(e);
    lv_spinbox_decrement(spinbox);
    lv_obj_send_event(spinbox, LV_EVENT_VALUE_CHANGED, NULL);
}

static void on_spin_inc_clicked(lv_event_t* e)
{
    lv_obj_t* spinbox = (lv_obj_t*)lv_event_get_user_data(e);
    lv_spinbox_increment(spinbox);
    lv_obj_send_event(spinbox, LV_EVENT_VALUE_CHANGED, NULL);
}

static lv_obj_t* create_step_button(lv_obj_t* parent,
                                    const char* text,
                                    int x,
                                    int y,
                                    lv_event_cb_t cb,
                                    lv_obj_t* spinbox)
{
    lv_obj_t* button = lv_button_create(parent);
    lv_obj_set_size(button, 28, 22);
    lv_obj_set_pos(button, x, y - 2);
    lv_obj_add_event_cb(button, cb, LV_EVENT_CLICKED, spinbox);

    lv_obj_t* label = lv_label_create(button);
    lv_label_set_text(label, text);
    lv_obj_center(label);
    return button;
}

static void style_spinbox(lv_obj_t* spinbox)
{
    lv_obj_set_height(spinbox, 24);
    lv_obj_set_style_bg_color(spinbox, lv_color_hex(0x202020), LV_PART_MAIN);
    lv_obj_set_style_border_color(spinbox, lv_color_hex(0x5A5A5A), LV_PART_MAIN);
    lv_obj_set_style_border_width(spinbox, 1, LV_PART_MAIN);
    lv_obj_set_style_text_color(spinbox, lv_color_hex(0xFFFFFF), LV_PART_MAIN);
}

static lv_obj_t* create_param_row(lv_obj_t* parent,
                                  const char* title,
                                  lv_obj_t** spinbox,
                                  int32_t min_value,
                                  int32_t max_value,
                                  int32_t initial,
                                  lv_event_cb_t on_changed,
                                  int y)
{
    lv_obj_t* label = lv_label_create(parent);
    lv_label_set_text(label, title);
    lv_obj_set_style_text_color(label, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_pos(label, 8, y);

    *spinbox = lv_spinbox_create(parent);
    lv_spinbox_set_range(*spinbox, min_value, max_value);
    lv_spinbox_set_digit_format(*spinbox, 3, 0);
    lv_spinbox_set_value(*spinbox, initial);
    lv_obj_set_width(*spinbox, 72);
    lv_obj_set_pos(*spinbox, 72, y - 4);
    style_spinbox(*spinbox);
    lv_obj_add_event_cb(*spinbox, on_changed, LV_EVENT_VALUE_CHANGED, NULL);

    create_step_button(parent, "-", 150, y, on_spin_dec_clicked, *spinbox);
    create_step_button(parent, "+", 182, y, on_spin_inc_clicked, *spinbox);

    if (g_ui.group != NULL) {
        lv_group_add_obj(g_ui.group, *spinbox);
    }

    return *spinbox;
}

static void build_ui(MpmDeviceConfigStore* store)
{
    g_ui.store = store;

    lv_obj_t* scr = lv_screen_active();
    lv_obj_set_style_bg_color(scr, lv_color_hex(0x101010), 0);

    lv_obj_t* title = lv_label_create(scr);
    lv_label_set_text(title, "Device Config (DEMO)");
    lv_obj_set_style_text_color(title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_align(title, LV_ALIGN_TOP_MID, 0, 4);

    lv_obj_t* hint = lv_label_create(scr);
    lv_label_set_text(hint, "Click value, then +/- or Up/Down");
    lv_obj_set_style_text_color(hint, lv_color_hex(0x888888), 0);
    lv_obj_align(hint, LV_ALIGN_TOP_MID, 0, 22);

    int y = 40;
    const int row_h = 24;

    create_param_row(scr, "HR", &g_ui.hr_spin, 38, 188,
                     (int32_t)mpm_config_get_hr(store), on_hr_changed, y);
    y += row_h;
    create_param_row(scr, "PR", &g_ui.pr_spin, 38, 188,
                     (int32_t)mpm_config_get_pr(store), on_pr_changed, y);
    y += row_h;
    create_param_row(scr, "SpO2", &g_ui.spo2_spin, 50, 99,
                     (int32_t)mpm_config_get_spo2(store), on_spo2_changed, y);
    y += row_h;
    create_param_row(scr, "Resp", &g_ui.resp_spin, 6, 38,
                     (int32_t)mpm_config_get_resp_rate(store), on_resp_changed, y);
    y += row_h;

    lv_obj_t* temp_title = lv_label_create(scr);
    lv_label_set_text(temp_title, "Temp");
    lv_obj_set_style_text_color(temp_title, lv_color_hex(0xFFFFFF), 0);
    lv_obj_set_pos(temp_title, 8, y);
    g_ui.temp_spin = lv_spinbox_create(scr);
    lv_spinbox_set_range(g_ui.temp_spin, 288, 420);
    lv_spinbox_set_digit_format(g_ui.temp_spin, 3, 0);
    lv_spinbox_set_value(g_ui.temp_spin, (int32_t)mpm_config_get_temperature(store));
    lv_obj_set_width(g_ui.temp_spin, 72);
    lv_obj_set_pos(g_ui.temp_spin, 72, y - 4);
    style_spinbox(g_ui.temp_spin);
    lv_obj_add_event_cb(g_ui.temp_spin, on_temp_changed, LV_EVENT_VALUE_CHANGED, NULL);
    create_step_button(scr, "-", 150, y, on_spin_dec_clicked, g_ui.temp_spin);
    create_step_button(scr, "+", 182, y, on_spin_inc_clicked, g_ui.temp_spin);
    if (g_ui.group != NULL) {
        lv_group_add_obj(g_ui.group, g_ui.temp_spin);
    }
    g_ui.temp_label = lv_label_create(scr);
    lv_obj_set_style_text_color(g_ui.temp_label, lv_color_hex(0xCCCCCC), 0);
    lv_obj_set_pos(g_ui.temp_label, 220, y);
    update_temp_label();
    y += row_h;

    create_param_row(scr, "SYS", &g_ui.sys_spin, 85, 180,
                     (int32_t)mpm_config_get_nibp_sys(store), on_nibp_changed, y);
    y += row_h;
    create_param_row(scr, "DIA", &g_ui.dia_spin, 50, 84,
                     (int32_t)mpm_config_get_nibp_dia(store), on_nibp_changed, y);
    y += row_h;

    g_ui.map_label = lv_label_create(scr);
    lv_obj_set_style_text_color(g_ui.map_label, lv_color_hex(0x5E5E5E), 0);
    lv_obj_set_pos(g_ui.map_label, 8, y);
    update_map_label();

    if (g_ui.group != NULL) {
        lv_group_focus_obj(g_ui.hr_spin);
    }
}

int config_ui_run(MpmDeviceConfigStore* store)
{
    if (store == NULL) {
        return 1;
    }

    lv_init();
    lv_display_t* display = lv_sdl_window_create(320, 240);
    if (display == NULL) {
        fprintf(stderr, "[device-ui] failed to create SDL window\n");
        return 1;
    }

    lv_sdl_window_set_title(display, "MiniPatientMonitor Device Config");

    lv_indev_t* mouse = lv_sdl_mouse_create();
    lv_indev_t* keyboard = lv_sdl_keyboard_create();
    if (mouse != NULL) {
        lv_indev_set_display(mouse, display);
    }
    if (keyboard != NULL) {
        lv_indev_set_display(keyboard, display);
        g_ui.group = lv_group_create();
        lv_group_set_default(g_ui.group);
        lv_indev_set_group(keyboard, g_ui.group);
    }

    build_ui(store);

    while (1) {
        lv_timer_handler();
        lv_delay_ms(5);
    }

    return 0;
}