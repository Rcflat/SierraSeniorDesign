
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

import socket
import json
import time
import cv2

def send_data_over_wifi(data, arduino_ip, arduino_port):
    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    message = json.dumps(data).encode('utf-8')
    sock.sendto(message, (arduino_ip, arduino_port))
    sock.close()

def list_available_cameras():
    """Lists all available camera indices."""
    available_cameras = []
    for i in range(10):  # Check the first 10 indices
        cap = cv2.VideoCapture(i)
        if cap.isOpened():
            available_cameras.append(i)
            cap.release()
    return available_cameras

def initialize_camera():
    # Check for available cameras
    available_cameras = list_available_cameras()
    if not available_cameras:
        print("Error: No available cameras found.")
        return None

    print(f"Available cameras: {available_cameras}")

    # Try to open the first available camera
    camera_index = available_cameras[2]
    cap = cv2.VideoCapture(camera_index)

    if not cap.isOpened():
        print(f"Error: Could not open camera at index {camera_index}.")
        return None

    print(f"Camera initialized at index {camera_index}.")
    return cap

def toggle_recording(is_recording, frame):
    global out
    if not is_recording:
        # Start recording
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        out = cv2.VideoWriter('output.avi', fourcc, 20.0, (frame.shape[1], frame.shape[0]))
        print("Recording started.")
        return True
    else:
        # Stop recording
        out.release()
        print("Recording stopped.")
        return False

def take_screenshot(frame):
    cv2.imwrite('screenshot.png', frame)
    print("Screenshot saved!")

if __name__ == "__main__":
    from capture_input import capture_input
    from connect_controller import connect_controller

    arduino_ip = "192.168.1.50"  # Replace with your Arduino's IP address
    arduino_port = 8888  # Replace with your Arduino's port number

    # Initialize joystick and camera
    joystick = connect_controller()
    cap = initialize_camera()

    is_recording = False
    out = None

    while cap and cap.isOpened():
        # Handle camera functionality separately
        ret, frame = cap.read()
        if not ret:
            print("Error: Could not read frame.")
            break

        # Display the resulting frame
        cv2.imshow('Camo Camera', frame)

        # If recording, save frames
        if is_recording and out is not None:
            out.write(frame)

        # Always capture and send joystick inputs
        if joystick:
            inputs = capture_input(joystick)
            send_data_over_wifi(inputs, arduino_ip, arduino_port)

            # Button 6: 'Menu' button to toggle recording
            if inputs['button_6'] == 1 and cap:
                is_recording = toggle_recording(is_recording, frame)

            # Button 4: 'Select' button to take a screenshot
            if inputs['button_4'] == 1 and cap:
                take_screenshot(frame)

        # Break the loop if 'q' is pressed
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break

        time.sleep(0.1)

    # Release resources
    if cap:
        cap.release()
    if out:
        out.release()
    cv2.destroyAllWindows()