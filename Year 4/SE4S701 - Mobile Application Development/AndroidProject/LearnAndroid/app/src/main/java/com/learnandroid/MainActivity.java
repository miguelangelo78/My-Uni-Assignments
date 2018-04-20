package com.learnandroid;

import android.os.Bundle;
import android.support.v4.view.ViewPager;
import android.support.v7.app.AppCompatActivity;
import android.view.WindowManager;

import java.util.ArrayList;

public class MainActivity extends AppCompatActivity {

	private static ArrayList<Topic> topicsArray;

	public static ArrayList<Topic> getTopics() {
		return topicsArray;
	}

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        this.getWindow().setFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN, WindowManager.LayoutParams.FLAG_FULLSCREEN);
        setContentView(R.layout.activity_main);

		topicsArray = Topic.parseTopics(getAssets(), getString(R.string.quiz_dbfile));

		ViewPager viewpager = ((ViewPager)findViewById(R.id.viewpager));
		viewpager.setAdapter(new SlideTopicAdapter(this, viewpager));
	}
}
