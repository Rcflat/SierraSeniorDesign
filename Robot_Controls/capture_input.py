import pygame
import time


def connect_controller():
    joystick = None
    try:
        # Attempt to connect to the first joystick
        joystick = pygame.joystick.Joystick(0)
        joystick.init()
        print(f"Connected to {joystick.get_name()}")
    except pygame.error:
        print("No DualSense controller found. Make sure it's connected via Bluetooth.")
    
    return joystick

def capture_input(joystick):
    pygame.event.pump()
    
    # Create a dictionary to store button states
    button_states = {}
    
    # Capture buttons
    for i in range(joystick.get_numbuttons()):
        button_states[f"button_{i}"] = joystick.get_button(i)
    
    # Capture axes (e.g., sticks and triggers)
    for i in range(joystick.get_numaxes()):
        button_states[f"axis_{i}"] = joystick.get_axis(i)
    
    return button_states

if __name__ == "__main__":
    pygame.init()
    pygame.joystick.init()
    joystick = connect_controller()
    while True:
        if joystick:
            inputs = capture_input(joystick)
            print(inputs)
        time.sleep(0.1)  # Delay to avoid high CPU usage
