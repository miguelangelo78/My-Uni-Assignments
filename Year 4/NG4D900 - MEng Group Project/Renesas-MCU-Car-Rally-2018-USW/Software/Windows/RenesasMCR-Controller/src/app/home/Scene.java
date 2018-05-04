package app.home;

import java.util.ArrayList;

import communications.CommunicationsManager;
import communications.DataRouter;
import communications.Packet;
import gui.GenericScene;
import gui.Util.Dbg;
import javafx.scene.canvas.Canvas;
import javafx.scene.canvas.GraphicsContext;
import javafx.scene.control.TextArea;
import javafx.scene.layout.Pane;
import javafx.scene.paint.Color;
import javafx.stage.Stage;

public class Scene extends GenericScene {
	
	public static class ApplicationDataRouter implements DataRouter {

		private Scene context;
		private TextArea textAreaConsole;
		private Pane lostConnectionPane;
				
		public ApplicationDataRouter() {}
		
		public boolean init(Object context) {
			if(context == null || !(context instanceof GenericScene))
				return false;
			
			this.context = (Scene)context;
			
			try {
				this.textAreaConsole = (TextArea)(this.context).getComponent("textAreaConsole");
				this.lostConnectionPane = (Pane)(this.context).getComponent("lostConnectionPane");
			} catch (Exception e) {
				e.printStackTrace();
			}
			
			return true;
		}
		
		@Override
		public void onConnect() {
			Dbg.log("We are now fully connected to '"+ CommunicationsManager.port.getSystemPortName() +"'");
		}
		
		@Override
		public void onReconnect() {
			Dbg.log("We have reconnected to the remote host.");
			lostConnectionPane.setVisible(false);
		}

		@Override
		public void onConnectionLost(boolean disconnectionWasRequestedByUs, boolean disconnectionWasRequestedByRemote) {
			if(disconnectionWasRequestedByUs) {
				Dbg.log("Disconnecting from '" + CommunicationsManager.port.getSystemPortName() + "'");
			} else {
				Dbg.log("ERROR: Lost connection to remote host. Trying to reconnect... (Reason: " + 
						(disconnectionWasRequestedByRemote ? 
							"remote host did not receive our keepalive packet in time" : 
							"keepalive timeout counter reached maximum value"
						) 
					+ ")");
				if(lostConnectionPane != null)
					lostConnectionPane.setVisible(true);
			}
		}
		
		@Override
		public void onDefaultPacketReceptionPacket(Packet packet) {

		}

		@Override
		public void onByteStreamReception(ArrayList<Byte> dataStream) {
			/* Simply append the received data into the console output */
			String str = "";
			for(Byte b : dataStream)
				str += (char)b.byteValue();
			textAreaConsole.appendText(str);
		}
	}
	
	public Scene(Stage stage) throws Exception {
		super(stage);
		
		/* XXX: Temporary */
		Canvas canvas = ((Canvas)getComponent("canvasTrackMap"));
		GraphicsContext gc = canvas.getGraphicsContext2D();
		gc.setFill(Color.BLUE);
		gc.fillRect(0, 0, canvas.getWidth(), canvas.getHeight());
	}
}
