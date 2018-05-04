package com.learnandroid;

import android.graphics.Color;
import android.os.Bundle;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.view.View;
import android.view.ViewGroup;
import android.view.WindowManager;
import android.widget.Button;
import android.widget.LinearLayout;
import android.widget.RadioButton;
import android.widget.RadioGroup;
import android.widget.TextView;

import java.util.ArrayList;
import java.util.Random;

/**
 * Created by Miguel on 07/04/2018.
 */

public class QuestionActivity extends AppCompatActivity {

	private ViewPager            viewpager_question;
	private SlideQuestionAdapter slideQuestionAdapter;

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
		setContentView(R.layout.activity_question);

		Random rand = new Random();

		Bundle params = getIntent().getExtras();

		/* Fetch nth topic using index passed from activity bundle */
		final int topic_index = params.getInt("topic_index");
		Topic currentTopic = MainActivity.getTopics().get(topic_index);

		TextView textView_topicName = (TextView)findViewById(R.id.textView_topicName);
		textView_topicName.setText("Topic " + Integer.toString(topic_index + 1) + " - " + currentTopic.get("name"));

		final ArrayList<Question>     questionsChosen      = new ArrayList<Question>();
		final ArrayList<Integer>      answerCorrectIndices = new ArrayList<Integer>();
		ArrayList<ArrayList<Integer>> answerIndices        = new ArrayList<ArrayList<Integer> >();

		for(int j = 0; j < Constants.QUESTIONS_PER_TOPIC; j++) {
			/* Pick a random question from this topic */
			Question currentQuestion;

			do {
				/* Must be unique */
				currentQuestion = currentTopic.getQuestions().get(rand.nextInt(Constants.QUESTION_BANK_SIZE));
			} while(questionsChosen.contains(currentQuestion));

			/* Reset the user answer index */
			currentQuestion.set("answer_index", "-1");

			questionsChosen.add(currentQuestion);

			ArrayList<String> answers = currentQuestion.getAnswers();
			int correct_answer_index = rand.nextInt(answers.size());
			answerCorrectIndices.add(correct_answer_index);

			ArrayList<Integer> randomAnswerList = new ArrayList<Integer>();

			for (int i = 0; i < answers.size(); i++) {
				int question_index;

				do {
					question_index = (i == correct_answer_index) ? 0 : rand.nextInt(answers.size() - 1) + 1;
				} while (randomAnswerList.contains(question_index));

				randomAnswerList.add(question_index);
			}

			answerIndices.add(randomAnswerList);
		}

		slideQuestionAdapter = new SlideQuestionAdapter(this, topic_index, questionsChosen, answerIndices, answerCorrectIndices);

		viewpager_question = (ViewPager)findViewById(R.id.viewpager_question);
		viewpager_question.setAdapter(slideQuestionAdapter);
		viewpager_question.addOnPageChangeListener(new ViewPager.OnPageChangeListener() {
			@Override
			public void onPageScrolled(int position, float positionOffset, int positionOffsetPixels) {

			}

			@Override
			public void onPageSelected(int position) {
				Button button_prevQuestion = (((Button)findViewById(R.id.button_prevQuestion)));
				Button button_nextQuestion = (((Button)findViewById(R.id.button_nextQuestion)));
				Button button_finish       = (((Button)findViewById(R.id.button_finish)));

				if(position >= questionsChosen.size()) {
					button_prevQuestion.setVisibility(View.INVISIBLE);
					button_finish.setVisibility(View.INVISIBLE);
					return;
				}

				boolean questionAnswered = !questionsChosen.get(position).get("answer_index").equals("-1");

				/* Set button 'button_prevQuestion' visibility */
				button_prevQuestion.setVisibility(position > 0 ? View.VISIBLE : View.INVISIBLE);

				/* Set button 'button_nextQuestion' visibility */
				button_nextQuestion.setVisibility(questionAnswered && position < questionsChosen.size() - 1 ? View.VISIBLE : View.INVISIBLE);

				/* Set button 'button_finish' visibility */
				button_finish.setVisibility((position == (Constants.QUESTIONS_PER_TOPIC - 1) && questionAnswered) || slideQuestionAdapter.isFinished() ? View.VISIBLE : View.INVISIBLE);
			}

			@Override
			public void onPageScrollStateChanged(int state) {

			}
		});

		((Button)findViewById(R.id.button_goback)).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				finish();
			}
		});

		((Button)findViewById(R.id.button_prevQuestion)).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				((ViewPager)findViewById(R.id.viewpager_question)).arrowScroll(View.FOCUS_LEFT);
			}
		});

		((Button)findViewById(R.id.button_nextQuestion)).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				((ViewPager)findViewById(R.id.viewpager_question)).arrowScroll(View.FOCUS_RIGHT);
			}
		});

		(((Button)findViewById(R.id.button_finish))).setOnClickListener(new View.OnClickListener() {
			@Override
			public void onClick(View v) {
				if(!slideQuestionAdapter.isFinished()) {
					/* Disable all radio buttons so the user can't change his answer */
					for (int i = 0; i < slideQuestionAdapter.getPagerViewCount() && i < questionsChosen.size(); i++) {
						View pagerView = slideQuestionAdapter.getPagerView(i);
						RadioGroup radioGroup_possibleAnswers = (RadioGroup) ((LinearLayout) pagerView.findViewById(R.id.linearLayout_questions)).getChildAt(0);
						if (radioGroup_possibleAnswers == null)
							continue;

						int userAnswerIndex = Integer.parseInt(questionsChosen.get(i).get("answer_index"));
						int correctAnswer   = answerCorrectIndices.get(i);

						if(userAnswerIndex != correctAnswer) {
							((RadioButton)radioGroup_possibleAnswers.getChildAt(correctAnswer)).setBackgroundColor(Color.parseColor(Constants.QUESTION_CORRECT_COLOR));
						}

						for (int j = 0; j < radioGroup_possibleAnswers.getChildCount(); j++) {
							RadioButton radioButton_possibleAnswer = (RadioButton)radioGroup_possibleAnswers.getChildAt(j);
							radioButton_possibleAnswer.setEnabled(false);
							radioButton_possibleAnswer.setLayoutParams(new RadioGroup.LayoutParams(ViewGroup.LayoutParams.MATCH_PARENT, ViewGroup.LayoutParams.WRAP_CONTENT));

							if(j == userAnswerIndex) {
								radioButton_possibleAnswer.setBackgroundColor(
										Color.parseColor(userAnswerIndex == correctAnswer ?
											Constants.QUESTION_CORRECT_COLOR :
											Constants.QUESTION_INCORRECT_COLOR));
							}
						}
					}

					/* Declare this set of questions as fully answered */
					slideQuestionAdapter.setFinished(true);

					/* Change text of button 'button_finish' from 'Finish' to the following */
					((Button)v).setText("See results");

					/* Update permanent user data */
					ScoreData.newEntry(v.getContext(), topic_index, questionsChosen, answerCorrectIndices);
				}

				/* Make button 'button_nextQuestion' invisible in case it's still in the view */
				((Button)findViewById(R.id.button_nextQuestion)).setVisibility(View.INVISIBLE);

				/* Scroll right to see the results */
				((ViewPager)findViewById(R.id.viewpager_question)).setCurrentItem(slideQuestionAdapter.getPagerViewCount() - 1);
			}
		});
	}
}
