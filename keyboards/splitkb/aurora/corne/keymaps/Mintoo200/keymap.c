#include "keycodes.h"
#include QMK_KEYBOARD_H
#include "config.h"

#define LOCK_SCREEN_WINDOWS LGUI(KC_L)
#define LOCK_SCREEN_MAC LCTL(LGUI(KC_Q))

// TODO:
//  - [] Customize OLED
//  - [] Customize default backlight
//  - [] 1 handed layer
//  - [] Clean LEDs default animations
//  - [] Display more layers on OLED
//  - [] Print something to dempen the orange LED on the Liatris
//  - [] configure Git
//  - [] Add modifier where expected and possible on mod layers
//  - [] 

enum layers_and_mods {
    // base layers
    QWERTY,
    GAMING,

    // mod layers
    NUMBERS,
    SYMBOLS,
    NUMPAD,

    // single handed
    SH_LEFT,
    SH_RIGHT,
    SH_NUMBERS,

    // mods
    CAPSLOCK
};

//#region Macros

#define TOGGLE_LAYER(layer1, layer2) if (IS_LAYER_ON(layer1)) { layer_move(layer2); } else { layer_move(layer1); }

enum custom_keycodes {
    ARROW_FN = SAFE_RANGE,
    TOGGLE_GAMING_LAYER,
    TOGGLE_SINGLE_HANDED_LAYER
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    switch (keycode) {
    case ARROW_FN:
        if (record->event.pressed) {
            SEND_STRING("() => {}");
        }
        break;
    case TOGGLE_GAMING_LAYER:
        if (record->event.pressed) {
            TOGGLE_LAYER(GAMING, QWERTY)
        }
        break;
    case TOGGLE_SINGLE_HANDED_LAYER:
        if (record->event.pressed) {
            TOGGLE_LAYER(SH_LEFT, QWERTY)
        }
        break;
    }
    return true;
};

//#endregion

//#region Combos

const uint16_t PROGMEM switch_to_gaming[] = {KC_Q, KC_W, KC_E, KC_R, COMBO_END};
const uint16_t PROGMEM toggle_single_handed[] = {KC_W, KC_E, KC_R, KC_T, COMBO_END};
const uint16_t PROGMEM lock_screen[] = {KC_T, KC_Y, COMBO_END};

#define LOCK_SCREEN_INDEX 1

combo_t key_combos[] = {
    COMBO(switch_to_gaming, TOGGLE_GAMING_LAYER),
    [LOCK_SCREEN_INDEX] = COMBO(lock_screen, LOCK_SCREEN_MAC),
    COMBO(toggle_single_handed, TOGGLE_SINGLE_HANDED_LAYER)
};
bool process_detected_host_os_kb(os_variant_t detected_os) {
    if (!process_detected_host_os_user(detected_os)) {
        return false;
    }
    switch (detected_os) {
        case OS_MACOS:
            key_combos[LOCK_SCREEN_INDEX].keycode = LOCK_SCREEN_MAC;
            break;
        case OS_WINDOWS:
            key_combos[LOCK_SCREEN_INDEX].keycode = LOCK_SCREEN_WINDOWS;
            break;
        default:
            break;
    }
    
    return true;
};

//#endregion

//#region Backlight

#define LED_LEFT(index)  index
#define LED_RIGHT(index)  index + 27

#ifdef RGBLIGHT_LAYERS
#define INDICATOR_LED(color) RGBLIGHT_LAYER_SEGMENTS({LED_LEFT(5), 1, color}, {LED_RIGHT(5), 1, color})

const rgblight_segment_t PROGMEM capslock_layer[] = INDICATOR_LED(HSV_RED);
const rgblight_segment_t PROGMEM numpad_layer[] = INDICATOR_LED(HSV_GREEN);
const rgblight_segment_t PROGMEM gaming_layer[] = INDICATOR_LED(HSV_CYAN);
const rgblight_segment_t PROGMEM single_handed_layer[] = INDICATOR_LED(HSV_WHITE);
const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(
    [NUMPAD] = numpad_layer,
    [GAMING] = gaming_layer,
    [CAPSLOCK] = capslock_layer,
    [SH_LEFT] = single_handed_layer,
    [SH_RIGHT] = single_handed_layer
);
void keyboard_post_init_user(void) {
    rgblight_layers = my_rgb_layers;
};
bool led_update_user(led_t led_state) {
    rgblight_set_layer_state(CAPSLOCK, led_state.caps_lock);
    return true;
};
layer_state_t layer_state_set_user(layer_state_t state) {
    rgblight_set_layer_state(NUMPAD, layer_state_cmp(state, NUMPAD));
    rgblight_set_layer_state(GAMING, layer_state_cmp(state, GAMING));
    rgblight_set_layer_state(SH_LEFT, layer_state_cmp(state, SH_LEFT));
    rgblight_set_layer_state(SH_RIGHT, layer_state_cmp(state, SH_RIGHT));
    return state;
};
#endif

//#endregion

//#region Keymap

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
	[QWERTY] = LAYOUT_split_3x6_3(
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T,                   KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        KC_TAB,LSFT_T(KC_A),KC_S,KC_D, LT(SYMBOLS,KC_F),KC_G,   KC_H, LT(SYMBOLS,KC_J), KC_K, KC_L, RSFT_T(KC_SCLN), KC_QUOT, 
        LSFT_T(KC_CAPS), KC_Z, KC_X, KC_C, KC_V, KC_B,          KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, SC_SENT,
                            KC_LCTL, KC_LGUI, KC_LALT,          KC_MPLY, KC_SPC, MO(NUMBERS)),
                            
    [SH_LEFT] = LAYOUT_split_3x6_3(
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T,                                           KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC,
        KC_TAB, RSFT_T(KC_A), RALT_T(KC_S), RGUI_T(KC_D), RCTL_T(KC_F), KC_G,           KC_NO, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_SLSH,
        KC_LSFT, RCTL_T(KC_Z), RGUI_T(KC_X), RALT_T(KC_C), RSFT_T(KC_V), KC_B,          KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_TRNS,
                            KC_SPC, LT(SH_RIGHT, KC_SPC), MO(SH_NUMBERS),                              KC_ENT, KC_TRNS, KC_TRNS),
	[SH_RIGHT] = LAYOUT_split_3x6_3(
        KC_BSPC, KC_P, KC_O, KC_I, KC_U, KC_Y,                                          KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC,
        KC_QUOT, LSFT_T(KC_SCLN), LALT_T(KC_L), LGUI_T(KC_K), LCTL_T(KC_J), KC_H,       KC_NO, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_SLSH,
        SC_SENT, RCTL_T(KC_SLSH), RGUI_T(KC_DOT), RALT_T(KC_COMM), RSFT_T(KC_M), KC_N,  KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_TRNS,
                            KC_NO, KC_NO, KC_NO,                                        KC_ENT, KC_TRNS, KC_TRNS),
	[SH_NUMBERS] = LAYOUT_split_3x6_3(
        KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5,                                           KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC,
        KC_QUOT, KC_6, KC_7, KC_8, KC_9, KC_0,                                          KC_NO, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_SLSH,
        SC_SENT, RCTL_T(KC_SLSH), RGUI_T(KC_DOT), RALT_T(KC_COMM), RSFT_T(KC_M), KC_N,  KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_TRNS,
                            KC_NO, KC_NO, MO(SH_NUMBERS),                               KC_ENT, KC_TRNS, KC_TRNS),

	[NUMBERS] = LAYOUT_split_3x6_3(
        KC_GRV, KC_1, KC_2, KC_3, KC_4, KC_5,                   KC_6, KC_7, KC_8, KC_9, KC_0, KC_BSPC,
        KC_TRNS, KC_NO, KC_NO, KC_NO, TG(NUMPAD), KC_NO,        KC_NO, KC_LEFT, KC_DOWN, KC_UP, KC_RGHT, KC_SLSH,
        KC_TRNS, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,             KC_NO, KC_NO, KC_LEFT, KC_DOWN, KC_RGHT, KC_TRNS,
                            KC_TRNS, KC_TRNS, KC_TRNS,          KC_ENT, KC_TRNS, KC_TRNS),
	[SYMBOLS] = LAYOUT_split_3x6_3(
        KC_TRNS, KC_NO, KC_NO, KC_LBRC, KC_RBRC, KC_NO,         KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_DEL,
        KC_TRNS, KC_NO, ARROW_FN, KC_LPRN, KC_RPRN, KC_NO,      KC_MINS, KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS, KC_BSLS,
        KC_TRNS, KC_NO, KC_NO, KC_LCBR, KC_RCBR, KC_NO,         KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_PIPE,
                            KC_TRNS, KC_TRNS, KC_TRNS,          KC_ENT, KC_TRNS, TG(NUMPAD)),
	[NUMPAD] = LAYOUT_split_3x6_3(
        KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO,               KC_PMNS, KC_P7, KC_P8, KC_P9, KC_PPLS, KC_BSPC,
        RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, KC_NO, KC_NO,       KC_PSLS, KC_P4, KC_P5, KC_P6, KC_PCMM, KC_PDOT,
        RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, KC_NO, KC_NO,       KC_PAST, KC_P1, KC_P2, KC_P3, KC_PEQL, KC_PENT,
                            KC_TRNS, KC_TRNS, KC_TRNS,          KC_ENT, KC_P0, TG(NUMPAD)),
	[GAMING] = LAYOUT_split_3x6_3(
        KC_ESC, KC_Q, KC_W, KC_E, KC_R, KC_T,                   KC_Y, KC_U, KC_I, KC_O, KC_P, KC_BSPC,
        KC_TAB, KC_A, KC_S, KC_D, KC_F, KC_G,                   KC_H, KC_J, KC_K, KC_L, KC_SCLN, KC_QUOT,
        KC_LSFT, KC_Z, KC_X, KC_C, KC_V, KC_B,                  KC_N, KC_M, KC_COMM, KC_DOT, KC_SLSH, KC_ENT, 
                            KC_LCTL, KC_SPC, KC_LALT,           KC_MPLY, KC_SPC, MO(NUMBERS))
};

#if defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)
#define ENCODER(anticlockwise, clockwise)  { ENCODER_CCW_CW(anticlockwise, clockwise), ENCODER_CCW_CW(anticlockwise, clockwise)  }

const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [QWERTY] = ENCODER(KC_VOLD, KC_VOLU),
    [NUMBERS] = ENCODER(KC_VOLD, KC_VOLU),
    [SYMBOLS] = ENCODER(KC_VOLD, KC_VOLU),
    [NUMPAD] = ENCODER(KC_VOLD, KC_VOLU),
    [GAMING] = ENCODER(KC_VOLD, KC_VOLU),
    [SH_LEFT] = ENCODER(KC_VOLD, KC_VOLU),
    [SH_RIGHT] = ENCODER(KC_VOLD, KC_VOLU),
    [SH_NUMBERS] = ENCODER(KC_VOLD, KC_VOLU),
};
#endif // defined(ENCODER_ENABLE) && defined(ENCODER_MAP_ENABLE)

//#endregion
