#pragma once
#include "uart/ProtocolSender.h"
#include "managers/KeyManager.h"
#include "managers/PageManager.h"

namespace {
void keyEventCb(int keyCode, int keyStatus) {
	if(!PageManager::getInstance().onKeyEvent(keyCode, keyStatus)) {
		return;
	}
	switch(keyCode) {
	case E_KEYCODE_CLOCKWISE:
		EASYUICONTEXT->openActivity("wifiTestActivity", nullptr);
		break;
	case E_KEYCODE_ANTI_CLOCKWISE:
		EASYUICONTEXT->openActivity("btnTestActivity", nullptr);
		break;
	default:
		break;
	}
}

}

static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {

};

static void onUI_init(){


}

static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {

    }
}

static void onUI_show() {
	KeyManager::getInstance().addKeyEventCallback(keyEventCb);
	PageManager::getInstance().navigateTo("RgbTestPage");
}

static void onUI_hide() {
	KeyManager::getInstance().removeKeyEventCallback(keyEventCb);
}

static void onUI_quit() {
	KeyManager::getInstance().removeKeyEventCallback(keyEventCb);
}

static void onProtocolDataUpdate(const SProtocolData &data) {

}

static bool onUI_Timer(int id){
	switch (id) {

		default:
			break;
	}
    return true;
}

static bool onrgbTestActivityTouchEvent(const MotionEvent &ev) {
    switch (ev.mActionStatus) {
		case MotionEvent::E_ACTION_DOWN://Touch down
			//LOGD("time = %ld coordinates x = %d, y = %d", ev.mEventTime, ev.mX, ev.mY);
			break;
		case MotionEvent::E_ACTION_MOVE://Touch move
			break;
		case MotionEvent::E_ACTION_UP:  //Touch up
			break;
		default:
			break;
	}
	return false;
}
