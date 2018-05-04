package app.start;

import java.util.ArrayList;
import java.util.ResourceBundle;

import com.fazecast.jSerialComm.SerialPort;

import communications.CommunicationsManager;
import gui.GenericController;
import gui.GenericScene;
import gui.JFX;
import gui.Util.Dbg;
import javafx.collections.FXCollections;
import javafx.concurrent.Task;
import javafx.event.Event;
import javafx.fxml.FXML;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.scene.control.Button;
import javafx.scene.control.ChoiceBox;
import javafx.scene.control.Label;
import javafx.scene.control.ProgressIndicator;
import javafx.scene.input.MouseEvent;
import javafx.scene.layout.AnchorPane;
import javafx.stage.Stage;

public class Controller extends GenericController {

	public Controller(Stage stage, Scene scene, Parent root) {
		super(stage, scene, root);
	}

	@FXML private AnchorPane bodypane;
	@FXML private Button buttonConnect;
	@FXML private ProgressIndicator indicatorConnect;
	@FXML private ChoiceBox<String> choiceboxCOMPorts;
	@FXML private Label labelConnectStatus;
	
	private double windowOffsetX = 0;
	private double windowOffsetY = 0;
	
	@FXML private void buttonClose_onAction(Event event) throws Exception {
		closeThis();
	}
	
	@FXML private void toolbar_onMousePressed(MouseEvent event) {
		Stage s = getStage();
		windowOffsetX = s.getX() - event.getScreenX();
		windowOffsetY = s.getY() - event.getScreenY();
		bodypane.requestFocus();
	}
	
	@FXML private void toolbar_onMouseDragged(MouseEvent event) {
		Stage s = getStage();
		s.setX(event.getScreenX() + windowOffsetX);
		s.setY(event.getScreenY() + windowOffsetY);
	}
	
	@FXML private void bodypane_onMousePressed() {
		bodypane.requestFocus();
	}
	
	private void updateConnectionLabelStatus(String msg) {
		labelConnectStatus.setText(msg);
	}
	
	public void updateCOMPortList() {
		/* Fill COM Port Choice Box */
		ArrayList<String> comList = new ArrayList<String>();
		for(SerialPort port : SerialPort.getCommPorts())
			comList.add(port.getSystemPortName() + " - " + port.getDescriptivePortName());
		choiceboxCOMPorts.setItems(FXCollections.observableArrayList(comList));
		
		/* Select default COM port from the configuration file */
		ResourceBundle appConfig = genericScene.getConfigBundle();
		if(appConfig.containsKey("defaultCOMPort")) {
			String appConfigCOMPort = appConfig.getString("defaultCOMPort");
			Dbg.log("Found COM port setting in config file: '" + appConfigCOMPort + "'");
			boolean found = false;
			
			for(int i = 0; i < comList.size(); i++) {
				if(comList.get(i).contains(appConfigCOMPort)) {
					choiceboxCOMPorts.getSelectionModel().select(i);
					found = true;
					break;
				}
			}
			
			if(!found) {
				/* Couldn't find provided COM port. Select first COM port */
				choiceboxCOMPorts.getSelectionModel().selectFirst();
			}
		} else {
			/* No config key provided. Select first COM port */
			choiceboxCOMPorts.getSelectionModel().selectFirst();	
		}
	}

	@FXML private void buttonConnect_onAction(Event event) throws Exception {
		String selectedCOMPort = (String) choiceboxCOMPorts.getValue();
		
		labelConnectStatus.setText("");
		
		if(selectedCOMPort == null) {
			updateConnectionLabelStatus("Please select valid COM port");
			choiceboxCOMPorts.setFocusTraversable(true);
			choiceboxCOMPorts.requestFocus();
		} else {
			SerialPort port = SerialPort.getCommPort(selectedCOMPort.substring(0, selectedCOMPort.indexOf(" -")));
			
			buttonConnect.setDisable(true);
			buttonConnect.setText(CommunicationsManager.port == null ? "Connecting..." : "Creating session...");
			indicatorConnect.setVisible(true);
			
			if(CommunicationsManager.port == null)
				Dbg.log("Connecting to '" + selectedCOMPort +"'...");

			JFX.runTask(new Task<Void>() {

				@Override
				protected Void call() throws Exception {
					boolean success = CommunicationsManager.port == null ? port.openPort() : true;
					
					JFX.runLater(new Runnable() {
						
						@Override
						public void run() {
							if(!success) {
								String msg = "Could not connect to '" + port.getSystemPortName() +"'";
								Dbg.log(msg);
								updateConnectionLabelStatus(msg);
								buttonConnect.setDisable(false);
								buttonConnect.setText("Connect");
								indicatorConnect.setVisible(false);
							} else {
								buttonConnect.setText("Creating session...");
								Dbg.log("Successfully connected to '" + port.getSystemPortName() +"'. Creating session now...");
								CommunicationsManager.initializeCommunication(port);
								
								/* Start communication protocol between PC/Android and the Car */
								app.home.Scene.ApplicationDataRouter router = new app.home.Scene.ApplicationDataRouter();
								
								JFX.runTask(new Task<Void>() {

									@Override
									protected Void call() throws Exception {
										if(!CommunicationsManager.start(router)) {
											
											JFX.runLater(new Runnable() {
												
												@Override
												public void run() {
													String msg = "Could not connect to '" + port.getSystemPortName() + "'. Remote host did not respond.";
													Dbg.log(msg);
													updateConnectionLabelStatus(msg);
													buttonConnect.setDisable(false);
													buttonConnect.setText("Connect");
													indicatorConnect.setVisible(false);													
												}
											});
										} else {
											
											JFX.runLater(new Runnable() {
												
												@Override
												public void run() {
													try {
														openAndCloseThis("home");
													} catch (Exception e) {
														e.printStackTrace();
													}
													
													for(gui.GenericScene scene : JFX.currentScenes) {
														GenericScene s = JFX.getScene("home");
														if(scene != null) {
															router.init(s);
															break;
														}
													}
												}
											});
										}
										
										return null;
									}
									
								});						
							}
						}
					});	
				
					return null;
				}
			});
		}
	}
	
	@FXML private void buttonRefresh_onAction(Event event) {
		updateCOMPortList();
	}
	
	@FXML private void choiceboxCOMPorts_onClick(Event event) {
		choiceboxCOMPorts.setFocusTraversable(true);
		choiceboxCOMPorts.requestFocus();
	}
	
	@Override
	public void handle(Event event) {

	}
}
