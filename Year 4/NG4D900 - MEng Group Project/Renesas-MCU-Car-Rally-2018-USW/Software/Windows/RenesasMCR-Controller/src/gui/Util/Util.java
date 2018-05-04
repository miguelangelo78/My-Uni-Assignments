package gui.Util;

import java.io.File;

public class Util {
	
	private static boolean isJarFlag_latched = false;
	private static boolean isExecutingWithinJar_latch = false;

	public static int countOcurrences(String str, String countWhat) {
		return str.length() - str.replace(countWhat, "").length();
	}
	
	public static boolean isExecutingFromWithinJar() {
		if(isJarFlag_latched)
			return isExecutingWithinJar_latch;
		else
			isJarFlag_latched = true;
		
		try {
			String location = Util.class.getProtectionDomain().getCodeSource().getLocation().toString();
			if (location.startsWith("rsrc:")
					|| location.endsWith(".jar") && !new File(location.substring(location.indexOf(':') + 1)).isDirectory())
			{
				isExecutingWithinJar_latch = true;
				return true;
			} else {
				isExecutingWithinJar_latch = false;
				return false;
			}
		}
		catch (Exception ex) {
			isExecutingWithinJar_latch = false;
			return false;
		}
	}

	public static String getJarFileAbsolutePath() {
		if(!isExecutingFromWithinJar())
			return null;

		String path = Util.class.getResource(Util.class.getSimpleName() + ".class").getFile();
	    if(path.startsWith("/"))
	    	return null;
	    path = ClassLoader.getSystemClassLoader().getResource(path).getFile();	    
	    return path.substring(path.indexOf(':') + 1, path.lastIndexOf('!'));
	}
	
	public static boolean fileExists(String fullAbsolutePath) {
		File f = new File(fullAbsolutePath);
		return f.exists() && !f.isDirectory();
	}
	
	public static boolean resourceExists(String fullAbsolutePathFromTop) {
		return ClassLoader.getSystemClassLoader().getResource(fullAbsolutePathFromTop) != null;
	}
}
