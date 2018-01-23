package gui.Util;

public class Dbg {
	private static boolean enabled = false;
	private static String promptPrefix = null;
	
	public static void setDebugging(boolean enableDebugging) {
		enabled = enableDebugging;
	}
	
	public static void setPromptPrefix(String promptPrefix) {
		Dbg.promptPrefix = promptPrefix;
	}
	
	public static boolean isDebugging() {
		return enabled;
	}
	
	public static boolean log(Object o) {
		if(!enabled)
			return false;
		System.err.println((promptPrefix != null ? promptPrefix + "> " : "") + o.toString());
		return true;
	}
}
