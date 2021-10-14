import socket
import serial

arduino = serial.Serial(port='/dev/ttyACM0', baudrate=9600)
arduino.timeout = 1

def ParseData(data):
    stringData = data.decode('utf-8')
    print(f"Received: {stringData}")
    try:
        arduino.write(data)
    except:
        print("Failed to write to serial!")
        

def RunSocketServer():
    LOCAL_IP = '192.168.1.113'
    PORT = 9001 # Non-privileged ports are > 1023)

    # Create a socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

    # Ensure that you can restart server quickly when it terminates
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.bind((LOCAL_IP, PORT))
    sock.listen()
    print(f"Started SocketServer at {LOCAL_IP}:{PORT}!")
    try:
        while True:
            newConnection, address = sock.accept()
            with newConnection:
                print(f"Connected by {address}")
                while True:
                    receivedData = newConnection.recv(1024)
                    if not receivedData:
                        break
                    
                    ParseData(receivedData)
                    
                    #Echo back received data.
                    newConnection.sendall(receivedData)
    finally:
        sock.close()

RunSocketServer()