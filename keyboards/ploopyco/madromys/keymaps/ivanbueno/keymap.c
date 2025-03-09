/* Copyright 2023 Colin Lam (Ploopy Corporation)
 * Copyright 2020 Christopher Courtney, aka Drashna Jael're  (@drashna) <drashna@live.com>
 * Copyright 2019 Sunjun Kim
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

extern bool is_drag_scroll;

// Tap Dance declarations
enum {
    TD_MIDDLE_CLICK,
    TD_COPYPASTE,
    TD_CUTPASTE,
    TD_DRAGSCROLL,
};

// Define a type containing as many tapdance states as you need
typedef enum {
    TD_NONE,
    TD_UNKNOWN,
    TD_COPYPASTE_SINGLE_TAP,
    TD_COPYPASTE_SINGLE_HOLD,
    TD_CUTPASTE_SINGLE_TAP,
    TD_CUTPASTE_SINGLE_HOLD,
    TD_DRAGSCROLL_SINGLE_TAP,
    TD_DRAGSCROLL_SINGLE_HOLD,
} td_state_t;

// Create a global instance of the tapdance state type
static td_state_t td_state;

// Function to determine the current tapdance state
td_state_t cur_dance(tap_dance_state_t *state, int key);

// `finished` and `reset` functions for each tapdance keycode
void copypaste_finished(tap_dance_state_t *state, void *user_data);
void copypaste_reset(tap_dance_state_t *state, void *user_data);
void cutpaste_finished(tap_dance_state_t *state, void *user_data);
void cutpaste_reset(tap_dance_state_t *state, void *user_data);
void dragscroll_finished(tap_dance_state_t *state, void *user_data);
void dragscroll_reset(tap_dance_state_t *state, void *user_data);

void tdfn_middle_click(tap_dance_state_t *state, void *user_data) {
    switch (state->count) {
        case 1:
            register_code(KC_BTN3); unregister_code(KC_BTN3);
            break;
        case 2:
            register_code(KC_LGUI); register_code(KC_LSFT); register_code(KC_T); unregister_code(KC_T); unregister_code(KC_LGUI); unregister_code(KC_LSFT);
            break;
    }
}

// Tap Dance definitions
tap_dance_action_t tap_dance_actions[] = {
    [TD_MIDDLE_CLICK] = ACTION_TAP_DANCE_FN(tdfn_middle_click),
    [TD_COPYPASTE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, copypaste_finished, copypaste_reset),
    [TD_CUTPASTE] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, cutpaste_finished, cutpaste_reset),
    [TD_DRAGSCROLL] = ACTION_TAP_DANCE_FN_ADVANCED(NULL, dragscroll_finished, dragscroll_reset),

};

enum custom_keycodes {
    MACRO_CTRL_SHIFT_T = SAFE_RANGE,
    MACRO_CTRL_C,
    MACRO_CTRL_V,
    MACRO_CTRL_1,
    MACRO_CTRL_2,
    MACRO_CTRL_3,
    MACRO_DRAGSCROLL_HOLD,
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
        case MACRO_CTRL_SHIFT_T:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI(SS_LSFT("t")));
            }
            break;
        case MACRO_CTRL_C:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI("c"));
            }
            return false;
            break;
        case MACRO_CTRL_V:
            if (record->event.pressed) {
                SEND_STRING(SS_LGUI("v"));
            }
            return false;
            break;
        case MACRO_CTRL_1:
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL("1"));
            }
            return false;
            break;
        case MACRO_CTRL_2:
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL("2"));
            }
            return false;
            break;
        case MACRO_CTRL_3:
            if (record->event.pressed) {
                SEND_STRING(SS_LCTL("3"));
            }
            return false;
            break;
        case MACRO_DRAGSCROLL_HOLD:
            is_drag_scroll = record->event.pressed;
            return false;
            break;
    }
    return true;
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0] = LAYOUT(KC_BTN1, TD(TD_MIDDLE_CLICK), LT(2,KC_BTN2), LT(1,KC_BTN4), TD(TD_DRAGSCROLL), KC_ESC),
    [1] = LAYOUT(MACRO_CTRL_1, MACRO_CTRL_2, MACRO_CTRL_3, KC_TRNS, KC_TRNS, DPI_CONFIG),
    [2] = LAYOUT(TD(TD_COPYPASTE), TD(TD_CUTPASTE), KC_TRNS, KC_BTN5, KC_TRNS, KC_TRNS),
    // [3] = LAYOUT(MACRO_CTRL_SHIFT_T, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS),
};

// Handle the possible states for each tapdance keycode you define:
// Determine the tapdance state to return
td_state_t cur_dance(tap_dance_state_t *state, int key) {
    switch (key) {
        case TD_COPYPASTE:
            if (state->count == 1) {
                if (state->interrupted || !state->pressed) return TD_COPYPASTE_SINGLE_TAP;
                else return TD_COPYPASTE_SINGLE_HOLD;
            }
            else return TD_UNKNOWN;
            break;
        case TD_CUTPASTE:
            if (state->count == 1) {
                if (state->interrupted || !state->pressed) return TD_CUTPASTE_SINGLE_TAP;
                else return TD_CUTPASTE_SINGLE_HOLD;
            }
            else return TD_UNKNOWN;
            break;
        case TD_DRAGSCROLL:
            if (state->count == 1) {
                if (state->interrupted || !state->pressed) return TD_DRAGSCROLL_SINGLE_TAP;
                else return TD_DRAGSCROLL_SINGLE_HOLD;
            }
            else return TD_UNKNOWN;
            break;
        default:
            break;
    }
    return TD_UNKNOWN;
}

void copypaste_finished(tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state, TD_COPYPASTE);
    switch (td_state) {
        case TD_COPYPASTE_SINGLE_TAP:
            register_code(KC_LGUI); register_code(KC_V);
            break;
        case TD_COPYPASTE_SINGLE_HOLD:
            register_code(KC_LGUI); register_code(KC_C);
            break;
        default:
            break;
    }
}

void copypaste_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case TD_COPYPASTE_SINGLE_TAP:
            unregister_code(KC_LGUI); unregister_code(KC_V);
            break;
        case TD_COPYPASTE_SINGLE_HOLD:
            unregister_code(KC_LGUI); unregister_code(KC_C);
            break;
        default:
            break;
    }
}

void cutpaste_finished(tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state, TD_CUTPASTE);
    switch (td_state) {
        case TD_CUTPASTE_SINGLE_TAP:
            register_code(KC_LGUI); register_code(KC_V);
            break;
        case TD_CUTPASTE_SINGLE_HOLD:
            register_code(KC_LGUI); register_code(KC_X);
            break;
        default:
            break;
    }
}

void cutpaste_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case TD_CUTPASTE_SINGLE_TAP:
            unregister_code(KC_LGUI); unregister_code(KC_V);
            break;
        case TD_CUTPASTE_SINGLE_HOLD:
            unregister_code(KC_LGUI); unregister_code(KC_X);
            break;
        default:
            break;
    }
}


void dragscroll_finished(tap_dance_state_t *state, void *user_data) {
    td_state = cur_dance(state, TD_DRAGSCROLL);
    switch (td_state) {
        case TD_DRAGSCROLL_SINGLE_TAP:
            is_drag_scroll ^= 1;
            break;
        case TD_DRAGSCROLL_SINGLE_HOLD:
            is_drag_scroll = true;
            break;
        default:
            break;
    }
}

void dragscroll_reset(tap_dance_state_t *state, void *user_data) {
    switch (td_state) {
        case TD_DRAGSCROLL_SINGLE_TAP:
            break;
        case TD_DRAGSCROLL_SINGLE_HOLD:
            is_drag_scroll = false;
            break;
        default:
            break;
    }
}