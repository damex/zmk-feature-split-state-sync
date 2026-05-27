// Copyright 2026 Roman Kuzmitskii (@damex)
// SPDX-License-Identifier: MIT

/*
 * Watches a peer's input stream and pushes a list of bindings to it on every
 * (re)connect, detected as the first input after a long silence. For
 * transports without a connect event (e.g. ESB).
 */
#define DT_DRV_COMPAT zmk_split_state_sync

#include <zephyr/device.h>
#include <zephyr/init.h>
#include <zephyr/input/input.h>
#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <drivers/behavior.h>
#include <zmk/behavior.h>
#include <zmk/keymap.h>

LOG_MODULE_REGISTER(split_state_sync, CONFIG_ZMK_SPLIT_STATE_SYNC_LOG_LEVEL);

#define STATE_SYNC_NODE DT_DRV_INST(0)
#define STATE_SYNC_SOURCE DT_INST_PROP(0, source)

static const struct zmk_behavior_binding state_sync_bindings[] = {
    LISTIFY(DT_INST_PROP_LEN(0, bindings), ZMK_KEYMAP_EXTRACT_BINDING, (, ), STATE_SYNC_NODE)};

static int64_t state_sync_last_activity_ms;
static bool state_sync_done;
static struct k_work_delayable state_sync_work;

/* source is set so EVENT_SOURCE routing reaches the right peripheral. */
static void state_sync_push(void) {
    const struct zmk_behavior_binding_event event = {
        .source = STATE_SYNC_SOURCE,
        .timestamp = k_uptime_get(),
    };
    for (size_t i = 0; i < ARRAY_SIZE(state_sync_bindings); i++) {
        int error = zmk_behavior_invoke_binding(&state_sync_bindings[i], event, true);
        if (error) {
            LOG_DBG("invoke binding %u failed (%d)", (unsigned int)i, error);
        }
    }
}

static void state_sync_work_handler(struct k_work *work) {
    ARG_UNUSED(work);
    state_sync_push();
}

/* Long silence re-arms the one-shot; the next input fires it. */
static void state_sync_input_cb(struct input_event *event, void *user_data) {
    ARG_UNUSED(event);
    ARG_UNUSED(user_data);
    const int64_t now = k_uptime_get();
    if ((now - state_sync_last_activity_ms) >= CONFIG_ZMK_SPLIT_STATE_SYNC_RECONNECT_TIMEOUT_MS) {
        state_sync_done = false;
    }
    if (!state_sync_done) {
        k_work_reschedule(&state_sync_work, K_NO_WAIT);
        state_sync_done = true;
    }
    state_sync_last_activity_ms = now;
}

INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(DT_INST_PHANDLE(0, input)), state_sync_input_cb, NULL);

static int state_sync_init(void) {
    k_work_init_delayable(&state_sync_work, state_sync_work_handler);
    return 0;
}

SYS_INIT(state_sync_init, APPLICATION, CONFIG_KERNEL_INIT_PRIORITY_DEFAULT);
