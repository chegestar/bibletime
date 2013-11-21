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
