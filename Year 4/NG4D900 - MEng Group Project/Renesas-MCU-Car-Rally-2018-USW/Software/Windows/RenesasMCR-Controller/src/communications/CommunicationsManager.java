package communications;

import java.lang.reflect.InvocationTargetException;
import java.util.ArrayList;
import java.util.LinkedList;
import java.util.Queue;

import com.fazecast.jSerialComm.SerialPort;
import com.fazecast.jSerialComm.SerialPortDataListener;
import com.fazecast.jSerialComm.SerialPortEvent;

import communications.PacketTypes.*;
import gui.JFX;
import gui.Util.Dbg;
import javafx.concurrent.Task;

public class CommunicationsManager {
	
	enum InitializationStatus {
		INIT_NULL,
		INIT_OK,
		INIT_BAD
	}
	
	private static InitializationStatus initializationStatus = InitializationStatus.INIT_NULL;
	
	public static SerialPort port = null;

	public static Thread managerTask;
	private static volatile boolean stopManagertask = false;

	private static Queue<Packet> packetQueue;
	private static boolean receivedKeepalivePacket = false;
	private static int keepaliveTimeoutCounter = 0;
	private static boolean isConnected = false;
	private static boolean isReconnecting = false;
	
	private static ArrayList<Byte> packetByteBuffered;
	private static ArrayList<Byte> byteStreamBuffered;
	private static int payloadSize = -1; /* The 3rd 32-bit word on the byte array above contains the size of the payload */			
	
	private static int updateDelta = 0;
	private static boolean streamByteTimeout = false;
	private static int updateByteStreamDelta = 0;
	
	private static DataRouter dataRouter;
	
	private static boolean isBytestreamTerminatorEnabled = Protocol.BYTESTREAM_ENABLE_TERMINATOR;
	
	enum StreamMode {
		STREAM_NULL,
		STREAM_BYTE,
		STREAM_PACKET
	};
	
	private static StreamMode streamMode = StreamMode.STREAM_NULL;
	
	interface ReservedDataRouter {
		public void callback(Packet packet);
	}
	
	@SuppressWarnings("rawtypes")
	static ArrayList<Util.Tuple<Class, ReservedDataRouter>> reservedDataRouter = null;

	private static void resetRxBuffer() {
		/* Reset this so we can receive a new packet of a different size */
		packetByteBuffered.clear();
		payloadSize = -1;
		streamMode = StreamMode.STREAM_NULL;
	}
	
	private static void dispatchByteStream() throws InterruptedException {
		/* Redirect byte stream to the application data handler */
		
		/* (Dispatch byte stream on a new thread) */
		JFX.runLater(new Runnable() { 
			
			@Override
			public void run() {
				dataRouter.onByteStreamReception(byteStreamBuffered);
				byteStreamBuffered.clear();
			}
		});
	}
	
	@SuppressWarnings("rawtypes")
	public static void installDataRoute(Class packetType, ReservedDataRouter callback) {
		if(reservedDataRouter == null)
			reservedDataRouter = new ArrayList<Util.Tuple<Class, ReservedDataRouter>>();

		reservedDataRouter.add(new Util.Tuple<Class, ReservedDataRouter>(packetType, callback));
	}
	
	private static void dispatchPacket() throws InterruptedException {
		/* Redirect packet to the application data router */
		if(packetQueue.size() > 0) {
			Packet packet = packetQueue.poll();			
			
			if(!packet.isValid()) {
				/* Drop packet */
				Dbg.log("Invalid packet received");
			}
			else {
				/* Dispatch packet on a new thread */
				JFX.runTaskSync(new Task<Void>() {
					@Override
					protected Void call() throws Exception {
						JFX.runLater(new Runnable() {
							
							@SuppressWarnings("rawtypes")
							@Override
							public void run() {
								/* Look for a dedicated callback for this specific packet */
								Class c = PacketTypes.packetTypeList[packet.payloadType];
								
								for(Util.Tuple<Class, ReservedDataRouter> cback : reservedDataRouter) {
									for(int i = 0; i < PacketTypes.packetTypeList.length; i++) {
										if(((Class)cback.x) == c) {
											/* Found dedicated router */
											((ReservedDataRouter)cback.y).callback(packet);
											return;
										}
									}
								}
								
								/* No callback was installed for this packet. We'll use the default router function */
								dataRouter.onDefaultPacketReceptionPacket(packet);
							}
						});
						return null;
					}
				});
			}
		}
	}
	
	private static void pushByteStream() throws InterruptedException {
		if(byteStreamBuffered == null)
			byteStreamBuffered = new ArrayList<Byte>();
		
		/* Make copy of received buffer */
		for(Byte b : packetByteBuffered) {
			byteStreamBuffered.add(b);
			
			/* We've reached the end of the byte stream reception */
			if(isBytestreamTerminatorEnabled && b == Protocol.BYTESTREAM_TERMINATOR_CHAR) {
				dispatchByteStream();
				resetRxBuffer();
				return;
			}
			
			if(byteStreamBuffered.size() > Protocol.BYTESTREAM_MAXSIZE) {
				/* The bytestream buffer is full!
				 * We must dispatch it and flush it before pushing the new data */
				dispatchByteStream();
			}
		}
		
		packetByteBuffered.clear();
		
		/* To prevent the update thread from running against the serial rx handler event */
		streamByteTimeout = false;
		updateByteStreamDelta = 0;
	}
	
	public static void initializeCommunication(SerialPort port) {
		if(CommunicationsManager.port != null)
			return;
		
		CommunicationsManager.port = port;
		
		if(packetQueue == null)
			packetQueue = new LinkedList<Packet>();
		else
			packetQueue.clear();
		
		port.addDataListener(new SerialPortDataListener() {
			
			private boolean isPacket(ArrayList<Byte> packetByteBuffered) {
				/* Gather the data from the byte array */		
				int magic0       = DataPacker.getInt(packetByteBuffered, 0);
				int magic1       = DataPacker.getInt(packetByteBuffered, 1);
				int payloadType  = DataPacker.getInt(packetByteBuffered, 3);
				
				/* TODO: We might also want to check the size of the payload? */
				
				boolean packetValid = magic0 == Protocol.STARTPACKET_MAGIC0 && magic1 == Protocol.STARTPACKET_MAGIC1;
				boolean payloadValid = payloadType >= 0 && payloadType <= PacketTypes.packetTypeList.length;
				
				return packetValid && payloadValid;
			}
			
			@Override
			public void serialEvent(SerialPortEvent event) {
				if (event.getEventType() != SerialPort.LISTENING_EVENT_DATA_AVAILABLE)
					return;
				
				byte[] rxData = new byte[port.bytesAvailable()];
				int bytesRead = port.readBytes(rxData, rxData.length);
				
				/* Push byte(s) into local packet byte buffer */
				if(packetByteBuffered == null)
					packetByteBuffered = new ArrayList<Byte>();
				for(int i = 0; i < bytesRead; i++)
					packetByteBuffered.add(rxData[i]);

				Dbg.log("Received " + bytesRead + " bytes (Accumulated: " + packetByteBuffered.size() + " => " + packetByteBuffered.toString() + ")");
				
				/* Indicate that we just got some data. We don't know yet if it's a packet or a regular string of bytes */
				streamByteTimeout = false;
				updateByteStreamDelta = 0;
				
				if(packetByteBuffered.size() >= Protocol.PACKET_HEADER_SIZE) {
					/* Received the minimum amount of bytes required to create a packet header.
					 * We must, however, check if this data is a packet or a normal stream of bytes */
					
					if(streamMode == StreamMode.STREAM_BYTE) {
						/* Push received data into the normal byte stream */
						try {
							pushByteStream();
						} catch (InterruptedException e) {
							e.printStackTrace();
						}
						return;
					}
					
					if(payloadSize == -1) {
						/* Check if what we received is a packet or a normal stream of bytes */
						if(streamMode == StreamMode.STREAM_NULL && !isPacket(packetByteBuffered)) {
							/* Not a packet! We must redirect this stream from now on to a normal byte stream handler */
							/* Push received byte stream */
							try {
								pushByteStream();
							} catch (InterruptedException e) {
								e.printStackTrace();
							}
							streamMode = StreamMode.STREAM_BYTE;
							Dbg.log("MATCH: Found byte stream!");
							return;
						}
						
						Dbg.log("MATCH: Found packet!");
						
						/* It's a packet! Let's start */
						streamMode = StreamMode.STREAM_PACKET;
						
						/* We've received at least the two magic numbers, the 32-bit word payloadSize and the payloadType.
						 * We now know how large the packet is */
						payloadSize = (packetByteBuffered.get(11) << 24) | (packetByteBuffered.get(10) << 16) | (packetByteBuffered.get(9) << 8) | packetByteBuffered.get(8);
					} else {
						/* Keep receiving the bytes until we reach the total size of the packet */
					}
					
					if(packetByteBuffered.size() == Protocol.PACKET_HEADER_SIZE + payloadSize) {
						/* We've received enough bytes.
						 * Time to construct, parse and push the packet into the packet queue */
						try {
							packetQueue.add(new Packet(packetByteBuffered));
						} catch (IllegalArgumentException | IllegalAccessException | InstantiationException |
								 NoSuchMethodException    | SecurityException      | InvocationTargetException e)
						{
							e.printStackTrace();
						}
						
						/* Reset reception buffer */
						resetRxBuffer();
					}
				}
			}
			
			@Override
			public int getListeningEvents() {
				return SerialPort.LISTENING_EVENT_DATA_AVAILABLE;
			}
		});
		
		/* Install connect acknowledgment event */
		installDataRoute(PacketTypes.PacketConnect.class, new ReservedDataRouter() {
			
			@Override
			public void callback(Packet packet) {
				isConnected = true;
				
				PacketConnect payload = (PacketConnect) packet.payload;
				
				if(payload.connAck) {
					dataRouter.onConnect();
				} else {
					PacketConnect ack = new PacketConnect();
					ack.connAck = true;
					sendPacket(ack);
				}
			}
		});
		
		/* Install disconnect acknowledgment event */
		installDataRoute(PacketTypes.PacketDisconnect.class, new ReservedDataRouter() {
			
			@Override
			public void callback(Packet packet) {
				isConnected = false;
				
				PacketDisconnect payload = (PacketDisconnect) packet.payload;
				dataRouter.onConnectionLost(payload.disconnAck, !payload.disconnAck);
			}
		});
		
		/* Install keepalive acknowledgment event */
		installDataRoute(PacketTypes.PacketKeepAlive.class, new ReservedDataRouter() {
			
			@Override
			public void callback(Packet packet) {
				PacketKeepAlive payload = (PacketKeepAlive) packet.payload;
				receivedKeepalivePacket = payload.iAmAlive;
			}
		});
	}
	
	public static void stopCommunication() throws InterruptedException {
		for(int i = 0; i < Protocol.DISCONNECT_TRIES; i++) {
			try {
				if(disconnect())
					break;
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}

		stopManagertask = true;
		managerTask.join();
	}
	
	public static boolean disconnect() throws InterruptedException {
		if(!isConnected)
			return true;
		
		sendPacket(new PacketTypes.PacketDisconnect());
		
		/* Wait for a disconnect acknowledgment packet to arrive */
		for(int i = 0; i < Protocol.DISCONNECT_TIMEOUT; i++) {
			if(!isConnected)
				return true;
			Thread.sleep(Protocol.PERIOD_UPDATE);
		}
		
		/* We didn't receive it. We have timed out. */
		return false;
	}
	
	public static boolean connect() throws InterruptedException {
		if(isConnected)
			return true;
		
		sendPacket(new PacketTypes.PacketConnect());
	
		/* Wait for a connect acknowledgment packet to arrive */
		for(int i = 0; i < Protocol.CONNECT_TIMEOUT; i++) {
			CommunicationsManager.updateCommunication();
			if(isConnected)
				return true;
			Thread.sleep(Protocol.PERIOD_UPDATE);
		}
		
		/* We didn't receive it. We have timed out. */
		return false;
	}
	
	public static boolean firstConnect() {
		for(int i = 0; i < Protocol.RECONNECT_TRIES; i++) {
			try {
				if(connect())
					return true;
			} catch (InterruptedException e) {
				e.printStackTrace();
				return false;
			}
		}
		return false;
	}
	
	public static boolean start(DataRouter handler) {
		if(handler == null)
			throw new UnsupportedOperationException();
		
		initializationStatus = InitializationStatus.INIT_NULL;
		
		/* Install callback that manages/routes the received byte streams / packets */
		dataRouter = handler;
		
		managerTask = new Thread(() -> {
				
			/* Connect to the destination host first */
			if(!firstConnect()) {
				initializationStatus = InitializationStatus.INIT_BAD;
				stopManagertask = false;
				updateDelta = 0;
				return;
			}
			
			initializationStatus = InitializationStatus.INIT_OK;
			
			/* And poll the communication link from now on */
			while(!stopManagertask) {
				try {
					CommunicationsManager.updateCommunication();
					Thread.sleep(Protocol.PERIOD_UPDATE);
				} catch (InterruptedException e) {
					e.printStackTrace();
				}
			}

			port.removeDataListener();
			port.closePort();
			port = null;
			stopManagertask = false;
			updateDelta = 0;
		});
		
		managerTask.start();
		
		/* Wait for the connect event to occur (or a timeout) */
		while(initializationStatus == InitializationStatus.INIT_NULL) {
			try {
				Thread.sleep(1);
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
		}
		
		return initializationStatus == InitializationStatus.INIT_OK;
	}

	public static void updateCommunication() throws InterruptedException {
		/* Handle reception timeouts for byte streams */
		if(!streamByteTimeout && streamMode == StreamMode.STREAM_BYTE && packetByteBuffered != null) {
			if(updateByteStreamDelta > Protocol.BYTESTREAM_TIMEOUT) {
				/* We timed out.
				 * We must listen for more packets (or regular bytes) */
				streamByteTimeout = true;
				updateByteStreamDelta = 0;
				
				if(packetByteBuffered.size() > 0 && payloadSize == -1) {
					/* We didn't manage to receive a packet, nor did we get enough bytes that match the size of the packet header,
					 * however, we did get something back, which we still have to push regardless */
					pushByteStream();
				}
				
				Dbg.log("Byte stream timed out");
				dispatchByteStream();
				
				/* Clear up the RX buffer */
				resetRxBuffer();
			} else {
				updateByteStreamDelta++;
			}
		}
		
		switch(++updateDelta) {
		case Protocol.PERIOD_KEEPALIVE:
			
			/* Restart the update cycle */
			updateDelta = 0;
			
			if(isReconnecting || !isConnected)
				break;

			/* If we have not received a keep alive packet in the past cycles, then we lost connection */
			if(!receivedKeepalivePacket) {
				Dbg.log("WARNING: Remote host did not acknowledge the keepalive packet.");
				if(++keepaliveTimeoutCounter >= Protocol.KEEPALIVE_MAXCOUNTER) {
					isConnected = false;
					isReconnecting = true;
					dataRouter.onConnectionLost(false, false);
					
					/* Reconnect to host */
					while(!connect());
					
					/* And wait for the keepalive packet that it should've sent */
					Dbg.log("Waiting for a keepalive packet to continue communication...");
					while(!receivedKeepalivePacket) {
						CommunicationsManager.updateCommunication();
						Thread.sleep(Protocol.PERIOD_UPDATE);
					}
					
					dataRouter.onReconnect();

					isReconnecting = false;
					receivedKeepalivePacket = false;
					keepaliveTimeoutCounter = 0;
				}
			} else {
				receivedKeepalivePacket = false;
				keepaliveTimeoutCounter = 0;
			}
			
			break;
		case Protocol.PERIOD_KEEPALIVE / 2:
			if(isReconnecting)
				break;

			sendPacket(new PacketTypes.PacketKeepAlive());
			break;
		default:
			/* Redirect packet to the application data router */
			dispatchPacket();
			break;
		}
	}
	
	public static void sendPacket(Object payload) {
		Packet packet;
		try {
			packet = new Packet(payload);
			port.writeBytes(packet.payloadRawData, packet.payloadRawData.length);
		} catch (IllegalArgumentException | IllegalAccessException e) {
			e.printStackTrace();
		}
	}
	
	public static void sendDataStream(ArrayList<Byte> dataStream) {
		byte[] data = new byte[dataStream.size()];
		for(int i = 0; i < data.length; i++)
			data[i] = dataStream.get(i);
		port.writeBytes(data, data.length);
	}
	
	public static void sendString(String str) {
		byte[] data = str.getBytes();
		port.writeBytes(data, data.length);
	}
}
