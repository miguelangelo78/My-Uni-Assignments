package com.learnandroid;

import android.app.Activity;
import android.app.AlertDialog;
import android.content.Context;
import android.content.DialogInterface;
import android.graphics.Color;
import android.os.Bundle;
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

import com.jjoe64.graphview.GraphView;
import com.jjoe64.graphview.GridLabelRenderer;
import com.jjoe64.graphview.LegendRenderer;
import com.jjoe64.graphview.ValueDependentColor;
import com.jjoe64.graphview.Viewport;
import com.jjoe64.graphview.series.BarGraphSeries;
import com.jjoe64.graphview.series.DataPoint;
import com.jjoe64.graphview.series.DataPointInterface;
import com.jjoe64.graphview.series.LineGraphSeries;
import com.jjoe64.graphview.series.OnDataPointTapListener;
import com.jjoe64.graphview.series.Series;

import org.json.JSONArray;
import org.json.JSONObject;

import java.util.regex.Matcher;
import java.util.regex.Pattern;

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
		viewpager_container.setAdapter(new CustomFragmentPagerAdapter(this, getSupportFragmentManager(), topic_index));
		viewpager_container.addOnPageChangeListener(new TabLayout.TabLayoutOnPageChangeListener(tabLayout));
		tabLayout.addOnTabSelectedListener(new TabLayout.ViewPagerOnTabSelectedListener(viewpager_container));

		scoreData = FileHandler.loadJSONArray(this, getString(R.string.quiz_user_dbfile));
	}

	public static class CustomFragment extends Fragment {

		private static final String ARG_POSITION = "position";
		public Context context;
		private int topic_index;

		public CustomFragment() {}

		public void setContext(Context context) {
			this.context = context;
		}

		public void setTopicIndex(int topic_index) { this.topic_index = topic_index; }

		public static CustomFragment newInstance(Context context, int topic_index, int position) {
			CustomFragment fragment = new CustomFragment();

			/* Manually set the context, since we're forced to use default constructors on fragments */
			fragment.setContext(context);
			fragment.setTopicIndex(topic_index);

			Bundle args = new Bundle();
			args.putInt(ARG_POSITION, position);
			fragment.setArguments(args);
			return fragment;
		}

		private View instantiateScoreList(View rootView) {
			ScrollView scrollView = new ScrollView(rootView.getContext());

			LinearLayout linearLayout = new LinearLayout(scrollView.getContext());
			linearLayout.setOrientation(LinearLayout.VERTICAL);

			final Topic topic = MainActivity.getTopics().get(topic_index);

			TextView textView_topic = new TextView(linearLayout.getContext());
			textView_topic.setText("Topic: " + topic.get("name"));
			textView_topic.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
			textView_topic.setTextSize(25);
			LinearLayout.LayoutParams params = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
			params.setMargins(0, 50, 0, 50);
			textView_topic.setLayoutParams(params);
			linearLayout.addView(textView_topic);

			/* Check if the user already played this topic before */
			int scoreEntriesFound = 0;

			try {
				for (int i = 0; i < scoreData.length(); i++) {
					/* Check if this entry belongs to the currently opened topic */
					if (this.topic_index == Integer.parseInt(scoreData.getJSONObject(i).getString("topic_index")))
						scoreEntriesFound++;
				}
			} catch(Exception e) {
				e.printStackTrace();
			}

			if(scoreData == null || scoreEntriesFound == 0) {
				TextView textView = new TextView(linearLayout.getContext());
				textView.setText("No user data found");
				textView.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
				linearLayout.addView(textView);
			} else {
				/* Load scores from stored user data and show them in a list */

				Button button_clearUserData = new Button(linearLayout.getContext());
				button_clearUserData.setText("Clear score data");
				button_clearUserData.setOnClickListener(new View.OnClickListener() {
					@Override
					public void onClick(final View v) {
						/* Ask user for confirmation before deleting the score from the permanent storage */
						AlertDialog.Builder alertBuilder = new AlertDialog.Builder(v.getContext());
						alertBuilder.setCancelable(true);
						alertBuilder.setTitle("Confirm");
						alertBuilder.setMessage("Are you sure you want to delete the user data?");

						alertBuilder.setPositiveButton("Confirm", new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int which) {
								String fileName = getResources().getString(R.string.quiz_user_dbfile);

								/* Use regex to remove the score entries for this topic only */
								String newScoreDataStr = "";
								String scoreDataStr = FileHandler.load(v.getContext(), fileName);

								String patt = "\\{\\n(?:\\n|.)+?\"topic_index\"     : \"[0-9]+?(?:\\n|.)+?\\]\\n.+?\\},?";
								Matcher m = Pattern.compile(patt).matcher(scoreDataStr);

								for(int i = 0; m.find(); i++) {
									String match = scoreDataStr.substring(m.start(), m.end());
									if(!match.contains("\"topic_index\"     : \"" + Integer.toString(topic_index) + "\""))
										newScoreDataStr += "\t" + match + "\n";
								}

								/* Delete save file so we can rewrite it without the scores for this topic */
								FileHandler.delete(getContext(), fileName);

								if(newScoreDataStr.length() > 0) {
									/* Replace the trailing ",\n" with just "\n" */
									if (newScoreDataStr.charAt(newScoreDataStr.length() - 2) == ',')
										newScoreDataStr = newScoreDataStr.substring(0, newScoreDataStr.length() - 2) + "\n";

									newScoreDataStr = "[\n" + newScoreDataStr + "]";

									/* Rewrite data with the scores for this specific topic deleted */
									FileHandler.save(getContext(), fileName, newScoreDataStr);
								}

								/* Close this activity and go back to the main activity */
								((Activity)context).finish();
							}
						});

						alertBuilder.setNegativeButton("Cancel", new DialogInterface.OnClickListener() {
							@Override
							public void onClick(DialogInterface dialog, int which) {

							}
						});

						alertBuilder.create().show();
					}
				});

				try {
					for (int i = 0, j = 0; i < scoreData.length(); i++) {
						JSONObject scoreEntry = scoreData.getJSONObject(i);
						int topic_index = Integer.parseInt(scoreEntry.getString("topic_index"));

						if(this.topic_index != topic_index)
							continue;

						LinearLayout innerLinearLayout = new LinearLayout(linearLayout.getContext());
						innerLinearLayout.setOrientation(LinearLayout.VERTICAL);

						LinearLayout innerLevel2LinearLayout = new LinearLayout(innerLinearLayout.getContext());
						innerLevel2LinearLayout.setOrientation(LinearLayout.HORIZONTAL);
						innerLevel2LinearLayout.setWeightSum(2.0f);

						/* Set score text */
						TextView textView_score = new TextView(innerLinearLayout.getContext());

						String correct_answers_str = scoreEntry.getString("correct_answers");
						String total_questions_str = scoreEntry.getString("total_questions");
						float percentage = (Float.parseFloat(correct_answers_str) / Float.parseFloat(total_questions_str)) * 100.0f;

						textView_score.setText(Integer.toString(j + 1) + " - Score: " + correct_answers_str + " / " + total_questions_str + " (" + String.format("%.2f", percentage) + "%)");
						textView_score.setTextSize(17.0f);
						LinearLayout.LayoutParams params2 = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
						params2.weight = 1.0f;
						params2.gravity = Gravity.LEFT;
						params2.setMargins(10, 60, 0, 0);
						textView_score.setLayoutParams(params2);

						innerLevel2LinearLayout.addView(textView_score);

						/* Set date and time text */
						TextView textView_dateAndTime = new TextView(innerLinearLayout.getContext());
						textView_dateAndTime.setText("(" + scoreEntry.getString("time") + ")");
						textView_dateAndTime.setTextAlignment(View.TEXT_ALIGNMENT_VIEW_END);
						textView_dateAndTime.setTextSize(12.0f);

						LinearLayout.LayoutParams params3 = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
						params3.weight = 1.0f;
						params3.gravity = Gravity.RIGHT;
						textView_dateAndTime.setLayoutParams(params3);
						innerLevel2LinearLayout.addView(textView_dateAndTime);

						innerLinearLayout.addView(innerLevel2LinearLayout);

						linearLayout.addView(innerLinearLayout);

						j++;
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

			/* Check if the user already played this topic before */
			int scoreEntriesFound = 0;

			try {
				for (int i = 0; i < scoreData.length(); i++) {
					/* Check if this entry belongs to the currently opened topic */
					if (this.topic_index == Integer.parseInt(scoreData.getJSONObject(i).getString("topic_index")))
						scoreEntriesFound++;
				}
			} catch(Exception e) {
				e.printStackTrace();
			}

			if(scoreData == null || scoreEntriesFound == 0) {
				TextView textView = new TextView(linearLayout.getContext());
				textView.setText("No user data found");
				textView.setTextAlignment(View.TEXT_ALIGNMENT_CENTER);
				linearLayout.addView(textView);
				return linearLayout;
			}

			/* Load scores from stored user data and show them in a graph */
			DataPoint[] dataPoints = new DataPoint[scoreEntriesFound];

			try {
				for (int i = 0, j = 0; i < scoreData.length() && j < dataPoints.length; i++) {
					JSONObject scoreEntry = scoreData.getJSONObject(i);

					/* Check if this entry belongs to the currently opened topic */
					int topic_index = Integer.parseInt(scoreEntry.getString("topic_index"));
					if(this.topic_index != topic_index)
						continue;

					dataPoints[j] = new DataPoint(j + 1, Integer.parseInt(scoreEntry.getString("correct_answers")));
					j++;
				}
			} catch (Exception e) {
				e.printStackTrace();
			}

			BarGraphSeries<DataPoint> series = new BarGraphSeries<>(dataPoints);
			series.setTitle("Score");
			series.setDrawValuesOnTop(true);
			series.setSpacing(5);
			series.setValuesOnTopColor(Color.BLACK);
			series.setValuesOnTopSize(50);

			series.setValueDependentColor(new ValueDependentColor<DataPoint>() {
				@Override
				public int get(DataPoint data) {
					return Color.rgb((int) data.getX()*255/4, (int) Math.abs(data.getY()*255/6), 100);
				}
			});

			series.setOnDataPointTapListener(new OnDataPointTapListener() {

				private Toast graphToast = null;

				@Override
				public void onTap(Series series, DataPointInterface dataPoint) {
					try {
						JSONObject scoreEntry = null;

						/* Search for the right score entry in between all the 'topic-mixed' score entries */
						int dataPointIndex = (int)dataPoint.getX();

						for(int i = 0, j = 0; i < scoreData.length(); i++) {
							scoreEntry = scoreData.getJSONObject(i);

							if(Integer.parseInt(scoreEntry.getString("topic_index")) == topic_index) {
								if(++j == dataPointIndex) {
									/* Found the score entry for this topic in specific! */
									break;
								}
							}
						}

						Topic topic            = MainActivity.getTopics().get(topic_index);
						String correct_answers = scoreEntry.getString("correct_answers");
						String total_questions = scoreEntry.getString("total_questions");
						String time            = scoreEntry.getString("time");

						graphToast = Toast.makeText(getActivity(),  topic.get("name") + " | Score: " + correct_answers + " / " + total_questions + "\nPlayed: " + time, Toast.LENGTH_SHORT);
						graphToast.show();
					} catch (Exception e) {
						e.printStackTrace();
					}
				}
			});

			GraphView graph = new GraphView(linearLayout.getContext());
			graph.setTitle("Score vs Attempt");
			graph.setTitleTextSize(50);

			GridLabelRenderer gridLabelRenderer = graph.getGridLabelRenderer();
			gridLabelRenderer.setHorizontalAxisTitle("Attempt");
			gridLabelRenderer.setHorizontalAxisTitleTextSize(50);
			gridLabelRenderer.setVerticalAxisTitle("Score");
			gridLabelRenderer.setVerticalAxisTitleTextSize(50);
			gridLabelRenderer.setNumHorizontalLabels(dataPoints.length + 1);

			Viewport viewport = graph.getViewport();
			viewport.setScalable(true);
			viewport.setScalableY(false);

			viewport.setYAxisBoundsManual(true);
			viewport.setMinY(0);
			viewport.setMaxY(Constants.QUESTIONS_PER_TOPIC);

			viewport.setXAxisBoundsManual(true);
			viewport.setMinX(1);
			viewport.setMaxX(dataPoints.length + 1);

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

		private Context context;
		private int topic_index;

		public CustomFragmentPagerAdapter(Context context, FragmentManager fm, int topic_index) {
			super(fm);
			this.context     = context;
			this.topic_index = topic_index;
		}

		@Override
		public Fragment getItem(int position) {
			return CustomFragment.newInstance(context, topic_index, position);
		}

		@Override
		public int getCount() {
			return 2;
		}
	}
}
