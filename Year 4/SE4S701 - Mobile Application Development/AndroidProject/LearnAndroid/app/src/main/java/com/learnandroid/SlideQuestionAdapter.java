package com.learnandroid;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.support.v4.view.PagerAdapter;
import android.support.v4.view.ViewPager;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import java.util.ArrayList;

/**
 * Created by Miguel on 07/04/2018.
 */

public class SlideQuestionAdapter extends PagerAdapter {

	private Context        context;
	private LayoutInflater inflater;

	private int                           topic_index;
	private ArrayList<Question>           questionsChosen;
	private ArrayList<ArrayList<Integer>> answerIndices;
	private ArrayList<View>               allocatedViews;
	private ArrayList<Integer>            answerCorrectIndices;

	private boolean finishedAllQuestions;

	public SlideQuestionAdapter(Context context, int topic_index, ArrayList<Question> questionsChosen, ArrayList<ArrayList<Integer>> answerIndices, ArrayList<Integer> answerCorrectIndices) {
		this.context              = context;
		this.topic_index          = topic_index;
		this.questionsChosen      = questionsChosen;
		this.answerIndices        = answerIndices;
		this.answerCorrectIndices = answerCorrectIndices;
		this.allocatedViews       = new ArrayList<View>();
		this.finishedAllQuestions = false;
	}

	public void setFinished(boolean finishedAnswering) {
		this.finishedAllQuestions = finishedAnswering;

		if(finishedAnswering)
			notifyDataSetChanged();
	}

	public boolean isFinished() {
		return finishedAllQuestions;
	}

	public View getPagerView(int index) {
		return (index < allocatedViews.size()) ? allocatedViews.get(index) : null;
	}

	public int getPagerViewCount() {
		return allocatedViews.size();
	}

	@Override
	public int getCount() {
		int count = 1;

		for(Question question : questionsChosen)
			if(!question.get("answer_index").equals("-1"))
				count++;

		if(count >= questionsChosen.size())
			return questionsChosen.size() + (finishedAllQuestions ? 1 : 0);
		else
			return count;
	}

	@Override
	public boolean isViewFromObject(View view, Object object) {
		return (view == (LinearLayout)object);
	}

	private View instantiateQuestionSlide(ViewGroup container, final int position) {
		View view = inflater.inflate(R.layout.slide_question, container, false);
		allocatedViews.add(view);

		/* Fetch question */
		Question currentQuestion = questionsChosen.get(position);

		/* Set question number */
		((TextView)view.findViewById(R.id.textView_questionIndex)).setText("Question " + (Integer.toString(position + 1)));

		/* Set question query */
		((TextView)view.findViewById(R.id.textView_question)).setText(currentQuestion.get("query"));

		/* Set question image */
		String imageName = currentQuestion.get("img");

		if(imageName != null) {
			ImageView imageview_quizImg = view.findViewById(R.id.imageview_quizImg);
			int imageID = context.getResources().getIdentifier(imageName, "drawable", context.getPackageName());
			imageview_quizImg.setImageResource(imageID);

			/* Resize image to a fixed size so it can fit the quiz slide properly */
			ViewGroup.LayoutParams imageView_layoutParams = imageview_quizImg.getLayoutParams();
			imageView_layoutParams.width  = ViewGroup.LayoutParams.MATCH_PARENT;
			imageView_layoutParams.height = 300;
			imageview_quizImg.setLayoutParams(imageView_layoutParams);
		}

		/* Set all possible answers for this question */
		RadioGroup radioGroup_possibleAnswers = new RadioGroup(context);
		ArrayList<String> currentAnswers = currentQuestion.getAnswers();

		LinearLayout.LayoutParams layoutParams = new LinearLayout.LayoutParams(LinearLayout.LayoutParams.MATCH_PARENT, LinearLayout.LayoutParams.WRAP_CONTENT);
		layoutParams.setMargins(0, 10, 0, 10);

		for (int i = 0; i < currentAnswers.size(); i++) {
			/* Create radio button with possible answer in it */
			int possibleAnswerIdx = answerIndices.get(position).get(i);
			RadioButton radioButton_possibleAnswer = new RadioButton(context);
			radioButton_possibleAnswer.setLayoutParams(layoutParams);

			radioButton_possibleAnswer.setText(Integer.toString(i + 1) + " - " + currentAnswers.get(possibleAnswerIdx));
			radioGroup_possibleAnswers.addView(radioButton_possibleAnswer);
		}

		radioGroup_possibleAnswers.setOnCheckedChangeListener(new RadioGroup.OnCheckedChangeListener() {
			@Override
			public void onCheckedChanged(RadioGroup group, int checkedId) {
				View view = allocatedViews.get(position);
				int selectedIndex = group.indexOfChild(view.findViewById(group.getCheckedRadioButtonId()));
				questionsChosen.get(position).set("answer_index", Integer.toString(selectedIndex));

				if(position < questionsChosen.size() - 1)
					/* Set button 'button_nextQuestion' visibility */
					((Button)((Activity)context).findViewById(R.id.button_nextQuestion)).setVisibility(View.VISIBLE);
				else
					/* Set button 'button_finish' visibility */
					(((Button)((Activity)context).findViewById(R.id.button_finish))).setVisibility(View.VISIBLE);

				notifyDataSetChanged();
			}
		});

		/* Add radio group to the linear layout of the view pager */
		((LinearLayout)view.findViewById(R.id.linearLayout_questions)).addView(radioGroup_possibleAnswers);

		return view;
	}

	private View instantiateScoreSlide(ViewGroup container, final int position) {
		View view = inflater.inflate(R.layout.slide_score, container, false);
		allocatedViews.add(view);

		int correctAnswers = ScoreData.countCorrectAnswers(questionsChosen, answerCorrectIndices);

		((TextView)view.findViewById(R.id.textView_score)).setText(Integer.toString(correctAnswers) + " / " + Integer.toString(questionsChosen.size()));

		((Button)view.findViewById(R.id.button_seeAnswers)).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				((ViewPager)((Activity)context).findViewById(R.id.viewpager_question)).setCurrentItem(0);
			}
		});

		((Button)view.findViewById(R.id.button_seeScoreboard)).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Intent intent = new Intent(context, ScorecardActivity.class);

				Bundle params = new Bundle();
				params.putInt("topic_index", topic_index);
				intent.putExtras(params);

				context.startActivity(intent);
				((Activity)context).finish();
			}
		});

		((Button)view.findViewById(R.id.button_playAgain)).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				Activity questionActivity = ((Activity)context);
				Intent intent = questionActivity.getIntent();

				questionActivity.finish();
				questionActivity.startActivity(intent);
			}
		});

		return view;
	}

	@Override
	public Object instantiateItem(ViewGroup container, int position) {
		if(position < allocatedViews.size()) {
			View view = allocatedViews.get(position);
			container.addView(view);
			return view;
		}

		inflater = (LayoutInflater)context.getSystemService(context.LAYOUT_INFLATER_SERVICE);

		View view;
		if(position < questionsChosen.size())
			view = instantiateQuestionSlide(container, position);
		else
			view = instantiateScoreSlide(container, position);

		container.addView(view);
		return view;
	}

	@Override
	public void destroyItem(ViewGroup container, int position, Object object) {
		container.removeView((LinearLayout)object);
	}
}
