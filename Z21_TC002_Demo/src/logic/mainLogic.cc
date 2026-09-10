#include "uart/ProtocolSender.h"
#include "base/base.h"
#include "base/json_object.h"
#include <base/wifi.h>
#include "utils/SpiHelper.h"
#include "utils/GpioHelper.h"
#include "managers/KeyManager.h"
#include "managers/PageManager.h"
#include "managers/McuManager.h"
#include "pages/RgbTestPage.h"
#include "pages/BtnTestPage.h"
#include "pages/AudioTestPage.h"
#include "pages/WifiTestPage.h"
#include "ble/bluetooth_service.h"
#include "net/NetManager.h"
#include <thread>
#include <os/SystemProperties.h>
#define WIFIMANAGER NETMANAGER->getWifiManager()
#define CONFIG_PATH "/mnt/usb1/test.cfg"
namespace {

void loadConfigAndStart() {
	if (!base::wifiOnAndWait(10)) {
		LOGE_TRACE("mainLogic: WiFi failed to start");
	}
	std::string ssid, pwd, bleName;
	bool needConnect = false;

	if (base::exists(CONFIG_PATH)) {
		try {
			base::JSONObject config = base::JSONObject::parseFromFile(CONFIG_PATH);
			ssid = config.getString("ssid", "");
			pwd = config.getString("pwd", "");
			needConnect = config.getBool("isConnect", false);
			bleName = config.getString("ble", "");
			LOGI_TRACE("mainLogic: config ssid=%s connect=%d ble=%s",
				ssid.c_str(), needConnect, bleName.c_str());
		} catch (...) {
			LOGE_TRACE("mainLogic: failed to parse %s", CONFIG_PATH);
		}
	} else {
		LOGW_TRACE("mainLogic: config not found: %s", CONFIG_PATH);
	}
	if (bleName.empty()) {
		std::string mac = base::wifiMacAddress();
		if (mac.size() >= 4) {
			bleName = "Ulanzi TC002 " + mac.substr(mac.size() - 4);
		} else {
			bleName = "Ulanzi TC002";
		}
	}
	BluetoothParams bleParams;
	bleParams.name = bleName;
	BluetoothService::instance().start(bleParams);
	auto* page = static_cast<WifiTestPage*>(
		PageManager::getInstance().getPage("WifiTestPage"));
	if (page) {
		page->setWifiParams(ssid, pwd, needConnect);
	}
}

}

static S_ACTIVITY_TIMEER REGISTER_ACTIVITY_TIMER_TAB[] = {

};

/**
 * Fired when the screen is constructed
 */
static void onUI_init(){
	//Anti-brick property
	SystemProperties::setString("sys.zkapp.state", "running");
	//Initialize the serial port
	McuManager::getInstance().initialize(new PixelMcuProto::McuParse("/dev/ttyS1", 1500000));

	//The version must be requested once at every boot before anything can be displayed
	std::string mcuVer;
	McuManager::getInstance().queryMcuVersion(mcuVer);
	LOGI_TRACE("mcuVer : [%s]", mcuVer.c_str());

	PageManager::getInstance().registerPage(std::unique_ptr<PageBase>(new BtnTestPage()));
	PageManager::getInstance().registerPage(std::unique_ptr<PageBase>(new RgbTestPage()));
	PageManager::getInstance().registerPage(std::unique_ptr<PageBase>(new AudioTestPage()));
	PageManager::getInstance().registerPage(std::unique_ptr<PageBase>(new WifiTestPage()));

	KeyManager::getInstance().start();
    loadConfigAndStart();
    EASYUICONTEXT->openActivity("btnTestActivity", nullptr);
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
}

/*
 * Fired when the screen is hidden
 */
static void onUI_hide() {

}

/*
 * Fired when the screen has fully exited
 */
static void onUI_quit() {

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
static bool onmainActivityTouchEvent(const MotionEvent &ev) {
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
