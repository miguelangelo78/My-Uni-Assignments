package com.learnandroid;

import android.app.Activity;
import android.content.Context;
import android.graphics.Color;
import android.os.Bundle;
import android.support.constraint.ConstraintLayout;
import android.support.design.widget.TabLayout;
import android.support.v4.app.Fragment;
import android.support.v4.app.FragmentManager;
import android.support.v4.app.FragmentPagerAdapter;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;
import android.view.Gravity;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.ScrollView;
import android.widget.TextView;
import android.widget.Toast;

import com.jjoe64.graphview.DefaultLabelFormatter;
import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.helper.StaticLabelsFormatter;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.DataPointInterface;
import com.jjoe64.graphview.series.LineGraphSeries;
import com.jjoe64.graphview.series.OnDataPointTapListener;
import com.jjoe64.graphview.series.PointsGraphSeries;
import com.jjoe64.graphview.series.Series;

import org.json.JSONArray;
import org.json.JSONObject;
import org.w3c.dom.Text;

import java.text.NumberFormat;

public class ScorecardActivity extends AppCompatActivity {

	private static JSONArray scoreData;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		getSupportActionBar().setDisplayHomeAsUpEnabled(true);
		setContentView(R.layout.activity_scorecard);

		Bundle params = getIntent().getExtras();

		int topic_index = params.getInt("topic_index");

		TabLayout tabLayout = findViewById(R.id.tabLayout);
		tabLayout.setSelectedTabIndicatorColor(Color.parseColor("#4286f4"));
		tabLayout.setSelectedTabIndicatorHeight((int) (5 * getResources().getDisplayMetrics().density));
		tabLayout.setTabTextColors(Color.parseColor("#bababa"), Color.parseColor("#ffffff"));

		ViewPager viewpager_container = findViewById(R.id.viewpager_container);
		viewpager_container.setAdapter(new CustomFragmentPagerAdapter(this, getSupportFragmentManager()));
		viewpager_container.addOnPageChangeListener(new TabLayout.TabLayoutOnPageChangeListener(tabLayout));
		tabLayout.addOnTabSelectedListener(new TabLayout.ViewPagerOnTabSelectedListener(viewpager_container));

		scoreData = FileHandler.loadJSONArray(this, getString(R.string.quiz_user_dbfile));
	}

	public static class CustomFragment extends Fragment {

		private static final String ARG_POSITION = "position";
		public Context context;

		public CustomFragment() {}

		public void setContext(Context context) {
			this.context = context;
		}

		public static CustomFragment newInstance(Context context, int position) {
			CustomFragment fragment = new CustomFragment();

			/* Manually set the context, since we're forced to use default constructors on fragments */
			fragment.setContext(context);

			Bundle args = new Bundle();
			args.putInt(ARG_POSITION, position);
			fragment.setArguments(args);
			return fragment;
		}

		private View instantiateScoreList(View rootView) {
			ScrollView scrollView = new ScrollView(rootView.getContext());

			LinearLayout linearLayout = new LinearLayout(scrollView.getContext());
			linearLayout.setOrientation(LinearLayout.VERTICAL);

			/* Load scores from stored user data and show them in a list */

			if(scoreData == null) {
				TextView t = new TextView(linearLayout.getContext());
				t.setText("No user data found");
				linearLayout.addView(t);
			} else {

				Button button_clearUserData = new Button(linearLayout.getContext());
				button_clearUserData.setText("Clear score data");
				button_clearUserData.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(View v) {
						FileHandler.delete(v.getContext(), getResources().getString(R.string.quiz_user_dbfile));
						((Activity)context).finish();
					}
				});

				try {
					for (int i = 0; i < scoreData.length(); i++) {
						JSONObject scoreEntry = scoreData.getJSONObject(i);

						LinearLayout innerLinearLayout = new LinearLayout(linearLayout.getContext());
						innerLinearLayout.setOrientation(LinearLayout.VERTICAL);

						LinearLayout innerLevel2LinearLayout = new LinearLayout(innerLinearLayout.getContext());
						innerLevel2LinearLayout.setOrientation(LinearLayout.HORIZONTAL);
						innerLevel2LinearLayout.setWeightSum(2.0f);

						/* Set topic index and name */
						int topic_index = Integer.parseInt(scoreEntry.getString("topic_index"));
						Topic topic = MainActivity.getTopics().get(topic_index);

						TextView textView_topic = new TextView(innerLinearLayout.getContext());
						textView_topic.setText(Integer.toString(i + 1) +" - Category\n" + topic.get("name"));
						LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
						params.weight = 1.0f;
						params.gravity = Gravity.LEFT;
						textView_topic.setLayoutParams(params);

						innerLevel2LinearLayout.addView(textView_topic);

						/* Set date and time */
						TextView textView_dateAndTime = new TextView(innerLinearLayout.getContext());
						textView_dateAndTime.setText("(" + scoreEntry.getString("time") + ")");
						textView_dateAndTime.setTextAlignment(View.TEXT_ALIGNMENT_VIEW_END);
						textView_dateAndTime.setTextSize(12.0f);

						LinearLayout.LayoutParams params2 = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
						params2.weight = 1.0f;
						params2.gravity = Gravity.RIGHT;
						textView_dateAndTime.setLayoutParams(params2);
						innerLevel2LinearLayout.addView(textView_dateAndTime);

						innerLinearLayout.addView(innerLevel2LinearLayout);

						/* Set score */
						TextView textView_score = new TextView(innerLinearLayout.getContext());
						textView_score.setText("Score: " + scoreEntry.getString("correct_answers") + " / " + scoreEntry.getString("total_questions"));
						textView_score.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
						textView_score.setTextSize(20.0f);
						innerLinearLayout.addView(textView_score);

						linearLayout.addView(innerLinearLayout);
					}
				} catch (Exception e) {
					e.printStackTrace();
				}

				linearLayout.addView(button_clearUserData);
			}

			scrollView.addView(linearLayout);
			return scrollView;
		}

		private View instantiateScoreGraph(View rootView) {
			LinearLayout linearLayout = new LinearLayout(rootView.getContext());

			if(scoreData == null) {
				TextView t = new TextView(linearLayout.getContext());
				t.setText("No user data found");
				linearLayout.addView(t);
				return linearLayout;
			}

			/* Load scores from stored user data and show them in a graph */
			GraphView graph = new GraphView(rootView.getContext());
			DataPoint[] dataPoints = new DataPoint[scoreData.length() + 1];
			dataPoints[0] = new DataPoint(0, 0);

			try {
				for (int i = 1; i < dataPoints.length; i++) {
					JSONObject scoreEntry = scoreData.getJSONObject(i - 1);
					dataPoints[i] = new DataPoint(i, Integer.parseInt(scoreEntry.getString("correct_answers")));
				}
			} catch (Exception e) {
				e.printStackTrace();
			}

			LineGraphSeries<DataPoint> series = new LineGraphSeries<>(dataPoints);
			series.setDrawDataPoints(true);
			series.setDataPointsRadius(25);
			series.setThickness(5);

			series.setOnDataPointTapListener(new OnDataPointTapListener() {
				@Override
				public void onTap(Series series, DataPointInterface dataPoint) {
					try {
						if((int)dataPoint.getX() < 1)
							return;

						JSONObject scoreEntry = scoreData.getJSONObject((int) dataPoint.getX() - 1);
						Topic topic = MainActivity.getTopics().get(Integer.parseInt(scoreEntry.getString("topic_index")));
						String correct_answers = scoreEntry.getString("correct_answers");
						String total_questions = scoreEntry.getString("total_questions");
						Toast.makeText(getActivity(),  topic.get("name") + " | Score: " + correct_answers + " / " + total_questions, Toast.LENGTH_SHORT).show();
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
			});

			graph.getViewport().setYAxisBoundsManual(true);
			graph.getViewport().setMinY(0);
			graph.getViewport().setMaxY(QuestionActivity.QUESTIONS_PER_TOPIC);

			graph.getViewport().setXAxisBoundsManual(true);
			graph.getViewport().setMinX(1);
			graph.getViewport().setMaxX(dataPoints.length - 1);

			// enable scaling and scrolling
			graph.getViewport().setScalable(false);
			graph.getViewport().setScalableY(false);
			graph.getGridLabelRenderer().setNumHorizontalLabels(dataPoints.length);

			graph.addSeries(series);

			linearLayout.addView(graph);
			return linearLayout;
		}

		@Override
		public View onCreateView(LayoutInflater inflater, ViewGroup container, Bundle savedInstanceState) {
			View rootView = inflater.inflate(R.layout.slide_scorecard, container, false);
			LinearLayout scorecard_linearlayout = rootView.findViewById(R.id.scorecard_linearlayout);

			int position = getArguments().getInt(ARG_POSITION);
			if(position == 0)
				scorecard_linearlayout.addView(instantiateScoreList(scorecard_linearlayout));
			else if(position == 1)
				scorecard_linearlayout.addView(instantiateScoreGraph(scorecard_linearlayout));

			return rootView;
		}
	}

	public class CustomFragmentPagerAdapter extends FragmentPagerAdapter {

		Context context;

		public CustomFragmentPagerAdapter(Context context, FragmentManager fm) {
			super(fm);
			this.context = context;
		}

		@Override
		public Fragment getItem(int position) {
			return CustomFragment.newInstance(context, position);
		}

		@Override
		public int getCount() {
			return 2;
		}
	}
}
