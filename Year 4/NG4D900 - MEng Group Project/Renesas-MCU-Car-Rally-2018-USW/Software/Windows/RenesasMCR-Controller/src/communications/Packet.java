package communications;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;

import gui.Util.Dbg;

public class Packet implements Protocol {
	public byte[] payloadRawData;
	
	/* Structure of a packet */
	public int magic0; /* Must be 0xFEBAAFEB */
	public int magic1; /* Must be 0x1BADB002 */
	public int payloadSize;
	public int payloadType;
	public Object payload;
	/* (The packet ends here) */
	
	private boolean packetValid;  /* Not part of the received packet */
	private boolean payloadValid; /* Not part of the received packet */
	
	public Packet(ArrayList<Byte> data) throws
		IllegalArgumentException, IllegalAccessException,
		InstantiationException, NoSuchMethodException,
		SecurityException, InvocationTargetException
	{
		/* Copy received buffer into our own byte buffer */
		payloadRawData = new byte[data.size()];
		for(int i = 0; i < data.size(); i++)
			payloadRawData[i] = data.get(i);
		
		/* Fill the packet header */
		parsePacketHeader();
		
		/* Convert received byte array into an object */
		payload = !isValid() ? null : DataPacker.unpack(payloadRawData, PacketTypes.packetTypeList[payloadType]);

		Dbg.log("-- PACKET RECEIVED -- ("+(isValid() ? "VALID - Packet type: " + PacketTypes.packetTypeList[payloadType].getSimpleName() : "!NOT VALID!")+")\n" + 
			payload + "\n-- SIZE: " + payloadRawData.length + " bytes --");
	}
	
	public Packet(Object payload) throws IllegalArgumentException, IllegalAccessException {
		/* Convert object into byte array */
		payloadRawData = DataPacker.pack(payload);
		
		/* Fill the packet header */
		parsePacketHeader();

		this.payload = payload;

		Dbg.log("-- PACKET SENT -- (Packet type: " + PacketTypes.packetTypeList[payloadType].getSimpleName() + ")\n" + 
				payload + "\n-- SIZE: " + payloadRawData.length + " bytes --");
	}
	
	public String toString() {
		String ret = "";
		for(int i = 0; i < payloadRawData.length; i++)
			ret += (char)payloadRawData[i];
		return ret;
	}
	
	public boolean isPacketValid() {
		return packetValid;
	}
	
	public boolean isPayloadValid() {
		return payloadValid;
	}

	public boolean isValid() {
		return isPacketValid() && isPayloadValid();
	}
	
	private void parsePacketHeader() {
		/* Gather the data from the byte array */		
		magic0       = DataPacker.getInt(payloadRawData, 0);
		magic1       = DataPacker.getInt(payloadRawData, 1);
		payloadSize  = DataPacker.getInt(payloadRawData, 2);
		payloadType  = DataPacker.getInt(payloadRawData, 3);

		packetValid = magic0 == Protocol.STARTPACKET_MAGIC0 && magic1 == Protocol.STARTPACKET_MAGIC1;
		payloadValid = payloadType >= 0 && payloadType <= PacketTypes.packetTypeList.length;
	}
}
