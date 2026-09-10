#ifndef MANAGERS_MCUMANAGER_H_
#define MANAGERS_MCUMANAGER_H_

#include <mcuProtocol/mcuProtoParse.h>
#include <string>
#include <utility>
#include <mutex>

class McuManager {
public:
    static McuManager& getInstance() {
        static McuManager instance;
        return instance;
    }
    
    McuManager(const McuManager&) = delete;
    McuManager& operator=(const McuManager&) = delete;
    
    /**
     * @brief Initialize the MCU instance
     * @param mcu Pointer to the MCU communication instance
     */
    void initialize(PixelMcuProto::McuParse* mcu);
    
    /**
     * @brief Check whether it has been initialized
     */
    bool isInitialized() const { return mMcu != nullptr; }
    

    void setMicValue(int value);
    /**
     * @brief Query the microphone value
     * @return The microphone value
     */
    int queryMicValue();
    
    void setUsbState(int state);
    /**
     * @brief Query the USB connection state
     * @return The USB state
     */
    int queryUsbState();
    
    void setBatteryState(const std::pair<int, int>& state);
    /**
     * @brief Query the battery level
     * @return pair<battery percentage, charging state> (0-100, 0=not charging/1=charging)
     */
    std::pair<int, int> queryBatteryPower();
    
    /**
     * @brief Set whether the microphone reports automatically
     */
    void setAutoMicReport(bool sw);
    /**
     * @brief Power off
     */
    void powerOff();
    
    /**
     * @brief Query the MCU version
     * @param mcuVer Output parameter, the MCU version string
     * @return 0=success, anything else=failure
     */
    int queryMcuVersion(std::string& mcuVer);
    
private:
    McuManager() : mMcu(nullptr), mMicValue(-1), mUsbState(-1), mBatterState({-1, -1}) {}
    ~McuManager() {}
    
    PixelMcuProto::McuParse* mMcu;
    int mMicValue;
    int mUsbState;
    std::pair<int, int> mBatterState;

    mutable std::mutex mMutex;
};

#endif
