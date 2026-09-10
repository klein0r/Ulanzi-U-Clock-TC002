/*
 * ProtocolSender.cpp
 *
 *  Created on: Sep 8, 2017
 */

#include "uart/ProtocolSender.h"
#include "uart/UartContext.h"
#include "utils/Log.h"
#include <stdio.h>

extern BYTE getCheckSum(const BYTE *pData, int len);

/**
 * Assemble this according to your protocol format; the following is only a template
 * Need to be spliced ​​according to the agreement format, the following is just a template
 */
bool sendProtocol(const UINT16 cmdID, const BYTE *pData, BYTE len) {
	if (len + DATA_PACKAGE_MIN_LEN > 256) {
		LOGE("sendProtocol data is too len !!!\n");
		return false;
	}

	BYTE dataBuf[256];

	dataBuf[0] = CMD_HEAD1;
	dataBuf[1] = CMD_HEAD2;			// Sync frame header

	dataBuf[2] = HIBYTE(cmdID);
	dataBuf[3] = LOBYTE(cmdID);		// Command byte

	dataBuf[4] = len;

	UINT frameLen = 5;

	// Data
	for (int i = 0; i < len; ++i) {
		dataBuf[frameLen] = pData[i];
		frameLen++;
	}

#ifdef PRO_SUPPORT_CHECK_SUM
	// Checksum
	dataBuf[frameLen] = getCheckSum(dataBuf, frameLen);
	frameLen++;
#endif

	return UARTCONTEXT->send(dataBuf, frameLen);
}
