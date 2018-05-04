package gui.Util;

import java.io.File;
import java.io.IOException;
import java.net.URL;
import java.net.URLDecoder;
import java.util.ArrayList;
import java.util.Enumeration;
import java.util.List;
import java.util.zip.ZipEntry;
import java.util.zip.ZipInputStream;

public class Package {
	
	public String packageName;
	public List<Class<?>> classList;
	public Package() {
		packageName = "";
		classList = new ArrayList<Class<?>>();
	}
	
	private static List<Package> getAllPackagesInsideJar(final String packageName, ClassLoader loader) {
		final List<Package> ret = new ArrayList<Package>();
		
		try {
		    ZipInputStream zip = new ZipInputStream(new URL("file:" + Util.getJarFileAbsolutePath()).openStream());
		    ZipEntry zent = null;
	
		    while((zent = zip.getNextEntry()) != null) {
		    	if(!zent.getName().startsWith(packageName + "/"))
		    		continue;
		    	if(zent.isDirectory()) {
		    		Package pack = new Package();
		    		pack.packageName = zent.getName();
		    		pack.packageName = pack.packageName.substring(0, pack.packageName.length() - 1).replace("/", ".");
		    		ret.add(pack);
		    	} else {
		    		String className = zent.getName().replace("/", ".");
		    		if(className.endsWith(".class")) {
		    			if(ret.size() > 0)
		    				ret.get(ret.size() - 1).classList.add(Class.forName(className.replace(".class", "")));
		    			else
		    				throw new Exception("Found class outside a package");
		    		}
		    	} 	
		    }
		} catch (Exception e) {
			System.out.println("Failed to load resources for package '" + packageName + "'");
			e.printStackTrace();
		}
		
		return ret;
	}
	
	public static List<Package> getAllPackages(final String packageName, ClassLoader loader) {
		if(Util.isExecutingFromWithinJar())
			return getAllPackagesInsideJar(packageName, loader);
		
		final List<Package> ret = new ArrayList<Package>();
		
		if (loader == null)
			loader = Thread.currentThread().getContextClassLoader();

		try {
			final Enumeration<URL> res = loader.getResources(packageName.replace('.', '/'));
			
			if(!res.hasMoreElements())
				return ret;
			
			Package pack = new Package();
			pack.packageName = packageName;
			ret.add(pack);
			
			while (res.hasMoreElements()) {
				File dir = new File(URLDecoder.decode(res.nextElement().getPath(), "UTF-8"));
				if(dir.listFiles() == null)
					continue;

				for (final File file : dir.listFiles()) {
					if (file.isDirectory()) {
						ret.addAll(getAllPackages(packageName + '.' + file.getName(), loader));
					} else if(file.isFile()) {
						String className = file.getName();
						if(className.endsWith(".class"))
							pack.classList.add(Class.forName(packageName + "." + className.replace(".class", "")));
					}
				}
			}
		} catch (final IOException | ClassNotFoundException e) {
			System.out.println("Failed to load resources for package '" + packageName + "'");
			e.printStackTrace();
		}
		
		return ret;
	}
}
