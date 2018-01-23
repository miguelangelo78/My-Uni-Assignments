package Main;

import gui.JFX;

public class Main {
	public static void main(String[] args) {
		JFX.initialize(args, false);
		JFX.launchScene("start");
		System.out.println("Welcome to " + JFX.APP_NAME);
	}
}
