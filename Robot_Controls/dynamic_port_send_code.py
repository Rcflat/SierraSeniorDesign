import socket
import time
import cv2
from connect_controller import connect_controller
from capture_input import capture_input
from pygrabber.dshow_graph import FilterGraph

# - Axis 0: Left Analog Stick (-1 = LEFT, 1 = RIGHT)
# - Axis 1: Left Analog Stick (-1 = UP, 1 = DOWN)
# - Axis 2: Right Analog Stick (-1 = UP, 1 = DOWN)
# - Axis 3: Right Analog Stick (-1 = UP, 1 = DOWN)
# - Axis 4: Back Left Trigger (-1 = Released, 1 = Pressed)
# - Axis 5: Back Right Trigger (-1 = Released, 1 = Pressed)
# - Button 0: X Button (0 = Released, 1 = Pressed)
# - Button 1: Circle Button (0 = Released, 1 = Pressed)
# - Button 2: Square Button (0 = Released, 1 = Pressed)
# - Button 3: Triangle Button (0 = Released, 1 = Pressed)
# - Button 4: Select Button (0 = Released, 1 = Pressed)
# - Button 5: Playstation Button (0 = Released, 1 = Pressed)
# - Button 6: Menu Button (0 = Released, 1 = Pressed)
# - Button 9: Back Left Button (0 = Released, 1 = Pressed)
# - Button 10: Back Right Button (0 = Released, 1 = Pressed)
# - Button 11: DPAD_UP (0 = Released, 1 = Pressed)
# - Button 12: DPAD_DOWN (0 = Released, 1 = Pressed)
# - Button 13: DPAD_LEFT (0 = Released, 1 = Pressed)
# - Button 14: DPAD_RIGHT (0 = Released, 1 = Pressed)
# - Button 15: Center Button, Big Panel (0 = Released, 1 = Pressed)

def take_screenshot(frame):
    cv2.imwrite('screenshot.png', frame)
    print("Screenshot saved!")

def toggle_recording(is_recording, frame):
    global out
    if not is_recording:
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        out = cv2.VideoWriter('output.avi', fourcc, 20.0, (frame.shape[1], frame.shape[0]))
        print("Recording started.")
        return True
    else:
        out.release()
        print("Recording stopped.")
        return False
    
def list_available_cameras():
    available_cameras = []
    for i in range(10):
        cap = cv2.VideoCapture(i)
        if cap.isOpened():
            available_cameras.append(i)
            cap.release()
    return available_cameras

def initialize_camera():
    available_cameras = list_available_cameras()
    if not available_cameras:
        print("Warning: No available cameras found.")
        return None
    print(f"Available cameras: {available_cameras}")
    camera_index = available_cameras[0]
    cap = cv2.VideoCapture(camera_index)
    if not cap.isOpened():
        print(f"Warning: Could not open camera at index {camera_index}.")
        return None
    print(f"Camera initialized at index {camera_index}.")
    return cap

def list_available_cameras():
    graph = FilterGraph()
    devices = graph.get_input_devices()
    cameras = {index: name for index, name in enumerate(devices)}
    return cameras

    
def initialize_camera():
    available_cameras = list_available_cameras()
    if not available_cameras:
        print("Warning: No available cameras found.")
        return None

    print("Available cameras:")
    for index, name in available_cameras.items():
        print(f"Index {index}: {name}")

    # Attempt to find the "Camo" camera
    camera_index = None
    for index, name in available_cameras.items():
        if "Camo" in name:
            camera_index = index
            break
    
    # If "Camo" is not found, use the first available camera
    if camera_index is None:
        print("Camo camera not found. Using the first available camera instead.")
        camera_index = list(available_cameras.keys())[0]

    cap = cv2.VideoCapture(camera_index)
    if not cap.isOpened():
        print(f"Warning: Could not open camera at index {camera_index}.")
        return None

    print(f"Camera initialized at index {camera_index} - {available_cameras[camera_index]}")
    return cap

def perform_handshake(sock, esp32_ip, discovery_port):
    print("Starting handshake...")

    # Send a discovery message to the ESP32 on the discovery port
    discovery_message = "DISCOVERY"
    sock.sendto(discovery_message.encode(), (esp32_ip, discovery_port))
    print(f"Sent discovery message to {esp32_ip}:{discovery_port}")

    # Wait for the ESP32 to respond with a proposed port
    sock.settimeout(5)  # Timeout in seconds
    try:
        response, addr = sock.recvfrom(1024)
        proposed_port = int(response.decode())
        print(f"Received proposed port: {proposed_port}")

        # Send acknowledgment back to the ESP32
        ack_message = f"ACK:{proposed_port}"
        sock.sendto(ack_message.encode(), (esp32_ip, discovery_port))
        print(f"Acknowledged proposed port: {proposed_port}")
        return proposed_port
    except socket.timeout:
        print("Handshake timed out.")
        return None

def main():
    # ESP32 IP address and discovery port
    esp32_ip = "192.168.0.50"  # Replace with the actual IP address of your ESP32
    discovery_port = 5000
    reset_timeout = 10  # Timeout duration (seconds) before resetting to discovery port
    last_received_time = time.time()

    # Create a UDP socket
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(('', discovery_port))
    cap = initialize_camera()
    joystick = connect_controller()

    # Perform the handshake to get the agreed port
    agreed_port = perform_handshake(sock, esp32_ip, discovery_port)
    if agreed_port:
        # Switch to the agreed port for further communication
        sock.close()
        sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        sock.connect((esp32_ip, agreed_port))
        print(f"Switched to agreed port: {agreed_port}")

        try:
            is_recording = False  # Initialize is_recording

            while True:
                # Controller Logic
                if joystick:
                    inputs = capture_input(joystick)
                    
                    # Send the inputs to the ESP32
                    message = str(inputs)  # Convert inputs to string
                    packet_size = len(message.encode())  # Get the size of the packet in bytes
                    sock.send(message.encode())
                    print(f"Sent test message: {message}")
                    print(f"Packet Size: {packet_size}")

                    # Button 6: 'Menu' button to toggle recording
                    if inputs['button_6'] == 1 and cap and cap.isOpened():
                        is_recording = toggle_recording(is_recording, frame)

                    # Button 4: 'Select' button to take a screenshot
                    if inputs['button_4'] == 1 and cap and cap.isOpened():
                        take_screenshot(frame)

                # Camera handling
                if cap and cap.isOpened():
                    ret, frame = cap.read()
                    if not ret:
                        print("Warning: Could not read frame, retrying...")
                        time.sleep(1)
                        continue

                    # If recording, save frames
                    if is_recording and out is not None:
                        out.write(frame)

                # Listen for a response with a timeout period
                sock.settimeout(0.1)  # 100 ms timeout for response
                try:
                    response, addr = sock.recvfrom(1024)
                    print(f"Received from ESP32: {response.decode()}")
                    last_received_time = time.time()  # Update the last received time
                except socket.timeout:
                    print("No response from ESP32.")
                
                # Check if the timeout period has passed since the last received packet
                if time.time() - last_received_time > reset_timeout:
                    print("No response received for a while. Resetting to discovery port...")
                    sock.close()  # Close current connection
                    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)  # Create a new socket
                    sock.bind(('', discovery_port))  # Bind to discovery port again
                    agreed_port = perform_handshake(sock, esp32_ip, discovery_port)
                    if agreed_port:
                        sock.connect((esp32_ip, agreed_port))  # Reconnect to the agreed port
                        print(f"Switched to agreed port: {agreed_port}")
                    last_received_time = time.time()  # Reset the timer after reconnecting

        except KeyboardInterrupt:
            print("Stopped by user.")
        finally:
            sock.close()
            if cap:
                cap.release()
            if 'out' in globals() and out:
                out.release()
    else:
        print("Failed to establish communication with the ESP32.")

if __name__ == "__main__":
    main()