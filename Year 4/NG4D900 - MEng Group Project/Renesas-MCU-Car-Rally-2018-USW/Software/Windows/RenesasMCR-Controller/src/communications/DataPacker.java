package communications;

import java.lang.reflect.Field;
import java.lang.reflect.InvocationTargetException;
import java.nio.ByteBuffer;
import java.util.ArrayList;

public class DataPacker {

	@SuppressWarnings({ "rawtypes", "unchecked" })
	public static Object unpack(byte[] rawData, Class packetType) throws
		InstantiationException,   IllegalAccessException,
		IllegalArgumentException, InvocationTargetException,
		NoSuchMethodException,    SecurityException
	{
		ByteBuffer byteBuffer = ByteBuffer.wrap(rawData);
		
		/* Skip the packet header */
		for(int i = 0; i < Protocol.PACKET_HEADER_SIZE; i++)
			byteBuffer.get();
		
		Object obj = (packetType.getConstructor()).newInstance();
		
		for(Field f : packetType.getDeclaredFields()) {
			f.setAccessible(true);
			switch(f.getType().getName()) {
			case "byte":    f.set(obj, byteBuffer.get());       break;
			case "short":   f.set(obj, byteBuffer.getShort());  break;
			case "int":     f.set(obj, byteBuffer.getInt());    break;
			case "long":    f.set(obj, byteBuffer.getLong());   break;
			case "float":   f.set(obj, byteBuffer.getFloat());  break;
			case "double":  f.set(obj, byteBuffer.getDouble()); break;
			case "char":    f.set(obj, (char)byteBuffer.get()); break;
			case "boolean": f.set(obj, byteBuffer.get() == 0 ? false : true); break;
			case "class java.lang.String":
				String tmp = "";
				while(byteBuffer.hasRemaining()) {
					char ch = byteBuffer.getChar();
					if(ch == '\0')
						break;
					tmp += ch;
				}
				f.set(obj, tmp);
				break;
			default:
				/* Found an object. This is not supported */
				throw new UnsupportedOperationException();
			}
		}
		
		return obj;
	}
	
	private static void pushInt(ArrayList<Byte> array, int value) {
		array.add((byte)(value  & 0xFF));
		array.add((byte)((value & 0xFF00)     >> 8));
		array.add((byte)((value & 0xFF0000)   >> 16));
		array.add((byte)((value & 0xFF000000) >> 24));
	}
	
	public static int getInt(byte[] array, int idx) {
		int base_idx = idx * 4;		
		
		return ((array[base_idx + 3] & 0xFF) << 24) | ((array[base_idx + 2] & 0xFF) << 16) | ((array[base_idx + 1] & 0xFF) << 8) | (array[base_idx] & 0xFF);
	}
	
	public static int getInt(ArrayList<Byte> arrayList, int idx) {
		byte [] array = new byte[arrayList.size()];
		for(int i = 0; i < array.length; i++)
			array[i] = arrayList.get(i);
		return getInt(array, idx);
	}
	
	@SuppressWarnings("rawtypes")
	public static byte[] pack(Object payload) throws IllegalArgumentException, IllegalAccessException {
		ArrayList<Byte> packedData = new ArrayList<Byte>();

		/* Push the Magic0 number */
		pushInt(packedData, Protocol.STARTPACKET_MAGIC0);

		/* Push the Magic1 number */
		pushInt(packedData, Protocol.STARTPACKET_MAGIC1);

		/* Push the payload size (unknown for now) */
		pushInt(packedData, -1);
		
		/* Find the type of this packet first */
		int payloadType = -1;
		Class payloadClass = payload.getClass();
		String className = payloadClass.getName();
		
		for(int i = 0; i < PacketTypes.packetTypeList.length; i++) {
			if(className.equals(PacketTypes.packetTypeList[i].getName())) {
				payloadType = i;
				break;
			}
		}
			
		/* Push the payload type */
		pushInt(packedData, payloadType);
		
		/* Build the raw byte data from the payload object using reflection */
		for(Field f : payloadClass.getDeclaredFields()) {
			f.setAccessible(true);
			
			switch(f.getType().getName()) {
			case "byte": {
				packedData.add(f.getByte(payload));
				break;
			}
			case "short": {
				short v = f.getShort(payload); 
				packedData.add((byte)(v & 0xFF));
				packedData.add((byte)((v & 0xFF00) >> 8));
				break;
			}
			case "int": {
				int v = f.getInt(payload); 
				packedData.add((byte)(v & 0xFF));
				packedData.add((byte)((v & 0xFF00)     >> 8));
				packedData.add((byte)((v & 0xFF0000)   >> 16));
				packedData.add((byte)((v & 0xFF000000) >> 24));
				break;
			}
			case "long": {
				long v = f.getLong(payload); 
				packedData.add((byte)(v & 0xFF));
				packedData.add((byte)((v & 0xFF00)              >> 8));
				packedData.add((byte)((v & 0xFF0000)            >> 16));
				packedData.add((byte)((v & 0xFF000000)          >> 24));
				packedData.add((byte)((v & 0xFF00000000L)       >> 32));
				packedData.add((byte)((v & 0xFF0000000000L)     >> 40));
				packedData.add((byte)((v & 0xFF000000000000L)   >> 48));
				packedData.add((byte)((v & 0xFF00000000000000L) >> 56));
				break;
			}
			case "float": {
				int v = Float.floatToIntBits(f.getFloat(payload)); 
				packedData.add((byte)(v & 0xFF));
				packedData.add((byte)((v & 0xFF00)     >> 8));
				packedData.add((byte)((v & 0xFF0000)   >> 16));
				packedData.add((byte)((v & 0xFF000000) >> 24));
				break;
			}
			case "double": {
				long v = Double.doubleToLongBits(f.getDouble(payload)); 
				packedData.add((byte)(v & 0xFF));
				packedData.add((byte)((v & 0xFF00)              >> 8));
				packedData.add((byte)((v & 0xFF0000)            >> 16));
				packedData.add((byte)((v & 0xFF000000)          >> 24));
				packedData.add((byte)((v & 0xFF00000000L)       >> 32));
				packedData.add((byte)((v & 0xFF0000000000L)     >> 40));
				packedData.add((byte)((v & 0xFF000000000000L)   >> 48));
				packedData.add((byte)((v & 0xFF00000000000000L) >> 56));
				break;
			}
			case "char": {
				packedData.add((byte)f.getChar(payload));
				break;
			}
			case "boolean": {
				packedData.add((byte)(f.getBoolean(payload) ? 1 : 0));
				break;
			}
			case "class java.lang.String":
				for(char ch : ((String)f.get(payload)).toCharArray())
					packedData.add((byte)ch);
				break;
			default:
				/* Found an object. This is not supported */
				throw new UnsupportedOperationException();
			}
		}
		
		/* Set the payload size now */
		int payloadSize = packedData.size() - Protocol.PACKET_HEADER_SIZE;
		packedData.set(8,  (byte)(payloadSize  & 0xFF));
		packedData.set(9,  (byte)((payloadSize & 0xFF00)     >> 8));
		packedData.set(10, (byte)((payloadSize & 0xFF0000)   >> 16));
		packedData.set(11, (byte)((payloadSize & 0xFF000000) >> 24));
		
		byte[] ret = new byte[packedData.size()];
		for(int i = 0; i < packedData.size(); i++)
			ret[i] = packedData.get(i);
		return ret;
	}
}
