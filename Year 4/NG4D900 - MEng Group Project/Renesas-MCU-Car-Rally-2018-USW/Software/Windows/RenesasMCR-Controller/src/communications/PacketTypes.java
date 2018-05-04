package communications;

public interface PacketTypes {
	@SuppressWarnings("rawtypes")
	public static Class[] packetTypeList = {
		PacketNull.class,
		PacketConnect.class,
		PacketDisconnect.class,
		PacketKeepAlive.class,
		PacketCMD.class,
		PacketStream.class
	};
	
	class PacketNull {}
	
	class PacketConnect {
		boolean connRequest;
		boolean connAck;
		
		public PacketConnect() {
			connRequest = true;
		}
		
		public String toString() {
			return "connRequest: " + connRequest + " connAck: " + connAck;
		}
	}

	class PacketDisconnect {
		boolean disconnRequest;
		boolean disconnAck;
		
		public PacketDisconnect() {
			disconnRequest = true;
		}
		
		public String toString() {
			return "disconnRequest: " + disconnRequest + " disconnAck: " + disconnAck;
		}
	}
	
	class PacketKeepAlive {
		boolean iAmAlive;
		
		public PacketKeepAlive() {
			iAmAlive = true;
		}
		
		public String toString() {
			return "iAmAlive: " + iAmAlive;
		}
	}
	
	class PacketCMD {
		/* TODO */
		
		public PacketCMD() {
			
		}
		
		public String toString() {
			return "";
		}
	}
	
	class PacketStream {
		/* TODO */
		
		public PacketStream() {
			
		}
		
		public String toString() {
			return "";
		}
	}
}
