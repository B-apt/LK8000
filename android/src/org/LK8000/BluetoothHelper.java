/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2016 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

package org.LK8000;

import java.util.UUID;
import java.util.Set;
import java.io.IOException;

import android.annotation.SuppressLint;
import android.os.Build;
import android.util.Log;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.content.Context;
import android.content.pm.PackageManager;

/**
 * A library that constructs Bluetooth ports.  It is called by C++
 * code.
 */
@SuppressLint("MissingPermission")
final class BluetoothHelper {
  private static final String TAG = "LK8000";
  private static final UUID SPP_UUID =
        UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");

  private static final BluetoothAdapter adapter;

  /**
   * Does this device support Bluetooth Low Energy?
   */
  private static boolean hasLe;

  static {
    BluetoothAdapter _adapter;
    try {
      _adapter = BluetoothAdapter.getDefaultAdapter();
    } catch (Exception e) {
      Log.e(TAG, "BluetoothAdapter.getDefaultAdapter() failed", e);
      _adapter = null;
    }

    adapter = _adapter;
  }

  public static void cancelDiscovery() {
    adapter.cancelDiscovery();
  }

  public static void Initialize(Context context) {
    hasLe = adapter != null &&
      context.getPackageManager().hasSystemFeature(PackageManager.FEATURE_BLUETOOTH_LE);
  }

  public static boolean isEnabled() {
    try {
      return adapter != null && adapter.isEnabled();
    } catch (Exception e) {
      Log.e(TAG, "BluetoothAdapter.isEnabled() failed", e);
    }
    return false;
  }

  /**
   * Turns the #BluetoothDevice into a human-readable string.
   */
  public static String getDisplayString(BluetoothDevice device) {
    String name = device.getName();
    String address = device.getAddress();

    if (name == null)
      return address;

    return name + " [" + address + "]";
  }

  public static String getDisplayString(BluetoothSocket socket) {
    return getDisplayString(socket.getRemoteDevice());
  }

  public static String getNameFromAddress(String address) {
    if (adapter == null)
      return null;

    try {
      return adapter.getRemoteDevice(address).getName();
    } catch (Exception e) {
      Log.e(TAG, "Failed to look up name of " + address, e);
      return null;
    }
  }

  public static String getTypeFromAddress(String address) {
    if (adapter == null)
      return null;

    try {
      switch (adapter.getRemoteDevice(address).getType()) {
        case BluetoothDevice.DEVICE_TYPE_CLASSIC:
          return "TYPE_CLASSIC";
        case BluetoothDevice.DEVICE_TYPE_DUAL:
          return "TYPE_DUAL";
        case BluetoothDevice.DEVICE_TYPE_LE:
          return "TYPE_LE";
        case BluetoothDevice.DEVICE_TYPE_UNKNOWN:
          return "TYPE_UNKNOWN";
      }
    } catch (Exception e) {
      Log.e(TAG, "Failed to look up type of " + address, e);
    }
    return null;
  }

  public static String[] list() {
    if (adapter == null)
      return null;

    try {
      Set<BluetoothDevice> devices = adapter.getBondedDevices();
      if (devices == null)
        return null;

      String[] addresses = new String[devices.size() * 3];
      int n = 0;
      for (BluetoothDevice device : devices) {
        addresses[n++] = device.getAddress();
        addresses[n++] = device.getName();
        if (BluetoothDevice.DEVICE_TYPE_LE == device.getType()) {
          // TODO : check if device as GATT HM10_SERVICE ...
          addresses[n++] = "HM10";
        }
        else {
          addresses[n++] = "CLASSIC";
        }
      }

      return addresses;
    } catch (Exception e) {
      Log.e(TAG, "Failed to get bluetooth devices list" , e);
      return null;
    }
  }

  public static boolean startLeScan(BluetoothAdapter.LeScanCallback cb) {
    return hasLe && adapter.startLeScan(cb);
  }

  public static void stopLeScan(BluetoothAdapter.LeScanCallback cb) {
    if (hasLe)
      adapter.stopLeScan(cb);
  }

  public static AndroidPort connect(Context ignoredContext, String address)
          throws IOException {
    if (adapter == null) {
      return null;
    }
    BluetoothDevice device = adapter.getRemoteDevice(address);
    if (device == null) {
      return null;
    }

    BluetoothSocket socket =
            device.createRfcommSocketToServiceRecord(SPP_UUID);
    return new BluetoothClientPort(socket);
  }

  public static AndroidPort connectHM10(Context context, String address) {
    if (adapter == null || !hasLe)
      return null;

    BluetoothDevice device = adapter.getRemoteDevice(address);
    if (device == null)
      return null;

    try {
      return new BluetoothGattClientPort(context, device);
    }
    catch (Exception e) {
      e.printStackTrace();
    }
    return null;
  }

  public static AndroidPort createServer() throws IOException {
    if (adapter == null)
      return null;

    return new BluetoothServerPort(adapter, SPP_UUID);
  }
}
