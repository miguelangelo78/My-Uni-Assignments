package com.learnandroid;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.graphics.Path;
import android.os.Bundle;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Created by Miguel on 07/04/2018.
 */

public class SlideAdapter extends PagerAdapter {
	Context        context;
	LayoutInflater inflater;
	ViewPager      viewPager_parentContainer;
	View           innerView;

	public SlideAdapter(final Context context, ViewPager viewPager) {
		this.context = context;
		viewPager_parentContainer = viewPager;

		viewPager_parentContainer.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
			@Override
			public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {

			}

			@Override
			public void onPageSelected(int position) {
				if(innerView == null)
					return;

				/* Animate the splash animation */
				AlphaAnimation animation = new AlphaAnimation(1.0f, 0.0f);
				animation.setDuration(3500);
				animation.setRepeatCount(0);

				ImageView imageView_arrowLeft = innerView.findViewById(R.id.imageView_arrowLeft);
				imageView_arrowLeft.startAnimation(animation);
				imageView_arrowLeft.setVisibility(View.INVISIBLE);

				ImageView imageView_arrowRight = innerView.findViewById(R.id.imageView_arrowRight);
				imageView_arrowRight.startAnimation(animation);
				imageView_arrowRight.setVisibility(View.INVISIBLE);
			}

			@Override
			public void onPageScrollStateChanged(int state) {

			}
		});
	}

	@Override
	public int getCount() {
		return MainActivity.getTopics().size();
	}

	@Override
	public boolean isViewFromObject(View view, Object object) {
		return (view == (LinearLayout)object);
	}

	@Override
	public Object instantiateItem(ViewGroup container, final int position) {
		inflater = (LayoutInflater)context.getSystemService(context.LAYOUT_INFLATER_SERVICE);
		innerView = inflater.inflate(R.layout.slide_topic, container, false);
		LinearLayout layoutSlide = innerView.findViewById(R.id.slidelinearlayout);

		Topic topic = MainActivity.getTopics().get(position);
		TextView title = innerView.findViewById(R.id.txttitle);
		title.setText(Integer.toString(position + 1) + ": " + topic.get("name"));

		TextView desc = innerView.findViewById(R.id.txtdescription);
		desc.setText(topic.get("desc"));

		Button button_play = innerView.findViewById(R.id.button_play);
		button_play.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(context, QuestionActivity.class);

				Bundle params = new Bundle();
				params.putInt("topic_index", position);
				intent.putExtras(params);

				context.startActivity(intent);
			}
		});

		Button button_scoreboard = innerView.findViewById(R.id.button_scoreboard);
		button_scoreboard.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(context, ScorecardActivity.class);

				Bundle params = new Bundle();
				params.putInt("topic_index", position);
				intent.putExtras(params);

				context.startActivity(intent);
			}
		});

		container.addView(innerView);
		return innerView;
	}

	@Override
	public void destroyItem(ViewGroup container, int position, Object object) {
		container.removeView((LinearLayout)object);
	}
}
