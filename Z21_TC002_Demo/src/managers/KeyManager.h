/*
 * KeyManager.cpp
 *
 *  Created on: 29 April 2022
 *      Author: guoxs
 */
#ifndef MANAGERS_KEYMANAGER_H_
#define MANAGERS_KEYMANAGER_H_

#include <stdint.h>

// Key event type
enum EKeyCode {
	E_KEYCODE_CLOCKWISE,        // Clockwise
	E_KEYCODE_ANTI_CLOCKWISE,   // Counter-clockwise
	E_KEYCODE_KNOB_BUTTON = 0x67,   // Knob button
	E_KEYCODE_LEFT_BUTTON = 0x6C,   // Left button
	E_KEYCODE_MIDDLE_BUTTON = 0x69, // Middle button
	E_KEYCODE_RIGHT_BUTTON = 0x6A,  // Right button
};

// Key event callback type
typedef void (*on_key_event_cb)(int keyCode, int keyStatus);

class KeyManager {
public:
	static KeyManager& getInstance();
	
	bool start();
	
	void stop();

	void addKeyEventCallback(on_key_event_cb cb);

	void removeKeyEventCallback(on_key_event_cb cb);
	
private:
	struct Impl;
	class EventThread;
	KeyManager();
	virtual ~KeyManager();
	
	bool eventThreadLoop();

	void notifyKeyEvent(int keyCode, int keyStatus);

	KeyManager(const KeyManager&) = delete;
	KeyManager& operator=(const KeyManager&) = delete;
	
	Impl* m_impl;
};

#endif
