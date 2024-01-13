// Copyright 2022 Anton Chernenko (@ch3rny)
// SPDX-License-Identifier: GPL-2.0-or-later
#include QMK_KEYBOARD_H
#include "bongocat.c"
#include "logos.c"

enum layers {
  BASE,
  LOWER,
  RAISE,
  ADJUST
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [BASE] = LAYOUT(
       KC_ESC,  KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,    KC_Y,    KC_U,    KC_I,    KC_O,    KC_P,    KC_LBRC, KC_RBRC, KC_BSLS, KC_DEL, 
       KC_TAB,  KC_A,    KC_S,    KC_D,    KC_F,    KC_G,    KC_H,    KC_J,    KC_K,    KC_L,    KC_SCLN, KC_QUOT, KC_ENT,           KC_PGUP, 
       KC_LSFT, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_N,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_RSFT,          KC_UP,   KC_PGDN,
       KC_LCTL, KC_LGUI, KC_LALT, LT(LOWER, KC_BSPC),   LT(ADJUST, KC_ENT),    LT(RAISE, KC_SPC),KC_RALT, KC_RCTL, KC_LEFT, KC_DOWN, KC_RGHT
    ),

    [LOWER] = LAYOUT(
       KC_GRV,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_UNDS, KC_PPLS, _______, KC_HOME,
       _______, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_PMNS, KC_EQL,           KC_END,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
       _______, _______, _______, _______,          _______,                   _______,          _______, _______, _______, _______, _______
    ),

    [RAISE] = LAYOUT(
       _______, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_F11,  KC_F12,  KC_PSCR, _______,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
       _______, _______, _______, _______,          _______,                   _______,          _______, _______, _______, _______, _______
    ),

    [ADJUST] = LAYOUT(
       QK_BOOT, KC_MUTE, KC_VOLD, KC_VOLU, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,
       _______, KC_MPLY, KC_MPRV, KC_MNXT, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______,
       _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______, _______,          _______, _______,
       _______, _______, _______, _______,          _______,                   _______,          _______, _______, _______, _______, _______
    ),
};
   
typedef union {
    uint32_t raw;
    struct {
        uint8_t led_mode : 3;
        uint8_t oled_mode : 3;
        uint8_t oled_brightness : 8;
        uint8_t led_status : 3;
    };
} rosalind_config_t;

rosalind_config_t rosalind_config;

void update_status_led(led_t led_state) {
    bool state = false;

    switch (rosalind_config.led_status) {
        case 0:
            state = led_state.caps_lock;
            break;
        case 1:
            state = led_state.num_lock;
            break;
        case 2:
            state = led_state.scroll_lock;
            break;
        case 3:
            state = led_state.kana;
            break;
        case 4:
            state = led_state.compose;
            break;
    }

    writePin(LED, state);
}

void update_led_state(void) {
    if (rosalind_config.led_mode == 1) {
        update_status_led(host_keyboard_led_state());
        return;
    }
    writePin(LED, rosalind_config.led_mode == 3);
}



void eeconfig_init_user(void) {
    rosalind_config.raw = 0;
    rosalind_config.led_mode = 2;
    rosalind_config.oled_mode = 1;
    rosalind_config.oled_brightness = 255;
    rosalind_config.led_status = 0;
    eeconfig_update_user(rosalind_config.raw);
}

void keyboard_post_init_user(void) {
    rosalind_config.raw = eeconfig_read_user();
    update_led_state();
    oled_set_brightness(rosalind_config.oled_brightness);
}

enum via_led_value {
    id_led_mode = 1,
    id_oled_mode = 2,
    id_oled_brightness = 3,
    id_led_status = 4
};

void rosalind_config_set_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_led_mode:
            rosalind_config.led_mode = (uint8_t)*value_data;
            update_led_state();
            break;
        case id_oled_mode:
            rosalind_config.oled_mode = (uint8_t)*value_data;
            break;
        case id_oled_brightness:
            rosalind_config.oled_brightness = (uint8_t)*value_data;
            oled_set_brightness((uint8_t)*value_data);
            break;
        case id_led_status:
            rosalind_config.led_status = (uint8_t)*value_data;
            update_led_state();
            break;
    }
}

void rosalind_config_get_value(uint8_t *data) {
    // data = [ value_id, value_data ]
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);

    switch (*value_id) {
        case id_led_mode:
            *value_data = rosalind_config.led_mode;
            break; 
        case id_oled_mode:
            *value_data = rosalind_config.oled_mode;
            break;
        case id_oled_brightness:
            *value_data = rosalind_config.oled_brightness;
            break;
        case id_led_status:
            *value_data = rosalind_config.led_status;
            break;
    }
}

void rosalind_config_save(void) {
    eeconfig_update_user(rosalind_config.raw);
}

void via_custom_value_command_kb(uint8_t *data, uint8_t length) {
    uint8_t *command_id        = &(data[0]);
    uint8_t *channel_id        = &(data[1]);
    uint8_t *value_id_and_data = &(data[2]);

    if (*channel_id == id_custom_channel) {
        switch (*command_id) {
            case id_custom_set_value: {
                rosalind_config_set_value(value_id_and_data);
                break;
            }
            case id_custom_get_value: {
                rosalind_config_get_value(value_id_and_data);
                break;
            }
            case id_custom_save: {
                rosalind_config_save();
                break;
            }
            default: {
                // Unhandled message.
                *command_id = id_unhandled;
                break;
            }
        }
        return;
    }
    *command_id = id_unhandled;
}


bool led_update_kb(led_t led_state) {
    bool res = led_update_user(led_state);
    if (res) {
         if (rosalind_config.led_mode == 1) {
            update_status_led(led_state);
         }
    }
    return res;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    if (rosalind_config.led_mode == 2) {
        writePin(LED, state);
    }
    return layer_state_set_user(state);
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) { 
    if (rosalind_config.led_mode == 4) { 
        writePin(LED, record->event.pressed);
    }

    return true;
}

bool oled_task_kb(void) {
    if (!oled_task_user()) {
    	return false; 
    }

    if (rosalind_config.oled_mode != 1) {
        OLED_redraw = true;
    }

    if (rosalind_config.oled_mode == 0) {
        oled_off();
        return false;
    }

    if (rosalind_config.oled_mode == 1) { 
        draw_bongo();
        return false;
    }
    
    if (rosalind_config.oled_mode == 2) {
        oled_write_raw_P(rosalind_logo, sizeof(rosalind_logo));
        return false;
    }
    return false;
}
