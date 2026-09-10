/*
 * EventContext.h
 *
 *  Created on: 29 April 2022
 *      Author: guoxs
 */

#ifndef _EVENT_CONTEXT_H_
#define _EVENT_CONTEXT_H_

typedef void (*on_key_event_cb)(int keyCode, int keyStatus);

enum EKeyCode {
	E_KEYCODE_CLOCKWISE,        // Clockwise
	E_KEYCODE_ANTI_CLOCKWISE,   // Counter-clockwise
	E_KEYCODE_KNOB_BUTTON = 0x67,   // Knob button
	E_KEYCODE_LEFT_BUTTON = 0x6C,   // Left button
	E_KEYCODE_MIDDLE_BUTTON = 0x69, // Middle button
	E_KEYCODE_RIGHT_BUTTON = 0x6A,  // Right button
};

namespace event {

bool start();

void stop();

void add_key_event_cb(on_key_event_cb cb);

void remove_key_event_cb(on_key_event_cb cb);

}

#endif /* _EVENT_CONTEXT_H_ */
