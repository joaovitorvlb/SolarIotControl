import socket
import time
server = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

host = socket.gethostname()                        # Get local machine name
port = 500

cont = 0
tag_pct = []
i_00 = []
i_01 = []
i_02 = []
i_03 = []

server.bind((host, port))
server.listen(5)

print 'Bind hostname'
print 'Waiting client socket'
print '...'
while True:
    client, add = server.accept()                  #Receives connection parameters
    data = client.recv(1024)                       #Wait for some data and print
    if data:
        print ' '
        print 'Received from ', add[0], ' >> ', data
        data = data.strip()
        tag_pct, i_00, i_01, i_02, i_03 = data.split(':')
        if tag_pct == 'ESP01-IIII':
            print 'Processing package'
            print 'Package tag   ', tag_pct
            print 'Load device 1 ', i_00, ' mA'
            print 'Load device 2 ', i_01, ' mA'
            print 'Load device 3 ', i_02, ' mA'
            print 'Load device 4 ', i_03, ' mA'
        if tag_pct == 'ESP02-VVVV':
            print 'Processing package'
            print 'Package tag       ', tag_pct
            print 'Device 01 voltage ', i_00, ' V'
            print 'Device 02 voltage ', i_01, ' V'
            print 'Device 03 voltage ', i_02, ' V'
            print 'Devide 04 voltage ', i_03, ' V' 

        cont = cont + 1
server.close()
