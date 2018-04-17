package com.learnandroid;

import android.content.res.AssetManager;

import org.json.JSONArray;
import org.json.JSONException;
import org.json.JSONObject;

import java.io.IOException;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Iterator;

/**
 * Created by Miguel on 07/01/2018.
 */

public class Topic {
	final static private String RESERVED_FIELDS[] = {"questions" , "answers"};

	private HashMap<String, String> fields;
	private ArrayList<Question> questions;

	public Topic() {
		fields = new HashMap<String, String>();
	}

	public String get(String field) {
		return fields.get(field);
	}

	public void set(String fieldName, String fieldValue) {
		fields.put(fieldName, fieldValue);
	}

	public ArrayList<Question> getQuestions() {
		return questions;
	}

	public void setQuestions(ArrayList<Question> questions) {
		this.questions = questions;
	}

	public static ArrayList<Topic> parseTopics(AssetManager assetManager, String jsonDBLocalFile) {
		ArrayList<Topic> topicsArray = null;
		JSONArray dbFile = null;

		try {
			/* Read and parse database file with JSON contents */
			InputStream dbReader = assetManager.open(jsonDBLocalFile);
			byte buffer[] = new byte[dbReader.available()];
			dbReader.read(buffer);
			dbReader.close();
			dbFile = new JSONArray(new String(buffer, "UTF-8"));

			/* Get all topics now */

			/* For every topic ... */
			for(int i = 0; i < dbFile.length(); i++) {
				JSONObject nthTopic = dbFile.getJSONObject(i);
				Topic topic = new Topic();

				Iterator<String> it = nthTopic.keys();
				while(it.hasNext()) {
					String key = it.next();
					if(key.equals(RESERVED_FIELDS[0]))
						continue;
					topic.set(key, nthTopic.getString(key));
				}

				/* Fetch all the questions inside */
				ArrayList<Question> questionsArray = new ArrayList<Question>();
				JSONArray questionsJSONArray = nthTopic.getJSONArray(RESERVED_FIELDS[0]);

				/* For every question ... */
				for (int j = 0; j < questionsJSONArray.length(); j++) {
					JSONObject nthQuestion = questionsJSONArray.getJSONObject(j);
					Question question = new Question();
					question.set("id", Integer.toString(j));

					Iterator<String> it2 = nthQuestion.keys();
					while(it2.hasNext()) {
						String key = it2.next();
						if(key.equals(RESERVED_FIELDS[1]))
							continue;
						question.set(key, nthQuestion.getString(key));
					}

					/* And all the answers inside */
					ArrayList<String> answersArray = new ArrayList<String>();
					JSONArray answersJSONArray = nthQuestion.getJSONArray(RESERVED_FIELDS[1]);

					/* For every answer ... */
					for (int k = 0; k < answersJSONArray.length(); k++)
						answersArray.add(answersJSONArray.getString(k));

					/* Add the answers to the question */
					question.setAnswers(answersArray);

					/* Afterwards, create the question and add it to the question array */
					questionsArray.add(question);
				}

				/* Now that the questions are fetched, add them to the topic object */
				topic.setQuestions(questionsArray);

				/* Finally, add the questions to the topic array */
				if(topicsArray == null)
					topicsArray = new ArrayList<Topic>();

				topicsArray.add(topic);
			}
		} catch(IOException | JSONException e) {
			e.printStackTrace();
		}

		return topicsArray;
	}
}
