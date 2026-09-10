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
		EASYUICONTEXT->openActivity("btnTestActivity", nullptr);
		break;
	case E_KEYCODE_ANTI_CLOCKWISE:
		EASYUICONTEXT->openActivity("wifiTestActivity", nullptr);
		break;
	default:
		break;
	}
}
#define TIMER_MIC_REFRESH 1
#define TIMER_MIC_REFRESH_TIME 33

}
static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {
	{TIMER_MIC_REFRESH, TIMER_MIC_REFRESH_TIME}
};

/**
 * Fired when the screen is constructed
 */
static void onUI_init(){

}

/**
 * Fired when switching to this screen
 */
static void onUI_intent(const Intent *intentPtr) {
    if (intentPtr != NULL) {
        //TODO
    }
}

/*
 * Fired when the screen is shown
 */
static void onUI_show() {
	KeyManager::getInstance().addKeyEventCallback(keyEventCb);
	PageManager::getInstance().navigateTo("AudioTestPage");
}

/*
 * Fired when the screen is hidden
 */
static void onUI_hide() {
	KeyManager::getInstance().removeKeyEventCallback(keyEventCb);
}

/*
 * Fired when the screen has fully exited
 */
static void onUI_quit() {
	KeyManager::getInstance().removeKeyEventCallback(keyEventCb);
}

/**
 * Serial data callback interface
 */
static void onProtocolDataUpdate(const SProtocolData &data) {

}

/**
 * Timer callback function
 * Avoid time-consuming work in this function, as it affects the UI refresh
 * Parameter: id
 *         The id of the timer that fired, the same id used at registration
 * Return value: true
 *             keep the current timer running
 *         false
 *             stop the current timer
 */
static bool onUI_Timer(int id){
	switch (id) {
		case TIMER_MIC_REFRESH:
			PageManager::getInstance().drawCurrentPage();
            break;
        default:
            break;
	}
    return true;
}

/**
 * Fired when there is a new touch event
 * Parameter: ev
 *         the new touch event
 * Return value: true
 *            the touch event is intercepted here and is no longer passed on to the widgets
 *         false
 *            the touch event is passed on to the widgets
 */
static bool onaudioTestActivityTouchEvent(const MotionEvent &ev) {
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
