#!/usr/bin/python           # This is server.py file

import socket               # Import socket module
import thread

def on_new_client(clientsocket,addr):
    while True:
        data = clientsocket.recv(1024) 
        if data:
            print addr[0], ' >> ', data 
            #clientsocket.send(msg) 
    clientsocket.close()

server = socket.socket()         # Create a socket object
host = socket.gethostname() # Get local machine name
port = 500                  # Reserve a port for your service.

print 'Server started!'
print 'Waiting for clients...'

server.bind((host, port))        # Bind to the port
server.listen(5)                 # Now wait for client connection.

while True:
   c, addr = server.accept()     # Establish connection with client.
   thread.start_new_thread(on_new_client,(c,addr))
server.close()
