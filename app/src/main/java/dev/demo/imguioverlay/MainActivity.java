package dev.demo.imguioverlay;

import android.app.Activity;
import android.os.Bundle;
import android.view.WindowManager;

public class MainActivity extends Activity {
    static { System.loadLibrary("imguidemo"); }
    private native void nativeInit(android.view.Surface surface);
    private native void nativeResize(int width, int height);
    private native void nativeRender();
    private native void nativeTouch(int action, float x, float y);

    private DemoSurface surface;

    @Override public void onCreate(Bundle state) {
        super.onCreate(state);
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
        surface = new DemoSurface(this, this::onTouch);
        setContentView(surface);
    }

    private void onTouch(int action, float x, float y) { nativeTouch(action, x, y); }

    private class DemoSurface extends android.opengl.GLSurfaceView {
        interface TouchCallback { void onTouch(int action, float x, float y); }
        private final TouchCallback callback;
        DemoSurface(Activity activity, TouchCallback callback) {
            super(activity);
            this.callback = callback;
            setEGLContextClientVersion(3);
            setRenderer(new Renderer() {
                @Override public void onSurfaceCreated(javax.microedition.khronos.opengles.GL10 gl,
                    javax.microedition.khronos.egl.EGLConfig config) { nativeInit(getHolder().getSurface()); }
                @Override public void onSurfaceChanged(javax.microedition.khronos.opengles.GL10 gl,
                    int width, int height) { nativeResize(width, height); }
                @Override public void onDrawFrame(javax.microedition.khronos.opengles.GL10 gl) { nativeRender(); }
            });
            setRenderMode(RENDERMODE_CONTINUOUSLY);
        }
        @Override public boolean onTouchEvent(android.view.MotionEvent e) {
            callback.onTouch(e.getActionMasked(), e.getX(), e.getY());
            return true;
        }
    }
}
