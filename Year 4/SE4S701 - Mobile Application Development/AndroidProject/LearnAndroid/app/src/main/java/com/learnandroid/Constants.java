package com.learnandroid;

import android.graphics.Color;

public final class Constants {
	public final static int    QUESTIONS_PER_TOPIC      = 5;
	public final static int    QUESTION_BANK_SIZE       = 10;
	public final static String QUESTION_CORRECT_COLOR   = "#ccffd2";
	public final static String QUESTION_INCORRECT_COLOR = "#ffc3ba";

	public final static String RESERVED_JSON_FIELDS[] = {
		"questions" ,
		"answers"
	};

	public final static Integer [] viewPager_backgroundColors = {
		Color.parseColor("#6f7d91"),
		Color.parseColor("#6c7684"),
		Color.parseColor("#434c59"),
		Color.parseColor("#294670"),
		Color.parseColor("#2a3544")
	};
}
