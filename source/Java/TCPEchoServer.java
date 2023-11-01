import java.net.*; // for Socket, ServerSocket
import java.io.*; // for IOException and Input/OutputStream

public class TCPEchoServer {
    private static final int BUFSIZE = 32; // Size of receive buffer
    public static void main (String[] args) throws IOException {
        if (args.length != 1)
            throw new IllegalArgumentException("Parameter(s): <Port>");
        try {
            InetAddress address = InetAddress.getLocalHost();
            System.out.println("Local Host:");
            System.out.println("\t" + address.getHostName());
            System.out.println("\t" + address.getHostAddress());
        } catch (UnknownHostException e) {
            System.out.println("Unable to determine this host's address");
        }
    
        int servPort = Integer.parseInt(args[0]);
        // Create a server socket to accept client connection requests
        ServerSocket servSock = new ServerSocket(servPort);
        int recvMsgSize; // Size of receive message
        byte[] byteBuffer = new byte[BUFSIZE]; // Receive buffer
        for (;;) { // Run forever, accpeting and servicing connections
            Socket clntSock = servSock.accept(); // Get client connection
            System.out.println("Handling client at " +
            clntSock.getInetAddress().getHostAddress() + " on port " +
            clntSock.getPort());
            InputStream in = clntSock.getInputStream();
            OutputStream out = clntSock.getOutputStream();
            // Receive until client closes connection,
            // indicated by -1 return
            while ((recvMsgSize = in.read(byteBuffer)) != -1)
                out.write(byteBuffer, 0, recvMsgSize);
            clntSock.close(); // Close de socket.
            // We are done with this client
        }
        /* NOT REACHED */
    }
}