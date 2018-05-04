package com.learnandroid;

import android.content.Context;

import org.json.JSONArray;
import org.json.JSONObject;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;

public class FileHandler {
	public static void save(Context context, String fileName, String data) {
		try {
			FileOutputStream outputStream = context.openFileOutput(fileName, Context.MODE_PRIVATE);
			outputStream.write(data.getBytes());
			outputStream.close();
		} catch (Exception e) {
			e.printStackTrace();
		}
	}

	public static String load(Context context, String fileName) {
		try {
			FileInputStream inputStream = context.openFileInput(fileName);
			byte[] byteBuffer = new byte[inputStream.available()];

			inputStream.read(byteBuffer);
			inputStream.close();

			return new String(byteBuffer, "UTF-8");
		} catch (Exception e) {
			return null;
		}
	}
	
	public static JSONArray loadJSONArray(Context context, String fileName) {
		String data = load(context, fileName);

		try {
			if (data != null)
				return new JSONArray(data);
			else
				return null;
		} catch (Exception e) {
			return null;
		}
	}

	public static boolean exists(Context context, String fileName) {
		File file = context.getFileStreamPath(fileName);
		return (file != null && file.exists());
	}

	public static void delete(Context context, String fileName) {
		context.getFileStreamPath(fileName).delete();
	}
}
