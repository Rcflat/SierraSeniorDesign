
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

import socket
import json
import time
import cv2
import threading

def send_data_over_wifi(data, arduino_ip, arduino_port, retries=3):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    message = json.dumps(data).encode('utf-8')
    for attempt in range(retries):
        try:
            sock.sendto(message, (arduino_ip, arduino_port))
            print(f"Packet sent to {arduino_ip}:{arduino_port}")

            # Wait for acknowledgment
            sock.settimeout(1.0)  # Set a timeout for receiving the acknowledgment
            try:
                response, addr = sock.recvfrom(1024)
                print(f"Acknowledgment received from {addr}: {response.decode('utf-8')}")
                break  # Exit if acknowledgment received
            except socket.timeout:
                print("No acknowledgment received, retrying...")
                time.sleep(0.5)
        except socket.error as e:
            print(f"Attempt {attempt + 1} failed to send packet: {e}")
            time.sleep(0.5)
    else:
        print("Failed to send packet after multiple attempts.")
    sock.close()

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

def receive_data(udp_port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    sock.bind(("", udp_port))
    print(f"Listening for data on port {udp_port}...")
    while True:
        data, addr = sock.recvfrom(1024)
        try:
            json_data = json.loads(data.decode('utf-8'))
            print(f"Received data: {json_data}")
        except json.JSONDecodeError:
            print("Error: Received malformed JSON data.")

if __name__ == "__main__":
    from capture_input import capture_input
    from connect_controller import connect_controller

    arduino_ip = "192.168.1.50"  # Static IP of the ESP32
    arduino_port = 8888
    udp_port = 8888

    joystick = connect_controller()
    cap = initialize_camera()
    is_recording = False
    out = None

    # Start a separate thread for receiving data
    receive_thread = threading.Thread(target=receive_data, args=(udp_port,), daemon=True)
    receive_thread.start()

    while True:
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

            if joystick:
                inputs = capture_input(joystick)
                
                # Button 6: 'Menu' button to toggle recording
                if inputs['button_6'] == 1:
                    is_recording = toggle_recording(is_recording, frame)

                # Button 4: 'Select' button to take a screenshot
                if inputs['button_4'] == 1:
                    take_screenshot(frame)

        # Capture and send joystick inputs
        if joystick:
            inputs = capture_input(joystick)
            send_data_over_wifi(inputs, arduino_ip, arduino_port)

        # Break the loop if 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        time.sleep(0.1)

    # Cleanup resources
    if cap:
        cap.release()
    if out:
        out.release()
    cv2.destroyAllWindows()
