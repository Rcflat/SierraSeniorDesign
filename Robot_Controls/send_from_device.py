import socket
import json
import time

# Axis 0: Left Analog Stick (-1 = LEFT, 1 = RIGHT)
# Axis 1: Left Analog Stick (-1 = UP, 1 = DOWN)
# Axis 2: Right Analog Stick (-1 = UP, 1 = DOWN)
# Axis 3: Right Analog Stick (-1 = UP, 1 = DOWN)
# Axis 4: Back Left Trigger (-1 = Released, 1 = Pressed)
# Axis 5: Back Right Trigger (-1 = Released, 1 = Pressed)

# Button 0: X Button (0 = Released, 1 = Pressed)
# Button 1: Circle Button (0 = Released, 1 = Pressed)
# Button 2: Square Button (0 = Released, 1 = Pressed)
# Button 3: Triangle Button (0 = Released, 1 = Pressed)
# Button 4: Select Button (0 = Released, 1 = Pressed)
# Button 5: Playstation Button (0 = Released, 1 = Pressed)
# Button 6: Menu Button (0 = Released, 1 = Pressed)
# Button 9: Back Left Button (0 = Released, 1 = Pressed)
# Button 10: Back Right Button (0 = Released, 1 = Pressed)
# Button 11: DPAD_UP (0 = Released, 1 = Pressed)
# Button 12: DPAD_DOWN (0 = Released, 1 = Pressed)
# Button 13: DPAD_LEFT (0 = Released, 1 = Pressed)
# Button 14: DPAD_RIGHT (0 = Released, 1 = Pressed)
# Button 15: Center Button, Big Panel (0 = Released, 1 = Pressed)

def send_data_over_wifi(data, arduino_ip, arduino_port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    message = json.dumps(data).encode('utf-8')
    sock.sendto(message, (arduino_ip, arduino_port))
    sock.close()

if __name__ == "__main__":
    from Robot_Controls.capture_input import capture_input
    from Robot_Controls.connect_controller import connect_controller
    
    arduino_ip = "192.168.1.50"  # Replace with your Arduino's IP address
    arduino_port = 8888  # Replace with your Arduino's port number
    
    joystick = connect_controller()
    
    while True:
        if joystick:
            inputs = capture_input(joystick)
            print(inputs)
            send_data_over_wifi(inputs, arduino_ip, arduino_port)
        time.sleep(0.1)
