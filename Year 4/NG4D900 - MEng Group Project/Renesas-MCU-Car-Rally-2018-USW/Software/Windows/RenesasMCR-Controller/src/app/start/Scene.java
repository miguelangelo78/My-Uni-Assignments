package app.start;

import gui.GenericScene;
import javafx.stage.Stage;
import javafx.stage.StageStyle;

public class Scene extends GenericScene {
	
	public Scene(Stage stage) throws Exception {
		super(stage);
		
		/* Initialize COM Port Choice Box */
		((app.start.Controller)controller).updateCOMPortList();

		stage.setResizable(false);
		stage.initStyle(StageStyle.UNDECORATED);
	}
}
