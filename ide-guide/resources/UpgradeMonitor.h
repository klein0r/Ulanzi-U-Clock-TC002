/*
 * UpgradeMonitor.h
 *
 *  Created on: Aug 4, 2017
 *      Author: guoxs
 */

#ifndef _OS_UPGRADE_MONITOR_H_
#define _OS_UPGRADE_MONITOR_H_

#include <string>
#include <vector>
#include "MountMonitor.h"

class UpgradeMonitor : Thread {
public:
	virtual ~UpgradeMonitor();

	void startMonitoring();
	void stopMonitoring();

	bool startUpgrade();
	void stopUpgrade();

	bool isUpgrading() const;
	bool needUpgrade() const;

	bool checkUpgrade();
    bool checkUpgradeFile(const char *pPath);
	static UpgradeMonitor* getInstance();

public:
	typedef unsigned char BYTE;
	typedef unsigned int uint32_t;

	typedef enum {
		E_UPGRADE_STATUS_START,
		E_UPGRADE_STATUS_SUCCESS,
		E_UPGRADE_STATUS_ERROR
	} EUpgradeStatus;

	typedef enum {
		E_UPGRADE_TYPE_UBOOT,
		E_UPGRADE_TYPE_BOOT,
		E_UPGRADE_TYPE_SYSTEM,
		E_UPGRADE_TYPE_RES,
		E_UPGRADE_TYPE_DATA,
		E_UPGRADE_TYPE_LOGO,
		E_UPGRADE_TYPE_ENV,
		E_UPGRADE_TYPE_PRIVATE,
		E_UPGRADE_TYPE_FULL
	} ESystemUpgradeType;

	/**
	 * Partition information
	 */
	typedef struct {
		BYTE partn;			// Partition number
		BYTE reserve[3];	// Reserved
		BYTE offset[4];		// Offset relative to the head
		BYTE imageSize[4];	// Partition image size
		BYTE imageHead[16];	// Image header data swap MD5
	} SPartInfo;

	/**
	 * Extended information
	 */
	typedef struct {
		BYTE size[4];		// Length of the extra information
		BYTE perm;			// Permissions
		BYTE type;			// Device model
		BYTE flag;			// Flags
		BYTE attr;			// Encryption-related information
		BYTE reserve[512];	// append
		BYTE crc32[4];		// Checksum
	} SExtendInfo;

	typedef struct {
		ESystemUpgradeType upgradeType;
		std::string upgradeFilePath;
		std::string partName;
		SPartInfo partInfo;
		bool needUpgrade;
	} SSystemUpgradeInfo;

	/**
	 * TS upgrade file information
	 */
	typedef struct {
		BYTE hsize;			// Equal to the size of tsinfo
		BYTE type;			// 0=unknown model, 1=TS_GT911
		BYTE flags;			// BIT(0)=force-upgrade bit (1 means force upgrade enabled), BIT(1)-BIT(7) reserved
		BYTE reserve;		// Reserved
		BYTE version[4];	// TS firmware version number
		BYTE pix[4];		// TS resolution, H16=x L16=y; the high 16 bits are x, the low 16 bits are y
		BYTE date[4];		// TS firmware build date, 0x20171104 = 4 November 2017
		BYTE dataLen[4];	// Data length
		BYTE hmd5[16];		// MD5 checksum of tsdata, excluding tsinfo; #define MD5_DIGEST_LENGTH 16
		BYTE *pData;		// Data
		bool needUpgrade;
	} STSUpgradeInfo;

	/**
	 * TS type
	 */
	enum ts_type {
		TS_GT911 = 1,
	};

	class IUpgradeStatusListener {
	public:
		virtual ~IUpgradeStatusListener() { };
		virtual void notify(int what, int status, const char *msg) = 0;
	};

	void setUpgradeStatusListener(IUpgradeStatusListener *pListener) {
		mUpgradeStatusListenerPtr = pListener;
	}

	std::vector<SSystemUpgradeInfo*>& getUpgradeInfoList() {
		return mSystemUpgradeInfoList;
	}

	const STSUpgradeInfo& getTSUpgradeInfo() const { return mTSUpgradeInfo; }


protected:
	virtual bool threadLoop();

private:
	UpgradeMonitor();

	bool checkSystemUpgradeFile();
	bool checkTSUpgradeFile();
	bool checkTouchCalib(const char *pPath);
	bool checkRestart(const char *pPath);
	bool upgradeFile(const SSystemUpgradeInfo &upgradeInfo);
	bool checkMD5(FILE *pf, const SPartInfo &partInfo);
	void resetSystemUpgradeInfo();
	void resetTSUpgradeInfo();

	void initMtdInfoList();
	const char* getMtdDevByName(const char *pName) const;
	bool writeMtdDev(const char *pDev, FILE *pf, const SSystemUpgradeInfo &upgradeInfo);

	bool readPrivate();
	bool writePrivate();

	typedef struct {
		char key[32];
		BYTE val[32];
		BYTE valLen;
		BYTE type;
	} SKeyAndVal;

	SKeyAndVal* getKeyAndValInfoByKey(const char *key);

private:
	class UpgradeMountListener : public MountMonitor::IMountListener {
	public:
		UpgradeMountListener(UpgradeMonitor *pUM) : mUMPtr(pUM) {

		}

		virtual void notify(int what, int status, const char *msg) {
			switch (status) {
			case MountMonitor::E_MOUNT_STATUS_MOUNTED:
				if (mUMPtr->checkRestart(msg) ||
					mUMPtr->checkTouchCalib(msg) ||
					mUMPtr->checkUpgradeFile(msg)) {
					// do nothing
				}
				break;

			case MountMonitor::E_MOUNT_STATUS_REMOVE:
				break;
			}
		}

	private:
		UpgradeMonitor *mUMPtr;
	};

private:
	UpgradeMountListener mUpgradeMountListener;
	IUpgradeStatusListener *mUpgradeStatusListenerPtr;

	bool mHasStartMonitor;

	std::vector<SSystemUpgradeInfo*> mSystemUpgradeInfoList;
	std::string mMountPath;

	bool mIsUpgrading;

	typedef struct {
		std::string dev;
		std::string name;
	} SMtdInfo;
	std::vector<SMtdInfo> mMtdInfoList;

	SExtendInfo mExtendInfo;

	STSUpgradeInfo mTSUpgradeInfo;

	std::string mVersionDateStr;
	const char *mErrorCode;


	/**
	 * Data storage type of the private partition
	 */
	enum val_type {
		TYPE_BYTE = 1,
		TYPE_INT,
		TYPE_STRING,
	};

	/**
	 * Header information of the private partition
	 */
	typedef struct {
		uint32_t crc32;			// head + all key + all val - 4
		uint32_t hsize;			// Header size
		uint32_t keyCount;		// Number of key-value pairs
		uint32_t keyOffset;		// Key-value offset
		uint32_t keyAsize;		// Total size of all key-value pairs
		uint32_t keySsize;		// Size of a single key-value pair
		uint32_t version;		// Version
		BYTE reserve[100];
	} SPrivateHead;

	/**
	 * Key-value information of the private partition
	 */
	typedef struct {
		char key[32];			// Key name
		uint32_t valOffset;		// Value offset
		uint32_t valLen;		// Value size
		BYTE type;				// Value type
		BYTE valid;				// Whether it is valid
		BYTE index;				// Key index
		BYTE reserve[21];
	} SPrivateKey;

	std::vector<SKeyAndVal> mKeyAndValList;
	uint32_t mPrivateVersion;
};

#define UPGRADEMONITOR		UpgradeMonitor::getInstance()

#endif /* _OS_UPGRADE_MONITOR_H_ */
