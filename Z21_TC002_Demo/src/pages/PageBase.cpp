#include "pages/PageBase.h"
#include <base/base.h>
Mutex PageBase::mSpiMutex;

PageBase::~PageBase() {

}

void PageBase::sendLedData(const std::vector<uint8_t>& rgbData) {
    Mutex::Autolock l_(mSpiMutex);
    //SPI initialization
    static SpiHelper spi(0, SPI_MODE_0, 10 * 1000 * 1000, 8, false);
    //64 * 16 * 3 bytes have to be sent
    uint8_t* copyData = new uint8_t[64 * 16 * 3]();
    for(int y = 0; y < 16; y++) {
        for(int x = 0; x < 52 * 3; x++) {
            copyData[y * 64 * 3 + x] = rgbData[y * 52 * 3 + x];
        }
    }
    const int maxSz = 64 * 16 * 3;
    //GPIO_35 must be held high for one second before sending, so the MCU can detect it reliably
    GpioHelper::output("GPIO_35", 0);
    usleep(1 * 1000);
    if(!spi.write(copyData, maxSz)) {
        LOGE_TRACE("spi write error");
    }
    //Remember to pull it low again once sending is done
    GpioHelper::output("GPIO_35", 1);
    delete[] copyData;

    //Do not send too fast; never below 15 ms/frame, otherwise sending fails
    usleep(15 * 1000);
}
