package com.learnandroid;

import android.content.Context;
import android.util.Log;

import java.text.SimpleDateFormat;
import java.util.ArrayList;
import java.util.Calendar;

public class ScoreData {

	public static int countCorrectAnswers(ArrayList<Question> questionsChosen, ArrayList<Integer> answerCorrectIndices) {
		int correctAnswers = 0;
		for(int i = 0; i < questionsChosen.size(); i++) {
			if(Integer.parseInt(questionsChosen.get(i).get("answer_index")) == answerCorrectIndices.get(i))
				correctAnswers++;
		}
		return correctAnswers;
	}

	public static void newEntry(Context context, int topic_index, ArrayList<Question> questionsChosen, ArrayList<Integer> answerCorrectIndices) {
		int correct_answers = countCorrectAnswers(questionsChosen, answerCorrectIndices);

		String fileName = context.getResources().getString(R.string.quiz_user_dbfile);
		boolean userFileExists = FileHandler.exists(context, fileName);

		/* Build JSON string */
		String saveData = "";
		String newData  = "";

		if(userFileExists) {
			saveData += FileHandler.load(context, fileName);

			/* Remove trailing '\n]' characters and replace it with a comma and newline */
			saveData = saveData.substring(0, saveData.length() - 2) + ",\n";
		} else {
			newData += "[\n";
		}

		/* Fetch current date and time */
		String dateAndTime = new SimpleDateFormat("dd-MM-yyyy HH:mm:ss").format(Calendar.getInstance().getTime());

		/* Add topic index, user score, question count and all the answered questions to the JSON array object */
		newData +=
				"\t{\n" +
				"\t\t\"time\"            : \"" + dateAndTime                              + "\",\n" +
				"\t\t\"topic_index\"     : \"" + Integer.toString(topic_index)            + "\",\n" +
				"\t\t\"correct_answers\" : \"" + Integer.toString(correct_answers)        + "\",\n" +
				"\t\t\"total_questions\" : \"" + Integer.toString(questionsChosen.size()) + "\",\n" +
				"\t\t\"questions\" :\t\t\n\t\t\t[\n";

		for(int i = 0; i < questionsChosen.size(); i++) {
			String correctlyAnswered = Integer.parseInt(questionsChosen.get(i).get("answer_index")) == answerCorrectIndices.get(i) ? "1" : "0";
			newData += "\t\t\t\t{\"q" + questionsChosen.get(i).get("id") + "\" : \"" + correctlyAnswered + "\"}" + (i == questionsChosen.size() - 1 ? "" : ",") + "\n";
		}

		/* Add the trailing characters in order to complete the JSON array object */
		newData += "\t\t\t]\n\t}\n]";

		/* Finally, save file to internal storage */
		FileHandler.save(context, fileName, saveData + newData);
	}
}
