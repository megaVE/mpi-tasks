import java.net.*; // for Socket, ServerSocket
import java.io.*; // for IOException and Input/OutputStream
import java.nio.charset.StandardCharsets;

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
        int[] bankAccounts = new int[5]; // Available bank accounts
        for(int i = 0; i < 5; i++)
            bankAccounts[i] = 0;
        for (;;) { // Run forever, accpeting and servicing connections
            Socket clntSock = servSock.accept(); // Get client connection
            System.out.println("Handling client at " +
            clntSock.getInetAddress().getHostAddress() + " on port " +
            clntSock.getPort());
            InputStream in = clntSock.getInputStream();
            OutputStream out = clntSock.getOutputStream();
            // Receive until client closes connection,
            // indicated by -1 return
            while ((recvMsgSize = in.read(byteBuffer)) != -1) {
                out.write(byteBuffer, 0, recvMsgSize);
                String byteOrderToString = new String(byteBuffer, StandardCharsets.UTF_8).substring(0, recvMsgSize);
                System.out.println(byteOrderToString);
                String[] bankOrderSplited = byteOrderToString.split("-");
                int accountNumber = Integer.parseInt(bankOrderSplited[1]);
                String bankOrder = bankOrderSplited[0];
                if(bankOrder.equals("saldo")){
                    System.out.println("Seu saldo atual eh: " + bankAccounts[accountNumber]);
                }else if(bankOrder.equals("deposito")){
                    bankAccounts[accountNumber] += Integer.parseInt(bankOrderSplited[2]);
                    System.out.println("Deposito de " + bankOrderSplited[2] + " realizado com sucesso");
                    System.out.println("Saldo atual: " + bankAccounts[accountNumber]);
                }else if(bankOrder.equals("saque")){
                    int withdrawValue = Integer.parseInt(bankOrderSplited[2]);
                    if(bankAccounts[accountNumber] >= withdrawValue){
                        bankAccounts[accountNumber] -= withdrawValue;
                        System.out.println("Saque de " + withdrawValue + " realizado com sucesso");
                    }else{
                        System.out.println("Saldo insuficiente! Não foi possível completar a operacao");
                    }
                    System.out.println("Saldo atual: " + bankAccounts[accountNumber]);
                }else{
                    System.out.println(bankOrder + ": Solicitacao invalida!");
                }
            }
            clntSock.close(); // Close de socket.
            // We are done with this client
        }
        /* NOT REACHED */
    }
}