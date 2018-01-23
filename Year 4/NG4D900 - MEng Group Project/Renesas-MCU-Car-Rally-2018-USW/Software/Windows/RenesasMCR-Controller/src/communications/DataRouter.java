package communications;

import java.util.ArrayList;

public interface DataRouter {
	public void onConnect();
	public void onReconnect();
	public void onConnectionLost(boolean disconnectionWasRequestedByUs, boolean disconnectionWasRequestedByRemote);
	public void onDefaultPacketReceptionPacket(Packet packet);
	public void onByteStreamReception(ArrayList<Byte> dataStream);
	public boolean init(Object context);
}
