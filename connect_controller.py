import pygame

# Initialize pygame
pygame.init()

# Initialize the joystick
pygame.joystick.init()

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

if __name__ == "__main__":
    connect_controller()
