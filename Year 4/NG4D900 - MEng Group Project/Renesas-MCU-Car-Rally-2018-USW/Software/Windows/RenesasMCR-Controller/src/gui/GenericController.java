package gui;

import javafx.event.EventHandler;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

@SuppressWarnings("rawtypes")
public abstract class GenericController implements EventHandler {

	protected final Stage stage;
	protected Scene scene = null;
	private Parent root;
	protected GenericScene genericScene;

	public GenericController(Stage stage, Scene scene, Parent root) {
		this.stage = stage;
		this.scene = scene;
		this.root = root;
	}

	public void setScene(Scene scene) {
		this.scene = scene;
	}

	public void setRoot(Parent root) {
		this.root = root;
	}

	public void setGenericScene(GenericScene genericScene) {
		this.genericScene = genericScene;
	}

	protected Stage getStage() {
		return stage;
	}

	public Scene getScene() {
		return scene;
	}

	public Parent getRoot() {
		return root;
	}

	public GenericScene getGenericScene() {
		return genericScene;
	}

	protected void closeThis() throws Exception {
		JFX.terminateStage(getStage(), genericScene.sceneTitleNormalized);
	}

	protected void openAndCloseThis(String targetScene) throws Exception {
		/* Open new scene as a sibling scene and close this one */
		JFX.launchScene(targetScene);
		closeThis();
	}

	protected void open(String targetScene) {
		/* Open new scene as a sibling scene */
		JFX.launchScene(targetScene);
	}

	protected void openOnce(String targetScene) {
		/* Open new scene only once as a sibling scene */
		JFX.launchScene(targetScene);
	}

	protected void openChild(String targetScene) {
		/* Open new scene as a child scene */
		JFX.launchChildScene(genericScene, targetScene);
	}

	protected void openChildOnce(String targetScene) {
		/* Open new scene only once as a child scene */
		JFX.launchChildSceneOnce(genericScene, targetScene);
	}

	protected void openChildModal(String targetScene) {
		/* Open new scene as a child modal scene */
		JFX.launchChildSceneModal(genericScene, targetScene);
	}

	protected void openChildModalOnce(String targetScene) {
		/* Open new scene only once as a child modal scene */
		JFX.launchChildSceneModalOnce(genericScene, targetScene);
	}
}
