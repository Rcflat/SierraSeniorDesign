import cv2

# Open the virtual webcam (Camo should be running)
cap = cv2.VideoCapture(0)  # Replace '0' with the correct index if multiple webcams are available

# Check if the camera opened successfully
if not cap.isOpened():
    print("Error: Could not open camera.")
    exit()

# Define the codec and create a VideoWriter object to save video
fourcc = cv2.VideoWriter_fourcc(*'XVID')
out = cv2.VideoWriter('output.avi', fourcc, 20.0, (640, 480))

while True:
    # Capture frame-by-frame
    ret, frame = cap.read()
    if not ret:
        print("Error: Could not read frame.")
        break

    # Display the resulting frame
    cv2.imshow('Camo Camera', frame)

    # Save the current frame to a video file
    out.write(frame)

    # Press 's' to save a screenshot, 'q' to quit
    key = cv2.waitKey(1) & 0xFF
    if key == ord('s'):
        cv2.imwrite('screenshot.png', frame)  # Save screenshot
        print("Screenshot saved!")
    elif key == ord('q'):
        break

# Release everything when done
cap.release()
out.release()
cv2.destroyAllWindows()
