package com.learnandroid;

import android.animation.Animator;
import android.animation.AnimatorSet;
import android.animation.ObjectAnimator;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.WindowManager;
import android.view.animation.Animation;
import android.view.animation.AnimationUtils;
import android.view.animation.LinearInterpolator;
import android.view.animation.RotateAnimation;
import android.widget.ImageView;

public class SplashScreen extends AppCompatActivity {

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.activity_splash_screen);

		/* Animate the splash animation by scaling it up on startup */
		ImageView imageView = ((ImageView)findViewById(R.id.imageView));
		ObjectAnimator scaleX = ObjectAnimator.ofFloat(imageView, "scaleX", 2.0f);
		ObjectAnimator scaleY = ObjectAnimator.ofFloat(imageView, "scaleY", 2.0f);
		scaleX.setDuration(5000);
		scaleY.setDuration(5000);

		AnimatorSet animator = new AnimatorSet();
		animator.play(scaleX).with(scaleY);
		animator.start();

		animator.addListener(new Animator.AnimatorListener() {
			@Override
			public void onAnimationStart(Animator animation) {

			}

			@Override
			public void onAnimationEnd(Animator animation) {
				finish();
				startActivity(new Intent(SplashScreen.this, MainActivity.class));
			}

			@Override
			public void onAnimationCancel(Animator animation) {

			}

			@Override
			public void onAnimationRepeat(Animator animation) {

			}
		});
	}
}
