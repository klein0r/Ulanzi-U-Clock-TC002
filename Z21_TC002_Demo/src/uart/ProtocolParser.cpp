/*
 * ProtocolParser.cpp
 *
 *  Created on: Sep 7, 2017
 */
#include <vector>
#include <string.h>
#include <system/Mutex.h>
#include "CommDef.h"
#include "uart/ProtocolParser.h"
#include "utils/Log.h"

static Mutex sLock;
static std::vector<OnProtocolDataUpdateFun> sProtocolDataUpdateListenerList;

void registerProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener) {
	Mutex::Autolock _l(sLock);
	LOGD("registerProtocolDataUpdateListener\n");
	if (pListener != NULL) {
		sProtocolDataUpdateListenerList.push_back(pListener);
	}
}

void unregisterProtocolDataUpdateListener(OnProtocolDataUpdateFun pListener) {
	Mutex::Autolock _l(sLock);
	LOGD("unregisterProtocolDataUpdateListener\n");
	if (pListener != NULL) {
		std::vector<OnProtocolDataUpdateFun>::iterator iter = sProtocolDataUpdateListenerList.begin();
		for (; iter != sProtocolDataUpdateListenerList.end(); iter++) {
			if ((*iter) == pListener) {
				sProtocolDataUpdateListenerList.erase(iter);
				return;
			}
		}
	}
}

static void notifyProtocolDataUpdate(const SProtocolData &data) {
	Mutex::Autolock _l(sLock);
	std::vector<OnProtocolDataUpdateFun>::const_iterator iter = sProtocolDataUpdateListenerList.begin();
	for (; iter != sProtocolDataUpdateListenerList.end(); iter++) {
		(*iter)(data);
	}
}

static SProtocolData sProtocolData;

SProtocolData& getProtocolData() {
	return sProtocolData;
}

/**
 * Get the checksum
 * Get checksum code
 */
BYTE getCheckSum(const BYTE *pData, int len) {
	int sum = 0;
	for (int i = 0; i < len; ++i) {
		sum += pData[i];
	}

	return (BYTE) (~sum + 1);
}

/**
 * Parse each frame of data
 * Parse each frame of data
 */
static void procParse(const BYTE *pData, UINT len) {
	// CmdID
	switch (MAKEWORD(pData[3], pData[2])) {
	case CMDID_POWER:
		sProtocolData.power = pData[5];
		break;
	}

	// Notify that the protocol data has been updated
  // Notify protocol data update
	notifyProtocolDataUpdate(sProtocolData);
}

/**
 * Purpose: parse the protocol
 * Function: Parse protocol
 * Parameters: pData the protocol data, len the data length
 * Parameters: pData - protocol data, len - data length
 * Return value: the length actually parsed
 * Return value: the length of the actual resolution protocol
 */
int parseProtocol(const BYTE *pData, UINT len) {
	UINT remainLen = len;	// Remaining data length
	UINT dataLen;	// Packet length
 	UINT frameLen;	// Frame length

	/**
	 * The following part has to be adapted to your protocol format to parse out each frame
   * The following parts need to be modified according to the protocol format to parse out the data of each frame
	 */
	while (remainLen >= DATA_PACKAGE_MIN_LEN) {
		// Found the header of a data frame
    // Find the data header of a frame of data
		while ((remainLen >= 2) && ((pData[0] != CMD_HEAD1) || (pData[1] != CMD_HEAD2))) {
			pData++;
			remainLen--;
			continue;
		}

		if (remainLen < DATA_PACKAGE_MIN_LEN) {
			break;
		}

		dataLen = pData[4];
		frameLen = dataLen + DATA_PACKAGE_MIN_LEN;
		if (frameLen > remainLen) {
			// The data is incomplete
      // Incomplete data packet
			break;
		}

		// Print one frame; enable the DEBUG_PRO_DATA macro in CommDef.h when needed
    // To print a data of frame, open the DEBUG_PRO_DATA macro in the CommDef.h file when needed
#ifdef DEBUG_PRO_DATA
		for (UINT i = 0; i < frameLen; ++i) {
			LOGD("%x ", pData[i]);
		}
		LOGD("\n");
#endif

		// Checksum support; enable the PRO_SUPPORT_CHECK_SUM macro in CommDef.h when needed
    // Support checksum verification, open the PRO_SUPPORT_CHECK_SUM macro in CommDef.h file when needed
#ifdef PRO_SUPPORT_CHECK_SUM
		// Verify the checksum
		if (getCheckSum(pData, frameLen - 1) == pData[frameLen - 1]) {
			// Parse one frame of data
      // Parse a data of frame
			procParse(pData, frameLen);
		} else {
			LOGE("CheckSum error!!!!!!\n");
		}
#else
		// Parse one frame of data
    // Parse a data of frame
		procParse(pData, frameLen);
#endif

		pData += frameLen;
		remainLen -= frameLen;
	}

	return len - remainLen;
}
