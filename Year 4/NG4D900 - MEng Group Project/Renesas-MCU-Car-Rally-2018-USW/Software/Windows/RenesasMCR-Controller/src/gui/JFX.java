package gui;

import gui.Util.*;
import gui.Util.Package;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.ResourceBundle;
import java.util.concurrent.CountDownLatch;

import javafx.application.Application;
import javafx.application.Platform;
import javafx.concurrent.Task;
import javafx.stage.Stage;

public class JFX extends Application implements Runnable {
	private static String CONFIG_BUNDLE_PATH = "config.config";
	private static ResourceBundle configBundle;
	
	public static String APP_NAME;
	public static String PROJECT_TOP_PACKAGE_NAME;
	public static String DEFAULT_LOCALE;
	public static List<GenericScene> currentScenes;
	
	@SuppressWarnings("rawtypes")
	public static HashMap<String, Class> sceneList;
	
	private static String[] cmdline_args;
	private static boolean initialized = false;
	private static CountDownLatch awaitInitializationCounter = null;
	
	private static boolean launchFirstStage = true;
	
	public static boolean initialize(String[] cmdline_args, boolean launchFirstStage, boolean synchronousInitialization) {
		if(initialized)
			return false;
		
		if(!CMDLine.initialized) {
			CMDLine.addFlags("jfxdbg", "Enable debugging");
			CMDLine.parse(cmdline_args);
		}
		
		Dbg.setDebugging(CMDLine.hasOption("jfxdbg"));
		Dbg.setPromptPrefix("");
		
		if(synchronousInitialization)
			restartWaitCounter();
		
		/* Set startup flags */
		JFX.cmdline_args = cmdline_args;
		JFX.launchFirstStage = launchFirstStage;
		
		/* Launch javafx application */
		new Thread(new JFX()).start();
		if(synchronousInitialization)
			awaitInitialization();
		
		initialized = true;
		return true;
	}
	
	public static void initialize(String[] cmdline_args, boolean launchFirstStage) {
		initialize(cmdline_args, launchFirstStage, true);
	}
	
	public static void initialize(String[] cmdline_args) {
		initialize(cmdline_args, launchFirstStage, true);
	}
	
	private static void terminateApp() {
		/* Closes the app itself */
		Platform.exit();
		System.exit(0);
	}
	
	public static void terminateAll() {
		/* Closes all the scenes first then the app */
		Dbg.log("Closing...");
		for(GenericScene scene : currentScenes) {
			try {
				terminateStage(scene.stage, scene.sceneTitleNormalized);
			} catch(IllegalStateException e) {
				/* Run on thread */
				Platform.runLater(new Runnable() {
					
					@Override
					public void run() {
						try {
							terminateStage(scene.stage, scene.sceneTitleNormalized);
						} catch (Exception e) {
							e.printStackTrace();
						}
					}
				});
			} catch (Exception e) {
				e.printStackTrace();
				break;
			}
		}
		terminateApp();
	}
	
	public static void terminateStage(Stage stage, String stageName) throws Exception {
		/* Close all this window's children first */
		GenericScene genericScene = JFX.getScene(stageName);
		if(genericScene == null)
			throw new UnsupportedOperationException();
		
		for(Stage childStage : genericScene.getChildrenStages())
			childStage.close();

		for(GenericScene scene : currentScenes) {
			if(scene.stage == stage) {
				scene.close();
				currentScenes.remove(scene);
				
				/* Close application in case
				 * that was the last window open */
				if(currentScenes.isEmpty()) {
					terminateAll();
					return;
				}
				
				return;
			}
		}
		
		throw new Exception("Could not terminate stage '" + stage.getTitle() + "'");
	}
	
	public static void runTask(Task<Void> task) {
		new Thread(task).start();
	}
	
	public static void runTaskSync(Task<Void> task) throws InterruptedException {
		Thread thread = new Thread(task);
		thread.start();
		thread.join(10000);
	}
	
	public static void runLater(Runnable runnable) {
		Platform.runLater(runnable);
	}
	
	public static GenericScene getScene(String sceneName) {
		for(GenericScene scene : currentScenes)
			if(scene.sceneTitleNormalized.equals(sceneName))
				return scene;
		return null;
	}
	
	@SuppressWarnings("unchecked")
	private static boolean launchScene_FXThread(Stage stage, GenericScene parentScene, String sceneName, boolean once, boolean launchAsModal) {
		if(!sceneList.containsKey(sceneName)) {
			Dbg.log("ERROR: Scene '" + sceneName + "' does not exist");
			JFX.terminateAll();
			return false;
		}

		try {
			@SuppressWarnings("rawtypes")
			Class c = sceneList.get(sceneName);

			boolean sceneAlreadyOpen = false;
			Stage tmpStage = null;
			
			if(stage == null && parentScene == null) {
				tmpStage = new Stage();
			}
			else if((stage == null && parentScene != null) || (stage != null && parentScene != null)) {
				if(once && (sceneAlreadyOpen = parentScene.isChildStageInstantiated(sceneName))) {
					parentScene.getChildStage(sceneName).requestFocus();
				} else {
					if(stage == null)
						tmpStage = new Stage();
					else
						tmpStage = stage;
					
					if(launchAsModal)
						tmpStage.initOwner(parentScene.stage);
					parentScene.addChildStage(tmpStage, sceneName);
				}
			}
			else {
				tmpStage = stage;
			}
			
			if(!sceneAlreadyOpen) {
				GenericScene scene;
				try {
					scene = (GenericScene)c.getDeclaredConstructor(Stage.class, GenericScene.class).newInstance(tmpStage, parentScene);
				} catch(NoSuchMethodException e) {
					scene = (GenericScene)c.getDeclaredConstructor(Stage.class).newInstance(tmpStage);	
				}

				scene.stage.setOnCloseRequest(event -> {
					try {
						/* Tell parent to remove us from their list */
						Stage s = (Stage)event.getSource();
						if(parentScene != null)
							parentScene.removeChildStage(s);
						
						/* Now close this stage */
						JFX.terminateStage(s, sceneName);
					} catch (Exception e) {
						e.printStackTrace();
						JFX.terminateAll();
					}
				});
				
				currentScenes.add(scene);
				scene.show();
			}
			
			decrementWaitCounter();
			return true;
		} catch (IllegalArgumentException | InvocationTargetException | NoSuchMethodException
				| SecurityException | InstantiationException | IllegalAccessException e)
		{
			Dbg.log("ERROR: Could not launch scene '" + sceneName + "'");
			e.printStackTrace();
			JFX.terminateAll();
			return false;
		}
	}
	
	private static boolean launchScene(Stage stage, GenericScene parentScene, String sceneName, boolean once, boolean launchAsModal) {
		boolean ret = false;
		
		restartWaitCounter();

		if(!Thread.currentThread().getName().equals("main")) {
			ret = launchScene_FXThread(stage, parentScene, sceneName, once, launchAsModal);
		} else {
			/* Run on thread instead */
			Platform.runLater(new Runnable() {
				
				@Override
				public void run() {
					launchScene_FXThread(stage, parentScene, sceneName, once, launchAsModal);
				}
			});

			ret = true;
		}

		awaitInitialization();
		return ret;
	}
	
	public static boolean launchScene(String sceneName) {
		return launchScene(null, null, sceneName, false, false);
	}
	
	public static boolean launchSceneOnce(String sceneName) {
		return launchScene(null, null, sceneName, true, false);
	}
	
	public static boolean launchChildScene(GenericScene parentScene, String sceneName) {
		return launchScene(null, parentScene, sceneName, false, false);
	}
	
	public static boolean launchChildSceneOnce(GenericScene parentScene, String sceneName) {
		return launchScene(null, parentScene, sceneName, true, false);
	}
	
	public static boolean launchChildSceneModal(GenericScene parentScene, String sceneName) {
		return launchScene(null, parentScene, sceneName, false, true);
	}
	
	public static boolean launchChildSceneModalOnce(GenericScene parentScene, String sceneName) {
		return launchScene(null, parentScene, sceneName, true, true);
	}

	@SuppressWarnings("rawtypes")
	@Override
	public void start(Stage primaryStage) {	
		/* Set up all scenes in this project */
		currentScenes = new ArrayList<GenericScene>();
		sceneList = new HashMap<String, Class>();
		
		/* Fetch all scenes from this project by using the package name as a way to find the scenes */
		List<Package> packages = Package.getAllPackages(PROJECT_TOP_PACKAGE_NAME, null);
		if(packages.isEmpty()) {
			Dbg.log("ERROR: Could not find any scenes in the given package '" + JFX.PROJECT_TOP_PACKAGE_NAME + "'");
			JFX.terminateAll();
		}

		/* Install them into the hash map */
		for(Package pack : packages) {
			if(Util.countOcurrences(pack.packageName, ".") != 1)
				continue; /* Scene needs to be 1 level deep into the top package */

			try {
				Class c = Class.forName(pack.packageName + ".Scene");
				sceneList.put(pack.packageName.substring(pack.packageName.lastIndexOf(".") + 1), c);
			} catch (ClassNotFoundException e) {
				/* We'll let it pass. Just ignore this package. */
			}
		}
		
		if(launchFirstStage) {
			/* Launch Start scene */
			launchScene(primaryStage, null, configBundle.getString("firstStage"), false, false);
		}
		
		decrementWaitCounter();
	}
	
	public static void loadConfigs() {
		/* Load up and set up configuration settings */
		try {
			configBundle = ResourceBundle.getBundle(CONFIG_BUNDLE_PATH);
		} catch(Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
		APP_NAME = configBundle.getString("applicationName");
		PROJECT_TOP_PACKAGE_NAME = configBundle.getString("topApplicationPackage");
		DEFAULT_LOCALE = configBundle.getString("locale");
		
		Dbg.setPromptPrefix(configBundle.containsKey("debugPrefix") ? configBundle.getString("debugPrefix") : APP_NAME);
	}
	
	public static ResourceBundle getConfigs() {
		return configBundle;
	}

	public static void awaitInitialization() {
		if(awaitInitializationCounter != null) {
			try {
				awaitInitializationCounter.await();
			} catch (InterruptedException e) {
				e.printStackTrace();
				throw new UnsupportedOperationException();
			}
		}
	}
	
	public static void decrementWaitCounter() {
		if(awaitInitializationCounter != null)
			awaitInitializationCounter.countDown();
	}
	
	public static void restartWaitCounter() {
		awaitInitializationCounter = new CountDownLatch(1);
	}
	
	@Override
	public void run() {
		Dbg.log("Loading...");
		JFX.loadConfigs();
		Dbg.log("Initializing...");
		JFX.launch(cmdline_args);
	}
}