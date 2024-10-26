import java.io.*;
import java.net.*;
import java.util.Scanner;

public class nc {

    private static final int BUFFER_SIZE = 1024;

    public static void main(String[] args) {
        if (args.length < 3) {
            System.err.println("Usage: java nc -l <port> udp|tcp (for server) or java nc <address> <port> udp|tcp (for client)");
            System.exit(1);
        }

        boolean isUdp = args[2].equalsIgnoreCase("udp");

        if (args[0].equals("-l")) {
            int port = Integer.parseInt(args[1]);
            if (isUdp) {
                runUdpServer(port);
            } else {
                runTcpServer(port);
            }
        } else {
            String address = args[0];
            int port = Integer.parseInt(args[1]);
            if (isUdp) {
                runUdpClient(address, port);
            } else {
                runTcpClient(address, port);
            }
        }
    }

    private static void runTcpClient(String address, int port) {
        try (Socket socket = new Socket(address, port);
             BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
             PrintWriter writer = new PrintWriter(socket.getOutputStream(), true);
             BufferedReader serverReader = new BufferedReader(new InputStreamReader(socket.getInputStream()))) {

            System.out.println("Connected to " + address + ":" + port);
            System.out.println("Type 'exit' to close the connection.");

            String userInput;
            while (true) {
                System.out.print("You: ");
                userInput = reader.readLine();

                if ("exit".equalsIgnoreCase(userInput)) {
                    System.out.println("Closing connection...");
                    break;
                }

                writer.println(userInput);
                String serverResponse = serverReader.readLine();
                if (serverResponse == null) {
                    System.out.println("Connection closed by server");
                    break;
                }
                System.out.println("Server: " + serverResponse);
            }
        } catch (IOException e) {
            System.err.println("TCP Client Error: " + e.getMessage());
        }
    }

    private static void runTcpServer(int port) {
        try (ServerSocket serverSocket = new ServerSocket(port);
             Socket clientSocket = serverSocket.accept();
             BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
             PrintWriter writer = new PrintWriter(clientSocket.getOutputStream(), true);
             BufferedReader clientReader = new BufferedReader(new InputStreamReader(clientSocket.getInputStream()))) {

            System.out.println("Listening on port " + port + "...");
            System.out.println("Connected to client");
            System.out.println("Type 'exit' to close the connection.");

            String clientInput;
            while (true) {
                clientInput = clientReader.readLine();
                if (clientInput == null || "exit".equalsIgnoreCase(clientInput)) {
                    System.out.println("Closing connection...");
                    break;
                }
                System.out.println("Client: " + clientInput);

                System.out.print("You: ");
                String serverResponse = reader.readLine();

                if ("exit".equalsIgnoreCase(serverResponse)) {
                    writer.println("exit");
                    System.out.println("Closing connection...");
                    break;
                }

                writer.println(serverResponse);
            }
        } catch (IOException e) {
            System.err.println("TCP Server Error: " + e.getMessage());
        }
    }

    private static void runUdpClient(String address, int port) {
        try (DatagramSocket socket = new DatagramSocket();
             Scanner scanner = new Scanner(System.in)) {

            InetAddress serverAddress = InetAddress.getByName(address);
            System.out.println("Connected to " + address + ":" + port);
            System.out.println("Type 'exit' to close the connection.");

            byte[] buffer = new byte[BUFFER_SIZE];
            while (true) {
                System.out.print("You: ");
                String userInput = scanner.nextLine();

                if ("exit".equalsIgnoreCase(userInput)) {
                    System.out.println("Closing connection...");
                    break;
                }

                buffer = userInput.getBytes();
                DatagramPacket sendPacket = new DatagramPacket(buffer, buffer.length, serverAddress, port);
                socket.send(sendPacket);

                DatagramPacket receivePacket = new DatagramPacket(new byte[BUFFER_SIZE], BUFFER_SIZE);
                socket.receive(receivePacket);
                String serverResponse = new String(receivePacket.getData(), 0, receivePacket.getLength());
                System.out.println("Server: " + serverResponse);
            }
        } catch (IOException e) {
            System.err.println("UDP Client Error: " + e.getMessage());
        }
    }

    private static void runUdpServer(int port) {
        try (DatagramSocket socket = new DatagramSocket(port)) {
            System.out.println("Listening on port " + port + "...");
            byte[] buffer = new byte[BUFFER_SIZE];

            while (true) {
                DatagramPacket receivePacket = new DatagramPacket(buffer, BUFFER_SIZE);
                socket.receive(receivePacket);

                String clientInput = new String(receivePacket.getData(), 0, receivePacket.getLength());
                System.out.println("Client: " + clientInput);

                if ("exit".equalsIgnoreCase(clientInput)) {
                    System.out.println("Closing connection...");
                    break;
                }

                System.out.print("You: ");
                BufferedReader reader = new BufferedReader(new InputStreamReader(System.in));
                String serverResponse = reader.readLine();

                if ("exit".equalsIgnoreCase(serverResponse)) {
                    byte[] responseData = "exit".getBytes();
                    socket.send(new DatagramPacket(responseData, responseData.length, receivePacket.getAddress(), receivePacket.getPort()));
                    System.out.println("Closing connection...");
                    break;
                }

                byte[] responseData = serverResponse.getBytes();
                DatagramPacket sendPacket = new DatagramPacket(responseData, responseData.length, receivePacket.getAddress(), receivePacket.getPort());
                socket.send(sendPacket);
            }
        } catch (IOException e) {
            System.err.println("UDP Server Error: " + e.getMessage());
        }
    }
}
