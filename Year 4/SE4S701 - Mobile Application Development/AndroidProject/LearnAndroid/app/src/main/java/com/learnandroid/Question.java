package com.learnandroid;

import java.util.ArrayList;
import java.util.HashMap;

/**
 * Created by Miguel on 07/01/2018.
 */

public class Question {
	private HashMap<String, String> fields;
	private ArrayList<String> answers;

	public Question() {
		fields = new HashMap<String, String>();
		set("answer_index", "-1");
	}

	public String get(String field) {
		return fields.get(field);
	}

	public void set(String fieldName, String fieldValue) {
		fields.put(fieldName, fieldValue);
	}

	public ArrayList<String> getAnswers() {
		return answers;
	}

	public void setAnswers(ArrayList<String> answers) {
		this.answers = answers;
	}
}
