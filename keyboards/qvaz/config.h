// Copyright 2023 Anton Chernenko (@Anton Chernenko)
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
//#define NO_DEBUG

/* disable print */
//#define NO_PRINT

/* disable action features */
//#define NO_ACTION_LAYER
//#define NO_ACTION_TAPPING
//#define NO_ACTION_ONESHOT
#define AUDIO_PIN C6
#define AUDIO_CLICKY
#define AUDIO_ENABLE_TONE_MULTIPLEXING
#define AUDIO_TONE_MULTIPLEXING_RATE_DEFAULT 10
