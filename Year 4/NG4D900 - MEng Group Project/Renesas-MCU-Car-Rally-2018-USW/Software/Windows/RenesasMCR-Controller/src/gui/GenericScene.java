package gui;

import gui.Util.*;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;
import java.util.ArrayList;
import java.util.List;
import java.util.Locale;
import java.util.ResourceBundle;

import javafx.event.Event;
import javafx.event.EventHandler;
import javafx.event.EventType;
import javafx.fxml.FXMLLoader;
import javafx.scene.Group;
import javafx.scene.Node;
import javafx.scene.Parent;
import javafx.scene.Scene;
import javafx.stage.Stage;

public abstract class GenericScene {

	/* Top path of the this scene */
	private String topPath;

	/* This scene's resources and settings */
	protected String sceneTitle;
	protected String sceneTitleNormalized;
	private final String sceneFile = "Scene.fxml";
	private final String sceneCSSFile  = "styles.css";
	private final String resBundleFile = "strings";
	private String localeName = JFX.DEFAULT_LOCALE;

	/* Important scene objects */
	protected ResourceBundle bundle;
	protected FXMLLoader loader = null;
	protected Stage stage;
	protected Scene scene;
	protected Parent root;
	protected GenericController controller;
	protected boolean hasStylesheet = false;

	private MultiHashMap<String, Stage> childStagesInstantiated = null;

	@SuppressWarnings({ "rawtypes", "unchecked" })
	public GenericScene(Stage stage) {

		childStagesInstantiated = new MultiHashMap<String, Stage>();

		try {
			String packagePath = this.getClass().getCanonicalName();

			/* Set the top path */
			topPath = packagePath.substring(0, packagePath.lastIndexOf(".")).replace(".", "/") + "/";

			/* The title of the scene */
			sceneTitle = packagePath.substring(0, packagePath.lastIndexOf("."));
			sceneTitle = sceneTitle.substring(sceneTitle.lastIndexOf(".") + 1);
			sceneTitle = sceneTitle.substring(0, 1).toUpperCase() + sceneTitle.substring(1);
			sceneTitleNormalized = sceneTitle.toLowerCase(); 

			/* Initialize Controller */
			Class controllerClass;
			try {
				controllerClass = Class.forName(packagePath.substring(0, packagePath.lastIndexOf(".")) + ".Controller");
			} catch(Exception e) {
				controllerClass = null;
			}

			/* Load resources and initialize FXML loader */
			try {
				bundle = ResourceBundle.getBundle(topPath + resBundleFile, new Locale(localeName));
			} catch(Exception e) {
				bundle = null;
			}

			String fxmlFilepath = topPath + sceneFile;
			if(Util.resourceExists(fxmlFilepath)) {
				/* Load the FXML file into Parent node */
				if(bundle != null)
					loader = new FXMLLoader(getClass().getResource("/" + fxmlFilepath), bundle);
				else
					loader = new FXMLLoader(getClass().getResource("/" + fxmlFilepath));

				/* Instantiate and set controller */
				if(controllerClass != null) {
					controller =
						(GenericController)controllerClass.getDeclaredConstructor(Stage.class, Scene.class, Parent.class)
							.newInstance(stage, null, null);
					loader.setController(controller);
				}

				/* Set parent node and scene */
				root = loader.load();
				scene = new Scene(root);

				/* Initialize everything else */
				if(controllerClass != null) {
					controller.setScene(scene);
					controller.setRoot(root);
					controller.setGenericScene(this);
				}
			} else {
				/* Create brand new Parent node */
				root = new Group();
				scene = new Scene(root);
				if(controllerClass != null) {
					controller =
						(GenericController)controllerClass.getDeclaredConstructor(Stage.class, Scene.class, Parent.class)
							.newInstance(stage, scene, root);
					controller.setGenericScene(this);
				}
			}

			/* Initialize scene and load stylesheet */
			String stylesheetFilepath = topPath + sceneCSSFile;
			if(Util.resourceExists(stylesheetFilepath)) {
				scene.getStylesheets().add(getClass().getResource("/" + stylesheetFilepath).toExternalForm());
				hasStylesheet = true;
			}

			stage.setTitle(sceneTitle + " - " + JFX.APP_NAME);		
			stage.setScene(scene);

			this.stage = stage;
		} catch(Exception e) {
			e.printStackTrace();
			JFX.terminateAll();
		}
	}

	@SuppressWarnings("rawtypes")
	class CustomNamedHandler implements EventHandler {

		private final Object objectReference;
		private final String handlerName;
		private Method handlerMethod = null;

		public CustomNamedHandler(Object objectReference, String handlerName) {
			this.objectReference = objectReference;
			this.handlerName = handlerName;
		}

		@Override
		public void handle(Event event) {
			try {
				if(handlerMethod == null) {
					handlerMethod = objectReference.getClass().getDeclaredMethod(handlerName, Event.class);
					handlerMethod.setAccessible(true);
				}
				handlerMethod.invoke(objectReference, event);
			} catch (IllegalAccessException | IllegalArgumentException | InvocationTargetException
					| NoSuchMethodException | SecurityException e)
			{
				e.printStackTrace();
			}
		}

	}

	public void addChildStage(Stage stage, String childStageName) {
		childStagesInstantiated.put(childStageName, stage);
	}

	public void removeChildStage(Stage stageObjectReference) {
		childStagesInstantiated.values().remove(stageObjectReference);
	}

	public Stage getChildStage(String childStageName) {
		return childStagesInstantiated.get(childStageName, 0);
	}

	public List<Stage> getChildrenStages() {
		return new ArrayList<Stage>(childStagesInstantiated.values());
	}

	public boolean isChildStageInstantiated(String childStage) {
		return childStagesInstantiated.containsKey(childStage);
	}

	@SuppressWarnings("unchecked")
	protected <T extends Event> boolean addEvent(Node node, EventType<T> evt, String handlerName) {
		if(controller == null)
			return false;
		node.addEventHandler(evt, new CustomNamedHandler(controller, handlerName));
		return true;
	}

	@SuppressWarnings("unchecked")
	protected <T extends Event> boolean addEvent(Node node, EventType<T> evt) {
		if(controller == null)
			return false;
		node.addEventHandler(evt, controller);
		return true;
	}

	@SuppressWarnings("unchecked")
	protected <T extends Event> boolean removeEvent(Node node, EventType<T> evt) {
		if(controller == null)
			return false;
		node.removeEventHandler(evt, controller);
		return true;
	}

	@SuppressWarnings("unchecked")
	protected <T> T getComponent(String componentID) throws Exception {
		T n = null;

		try {
			/* Search for this node on the Controller class */
			Field f = controller.getClass().getDeclaredField(componentID);
			f.setAccessible(true);
			n = (T) f.get(controller);
		} catch (IllegalArgumentException | IllegalAccessException | NoSuchFieldException | SecurityException e) {
			/* Try looking for this node through the root node instead */
			char firstChar = componentID.charAt(0);

			if(firstChar != '#' && firstChar != '.') {
				/* Use ID selector */
				n = (T) root.lookup("#" + componentID);
				if(n == null) {
					/* Try a class selector */
					n = (T) root.lookup("." + componentID);
				}
			} else {
				n = (T) root.lookup(componentID);
			}
		}
		
		if(n == null)
			throw new Exception("Component '" + componentID + "' does not exist.");

		return n;
	}
	
	public ResourceBundle getConfigBundle() {
		return JFX.getConfigs();
	}

	public void show() {
		stage.show();
		afterShow();
	}

	protected void afterShow() {
		/* Nothing to do.
		 * Sub class can override this optionally */
	}

	public void close() {
		stage.close();
	}
}
