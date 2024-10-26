package main

import (
	"bufio"
	"fmt"
	"net"
	"os"
	"strings"
)

const bufferSize = 1024

func error(msg string) {
	fmt.Println(msg)
	os.Exit(1)
}

func runClient(address string, port string) {
	conn, err := net.Dial("tcp", address+":"+port)
	if err != nil {
		error("Connection failed: " + err.Error())
	}
	defer conn.Close()

	fmt.Printf("Connected to %s:%s\n", address, port)
	fmt.Println("Type 'exit' to close the connection.")

	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Print("You: ")
		userInput, _ := reader.ReadString('\n')
		userInput = strings.TrimSpace(userInput)

		if userInput == "exit" {
			fmt.Println("Closing connection...")
			break
		}

		_, err = conn.Write([]byte(userInput + "\n"))
		if err != nil {
			error("Send failed: " + err.Error())
		}

		serverResponse := make([]byte, bufferSize)
		n, err := conn.Read(serverResponse)
		if err != nil {
			fmt.Println("Connection closed by server")
			break
		}

		fmt.Printf("Server: %s", string(serverResponse[:n]))
	}
}

func runServer(port string) {
	listener, err := net.Listen("tcp", ":"+port)
	if err != nil {
		error("Listen failed: " + err.Error())
	}
	defer listener.Close()

	fmt.Printf("Listening on port %s...\n", port)

	conn, err := listener.Accept()
	if err != nil {
		error("Accept failed: " + err.Error())
	}
	defer conn.Close()

	fmt.Println("Connected to client")
	fmt.Println("Type 'exit' to close the connection.")

	reader := bufio.NewReader(os.Stdin)

	for {
		clientMessage := make([]byte, bufferSize)
		n, err := conn.Read(clientMessage)
		if err != nil {
			fmt.Println("Connection closed by client")
			break
		}

		message := strings.TrimSpace(string(clientMessage[:n]))
		fmt.Printf("Client: %s\n", message)

		if message == "exit" {
			fmt.Println("Closing connection...")
			break
		}

		fmt.Print("You: ")
		userInput, _ := reader.ReadString('\n')
		userInput = strings.TrimSpace(userInput)

		if userInput == "exit" {
			fmt.Println("Closing connection...")
			conn.Write([]byte(userInput + "\n"))
			break
		}

		_, err = conn.Write([]byte(userInput + "\n"))
		if err != nil {
			error("Send failed: " + err.Error())
		}
	}
}

func runUDPClient(address string, port string) {
	conn, err := net.Dial("udp", address+":"+port)
	if err != nil {
		error("Connection failed: " + err.Error())
	}
	defer conn.Close()

	fmt.Printf("Connected to %s:%s\n", address, port)
	fmt.Println("Type 'exit' to close the connection.")

	reader := bufio.NewReader(os.Stdin)

	for {
		fmt.Print("You: ")
		userInput, _ := reader.ReadString('\n')
		userInput = strings.TrimSpace(userInput)

		if userInput == "exit" {
			fmt.Println("Closing connection...")
			break
		}

		_, err = conn.Write([]byte(userInput + "\n"))
		if err != nil {
			error("Send failed: " + err.Error())
		}

		serverResponse := make([]byte, bufferSize)
		n, err := conn.Read(serverResponse)
		if err != nil {
			fmt.Printf("Connection closed by server, err: %v\n", err)
			break
		}

		fmt.Printf("Server: %s", string(serverResponse[:n]))
	}
}

func runUDPServer(port string) {
	addr, err := net.ResolveUDPAddr("udp", ":"+port)
	if err != nil {
		error("Resolve failed: " + err.Error())
	}

	conn, err := net.ListenUDP("udp", addr)
	if err != nil {
		error("Listen failed: " + err.Error())
	}
	defer conn.Close()

	fmt.Printf("Listening on port %s...\n", port)

	clientAddr := &net.UDPAddr{}
	buffer := make([]byte, bufferSize)
	reader := bufio.NewReader(os.Stdin)

	for {
		n, addr, err := conn.ReadFromUDP(buffer)
		if err != nil {
			fmt.Println("Connection closed by client")
			break
		}

		message := strings.TrimSpace(string(buffer[:n]))
		clientAddr = addr
		fmt.Printf("Client: %s\n", message)

		if message == "exit" {
			fmt.Println("Closing connection...")
			break
		}

		fmt.Print("You: ")
		userInput, _ := reader.ReadString('\n')
		userInput = strings.TrimSpace(userInput)

		if userInput == "exit" {
			fmt.Println("Closing connection...")
			conn.WriteToUDP([]byte(userInput+"\n"), clientAddr)
			break
		}

		_, err = conn.WriteToUDP([]byte(userInput+"\n"), clientAddr)
		if err != nil {
			error("Send failed: " + err.Error())
		}
	}
}

func main() {
	if len(os.Args) < 4 {
		fmt.Println("Usage: <program> -l <port> udp|tcp (for server) or <program> <address> <port> udp|tcp (for client)")
		os.Exit(1)
	}

	isUDP := os.Args[3] == "udp"

	if os.Args[1] == "-l" {
		port := os.Args[2]
		if isUDP {
			runUDPServer(port)
		} else {
			runServer(port)
		}
	} else {
		address := os.Args[1]
		port := os.Args[2]
		if isUDP {
			runUDPClient(address, port)
		} else {
			runClient(address, port)
		}
	}
}

