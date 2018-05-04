package communications;

public interface Protocol {
	final int     PERIOD_UPDATE                = 1;          /* How frequent are the communications updated (in milliseconds)                           */
	final int     PERIOD_KEEPALIVE             = 1000;       /* The period in which we expect to receive a keep alive packet                            */
	final int     STARTPACKET_MAGIC0           = 0xFEBAAFEB; /* This MUST be received/transmitted in every start of packet                              */
	final int     STARTPACKET_MAGIC1           = 0x1BADB002; /* This MUST be received/transmitted in every start of packet after the first magic number */
	final int     PACKET_HEADER_SIZE           = 4 * 4;      /* How large is the header of the packet (in bytes)                                        */
	final int     BYTESTREAM_TIMEOUT           = 50;         /* How long shall we wait until we consider the byte stream reception to be over           */
	final int     BYTESTREAM_MAXSIZE           = 128;        /* How large is the bytestream before we dispatch it into the data router                  */
	final boolean BYTESTREAM_ENABLE_TERMINATOR = true;       /* Does the bytestream end + gets dispatched when it finds the character '\n' \ '\0'?      */
	final char    BYTESTREAM_TERMINATOR_CHAR   = '\0';       /* What character terminates the byte stream buffer                                        */
	final int     RECONNECT_TRIES              = 3;          /* How many tries shall we take to connect/reconnect to the car                            */
	final int     DISCONNECT_TRIES             = 3;          /* How many tries shall we take to disconnect from the car                                 */
	final int     CONNECT_TIMEOUT              = 1000;       /* How long shall we wait until the connect packet fails to acknowledge                    */
	final int     DISCONNECT_TIMEOUT           = 1000;       /* How long shall we wait until the disconnect packet fails to acknowledge                 */
	final int     CONNECT_TRY_PERIOD           = 500;        /* How long shall we wait in between every failed connect before we try again (ms)         */
	final int     KEEPALIVE_MAXCOUNTER         = 3;          /* How many times will we allow the keepalive time out event to occur                      */
}
