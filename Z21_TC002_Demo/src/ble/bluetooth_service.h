/*
 * bluetooth_service.h
 *
 *  Created on: 12 September 2024
 *      Author: pengzc
 */

#ifndef BLE_BLUETOOTH_SERVICE_H_
#define BLE_BLUETOOTH_SERVICE_H_

#include <string>
#include <functional>
#include <atomic>
#include <base/base.h>

struct BluetoothParams {
  /**
   * The Bluetooth name shown when this device is discovered
   */
  std::string name;

  typedef std::function<void (const std::string& msg)> MessageListener;
  /**
   * Message listener
   */
  MessageListener on_message;
};

class BluetoothService : public base::Task<BluetoothParams> {
public:
  virtual ~BluetoothService();

  static BluetoothService& instance();
  DISALLOW_COPY_AND_ASSIGN_METHOD(BluetoothService);

public:
  virtual void doTask(const BluetoothParams& params) override;

  /**
   * Get the Bluetooth name from the startup parameters
   * Returns an empty string if the service has stopped
   * @return
   */
  std::string bluetoothName();

  bool isConnected() const;

protected:
  void startBluetooth();
  void stopBluetooth();
  void servicing();

  void onStarted(struct gatt_db *db);
  void onConnectionChanged(bool connection);
  void onReceive(const std::string& msg);

private:
  BluetoothService();

  class Impl;
  Impl* impl_ = NULL;
  std::atomic<bool> connected_{false};
};

#endif /* BLE_BLUETOOTH_SERVICE_H_ */
