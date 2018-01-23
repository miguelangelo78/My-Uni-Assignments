package app.home;

import communications.CommunicationsManager;
import gui.GenericController;
import gui.JFX;
import javafx.concurrent.Task;
import javafx.event.Event;
import javafx.fxml.FXML;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.canvas.Canvas;
import javafx.scene.control.Button;
import javafx.scene.control.TextArea;
import javafx.stage.Stage;

public class Controller extends GenericController {

	@FXML private Canvas canvasTrackMap;
	@FXML private Button buttonDisconnect;
	@FXML private TextArea textAreaConsole;
	
	public Controller(Stage stage, Scene scene, Parent root) {
		super(stage, scene, root);
	}

	@FXML private void buttonDisconnect_onAction(Event event) {
		/* Close communication link */
		buttonDisconnect.setText("Disconnecting...");
		buttonDisconnect.setDisable(true);
		
		JFX.runTask(new Task<Void>() {
			
			@Override
			protected Void call() throws Exception {
				CommunicationsManager.stopCommunication();
				
				JFX.runLater(new Runnable() {
					
					@Override
					public void run() {
						try {
							openAndCloseThis("start");
						} catch (Exception e) {
							e.printStackTrace();
						}
					}
				});
				
				return null;
			}
		});
	}
	
	@FXML private void buttonConsoleClear_onAction(Event event) {
		textAreaConsole.clear();
	}
	
	@Override
	public void handle(Event event) {
		
	}
}
