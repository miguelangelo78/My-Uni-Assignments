package com.learnandroid;

import android.animation.ArgbEvaluator;
import android.content.Context;
import android.content.Intent;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.os.Bundle;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AlphaAnimation;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Created by Miguel on 07/04/2018.
 */

public class SlideTopicAdapter extends PagerAdapter {

	private Context        context;
	private LayoutInflater inflater;
	private ViewPager      viewPager_parentContainer;
	private View           innerView;
	private ArgbEvaluator  argbEvaluator;

	public SlideTopicAdapter(final Context context, final ViewPager viewPager) {
		this.context = context;
		viewPager_parentContainer = viewPager;

		argbEvaluator = new ArgbEvaluator();

		final SlideTopicAdapter obj = this;

		viewPager_parentContainer.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {

			@Override
			public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {
				if(position < obj.getCount() - 1 && position < Constants.viewPager_backgroundColors.length - 1) {
					viewPager.setBackgroundColor(
							(Integer)argbEvaluator.evaluate(positionOffset,
									Constants.viewPager_backgroundColors[position],
									Constants.viewPager_backgroundColors[position + 1]));
				} else {
					viewPager.setBackgroundColor(Constants.viewPager_backgroundColors[Constants.viewPager_backgroundColors.length - 1]);
				}
			}

			@Override
			public void onPageSelected(int position) {
				if(innerView == null)
					return;

				/* Animate the left and right arrows by fading them out */
				AlphaAnimation animation = new AlphaAnimation(1.0f, 0.0f);
				animation.setDuration(1500);
				animation.setRepeatCount(0);

				ImageView imageView_arrowLeft = innerView.findViewById(R.id.imageView_arrowLeft);
				imageView_arrowLeft.setVisibility(View.INVISIBLE);
				if(position > 0)
					imageView_arrowLeft.startAnimation(animation);

				ImageView imageView_arrowRight = innerView.findViewById(R.id.imageView_arrowRight);
				imageView_arrowRight.setVisibility(View.INVISIBLE);
				if(position < getCount() - 1)
					imageView_arrowRight.startAnimation(animation);
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

		ImageView imageView_arrowLeft = innerView.findViewById(R.id.imageView_arrowLeft);
		imageView_arrowLeft.setColorFilter(Color.parseColor("#585859"), PorterDuff.Mode.SRC_ATOP);
		imageView_arrowLeft.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				viewPager_parentContainer.arrowScroll(View.FOCUS_LEFT);
			}
		});

		if(position == 0)
			imageView_arrowLeft.setVisibility(View.INVISIBLE);

		ImageView imageView_arrowRight = innerView.findViewById(R.id.imageView_arrowRight);
		imageView_arrowRight.setColorFilter(Color.parseColor("#585859"), PorterDuff.Mode.SRC_ATOP);
		imageView_arrowRight.setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				viewPager_parentContainer.arrowScroll(View.FOCUS_RIGHT);
			}
		});

		if(position == getCount() - 1)
			imageView_arrowRight.setVisibility(View.INVISIBLE);

		Topic topic = MainActivity.getTopics().get(position);
		TextView title = innerView.findViewById(R.id.textView_title);
		title.setText(Integer.toString(position + 1) + ": " + topic.get("name"));

		TextView desc = innerView.findViewById(R.id.textview_description);
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
