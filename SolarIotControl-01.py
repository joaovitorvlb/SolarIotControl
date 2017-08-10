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
        print 'Desmenbrando pacote'
        print 'Tag do pacote ', tag_pct
        print 'Load device 1 ', i_00, ' mA'
        print 'Load device 2 ', i_01, ' mA'
        print 'Load device 3 ', i_02, ' mA'
        print 'Load device 4 ', i_03, ' mA'

        cont = cont + 1
server.close()
