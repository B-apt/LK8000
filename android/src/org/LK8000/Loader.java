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

import android.util.Log;

public class Loader {
  private static final String TAG = "LK8000";

  public static boolean loaded = false;
  public static String error;

  static {
    try {
      System.loadLibrary("c++_shared");
      System.loadLibrary("oboe");
      System.loadLibrary("crypto");
      System.loadLibrary("ssl");
      System.loadLibrary("curl");
      System.loadLibrary("LK8000");
      loaded = true;
    } catch (UnsatisfiedLinkError e) {
      Log.e(TAG, e.getMessage());
      error = e.getMessage();
    }
  }
}
