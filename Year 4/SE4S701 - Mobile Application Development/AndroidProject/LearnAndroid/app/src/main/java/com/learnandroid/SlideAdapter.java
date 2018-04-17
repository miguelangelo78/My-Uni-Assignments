package com.learnandroid;

import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.view.PagerAdapter;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.TextView;

/**
 * Created by Miguel on 07/04/2018.
 */

public class SlideAdapter extends PagerAdapter {
	Context        context;
	LayoutInflater inflater;

	public SlideAdapter(Context context) {
		this.context = context;
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
		View view = inflater.inflate(R.layout.slide_topic, container, false);
		LinearLayout layoutSlide = view.findViewById(R.id.slidelinearlayout);

		Topic topic = MainActivity.getTopics().get(position);
		TextView title = view.findViewById(R.id.txttitle);
		title.setText(Integer.toString(position + 1) + ": " + topic.get("name"));

		TextView desc = view.findViewById(R.id.txtdescription);
		desc.setText(topic.get("desc"));

		Button button_play = view.findViewById(R.id.button_play);
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

		Button button_scoreboard = view.findViewById(R.id.button_scoreboard);
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

		container.addView(view);
		return view;
	}

	@Override
	public void destroyItem(ViewGroup container, int position, Object object) {
		container.removeView((LinearLayout)object);
	}
}
