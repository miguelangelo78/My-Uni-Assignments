package gui.Util;

import java.util.ArrayList;
import java.util.Collection;
import java.util.HashMap;
import java.util.Map;

public class MultiHashMap<K, V> {

	private Map<K, ArrayList<V>> m = new HashMap<>();

	public void put(K k, V v) {
		if (m.containsKey(k)) {
			m.get(k).add(v);
		} else {
			ArrayList<V> arr = new ArrayList<>();
			arr.add(v);
			m.put(k, arr);
		}
	}
	
	public ArrayList<V> get(K k) {
		return m.get(k);
	}
	
	public V get(K k, int index) {
		return m.get(k).size() - 1 < index ? null : m.get(k).get(index);
	}
	
	public boolean containsKey(Object key) {
		return m.containsKey(key);
	}
	
	public Collection<V> values() {
		Collection<V> values = new ArrayList<V>();
		for(ArrayList<V> subvalues : m.values())
			values.addAll(subvalues);
		return values; 
	}
}