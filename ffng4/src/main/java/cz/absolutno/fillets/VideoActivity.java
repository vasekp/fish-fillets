package cz.absolutno.fillets;

import android.app.Activity;
import android.graphics.Color;
import android.media.MediaPlayer;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.support.constraint.ConstraintLayout;
import android.view.MotionEvent;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowInsets;
import android.view.WindowManager;
import android.widget.VideoView;

public class VideoActivity extends Activity implements MediaPlayer.OnCompletionListener {

    private VideoView videoView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ViewGroup layout = new ConstraintLayout(this);
        layout.setLayoutParams(new ViewGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.MATCH_PARENT));

        videoView = new VideoView(this);
        ConstraintLayout.LayoutParams lp = new ConstraintLayout.LayoutParams(0, 0);
        lp.dimensionRatio = "4:3";
        lp.horizontalBias = 0.5f;
        lp.verticalBias = 0.5f;
        lp.topToTop = 0;
        lp.bottomToBottom = 0;
        lp.leftToLeft = 0;
        lp.rightToRight = 0;
        videoView.setLayoutParams(lp);

        String path = "android.resource://" + getPackageName() + "/" + R.raw.intro;
        videoView.setVideoURI(Uri.parse(path));

        videoView.setOnCompletionListener(this);

        layout.addView(videoView);
        setContentView(layout);
        getWindow().getDecorView().setBackgroundColor(Color.BLACK);

        if (Build.VERSION.SDK_INT >= 30) {
            videoView.getWindowInsetsController().hide(
                    WindowInsets.Type.statusBars() | WindowInsets.Type.navigationBars());
        } else {
            videoView.setSystemUiVisibility(View.SYSTEM_UI_FLAG_LOW_PROFILE
                    | View.SYSTEM_UI_FLAG_FULLSCREEN
                    | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                    | View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                    | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
                    | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION);
        }
        getWindow().addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
    }

    @Override
    protected void onStart() {
        super.onStart();
        videoView.start();
    }

    @Override
    public void onCompletion(MediaPlayer mediaPlayer) {
        finish();
    }

    @Override
    public boolean onTouchEvent(MotionEvent event) {
        finish();
        return true;
    }
}