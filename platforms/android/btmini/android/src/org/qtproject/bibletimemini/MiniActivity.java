/*********
*
* In the name of the Father, and of the Son, and of the Holy Spirit.
*
* This file is part of BibleTime's source code, http://www.bibletime.info/.
*
* Copyright 1999-2014 by the BibleTime developers.
* The BibleTime source code is licensed under the GNU General Public License version 2.0.
*
**********/

package org.qtproject.bibletimemini;

import android.app.Activity;
import android.content.Context;
import android.os.Vibrator;

public class MiniActivity extends org.qtproject.qt5.android.bindings.QtActivity
{
    private static MiniActivity _instance;

    public MiniActivity()
    {
        _instance = this;
    }

    public static void vibrate(long miliseconds)
    {
        Vibrator v = (Vibrator)_instance.getSystemService(Context.VIBRATOR_SERVICE);
        v.vibrate(miliseconds);
    }
}
