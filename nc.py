import socket
import sys

BUFFER_SIZE = 1024

def error(msg):
    print(f"Error: {msg}")
    sys.exit(1)

def run_client(address, port, use_udp):
    sock_type = socket.SOCK_DGRAM if use_udp else socket.SOCK_STREAM
    with socket.socket(socket.AF_INET, sock_type) as sock:
        server_addr = (address, port)

        if not use_udp:  # For TCP, establish a connection
            try:
                sock.connect(server_addr)
            except Exception as e:
                error(f"Connection failed: {e}")
            print(f"Connected to {address}:{port}")
        
        print("Type 'exit' to close the connection.")
        while True:
            # Get user input
            msg = input("You: ")
            if msg.lower() == 'exit':
                print("Closing connection...")
                break

            # Send data
            try:
                if use_udp:
                    sock.sendto(msg.encode(), server_addr)
                else:
                    sock.sendall(msg.encode())
                
                # Receive response
                if use_udp:
                    data, _ = sock.recvfrom(BUFFER_SIZE)
                else:
                    data = sock.recv(BUFFER_SIZE)
                
                if not data:
                    print("Connection closed by server")
                    break
                
                print(f"Server: {data.decode()}")
            except Exception as e:
                error(f"Error during communication: {e}")
                break

def run_server(port, use_udp):
    sock_type = socket.SOCK_DGRAM if use_udp else socket.SOCK_STREAM
    with socket.socket(socket.AF_INET, sock_type) as sock:
        server_addr = ('', port)
        try:
            sock.bind(server_addr)
        except Exception as e:
            error(f"Bind failed: {e}")

        if not use_udp:
            sock.listen(5)
            print(f"Listening on port {port}...")
            conn, client_addr = sock.accept()
            with conn:
                print(f"Connected to {client_addr}")
                print("Type 'exit' to close the connection.")
                while True:
                    try:
                        data = conn.recv(BUFFER_SIZE)
                        if not data:
                            print("Connection closed by client")
                            break
                        print(f"Client: {data.decode()}")
                        
                        # Get server response
                        msg = input("You: ")
                        if msg.lower() == 'exit':
                            print("Closing connection...")
                            conn.sendall(msg.encode())
                            break
                        conn.sendall(msg.encode())
                    except Exception as e:
                        error(f"Error during communication: {e}")
                        break
        else:
            print(f"Listening for UDP on port {port}...")
            while True:
                try:
                    data, client_addr = sock.recvfrom(BUFFER_SIZE)
                    if not data:
                        print("Connection closed by client")
                        break
                    print(f"Client: {data.decode()}")

                    # Get server response
                    msg = input("You: ")
                    if msg.lower() == 'exit':
                        print("Closing connection...")
                        sock.sendto(msg.encode(), client_addr)
                        break
                    sock.sendto(msg.encode(), client_addr)
                except Exception as e:
                    error(f"Error during communication: {e}")
                    break

def main():
    if len(sys.argv) < 4:
        print("Usage: python script.py -l <port> udp|tcp (for server) or python script.py <address> <port> udp|tcp (for client)")
        sys.exit(1)

    use_udp = sys.argv[3].lower() == 'udp'

    if sys.argv[1] == '-l':
        port = int(sys.argv[2])
        run_server(port, use_udp)
    else:
        address = sys.argv[1]
        port = int(sys.argv[2])
        run_client(address, port, use_udp)

if __name__ == "__main__":
    main()
