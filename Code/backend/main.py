import asyncio
import websockets
from io import BytesIO
from PIL import Image, UnidentifiedImageError
import os
import datetime
import cv2
import numpy as np

HOST = "0.0.0.0"
PORT = 3000

cwd = os.getcwd()


def is_valid_image(bytes):
    try:
        Image.open(BytesIO(bytes))
        return True
    except UnidentifiedImageError:
        print("\nError: image invalid\n")
        return False


def store_image(bytes, id):
    time = datetime.datetime.now()
    if not os.path.exists(f"/uploads/{id}/{time}.jpeg"):
        with open(f"/uploads/{id}/{time}.jpeg", "wb") as f:
            f.write(bytes)
    else:
        counter = 1
        added = False
        while added is False:
            time = datetime.datetime.now()
            if not os.path.exists(f"/uploads/{id}/{time}-{counter}.jpeg"):
                added = True
                with open(f"/uploads/{id}/{time}-{counter}.jpeg", "wb") as f:
                    f.write(bytes)
            else:
                counter += 1


def check_for_change(bytes, id):
    file_names = []

    for entry in os.listdir(f"{cwd}/uploads/{id}"):
        if os.path.isfile(f"{cwd}/uploads/{id}/{entry}"):
            file_names.append(entry)

    file_names = [datetime.strptime(dt, "%Y-%m-%d %H:%M") for dt in file_names]
    file_names = sorted(file_names, reverse=True)

    old_image = cv2.imread(f"{cwd}/uploads/{id}/{str(file_names[0])}.jpeg")

    nparr = np.fromstring(bytes, np.uint8)
    image = cv2.imdecode(
        nparr, cv2.CV_LOAD_IMAGE_COLOR  # cv2.IMREAD_COLOR in OpenCV 3.1
    )

    difference = cv2.absdiff(image, old_image)

    total_difference = np.sum(difference)

    max_intensity = 255
    num_pixels = image.shape[0] * image.shape[1] * image.shape[2]
    total_possible_difference = num_pixels * max_intensity

    percentage = (total_difference / total_possible_difference) * 100

    print(percentage)

    cv2.imshow("Latest image", image)

    while True:
        if cv2.waitKey(10) & 0xFF == ord("q") or cv2.waitKey(0):
            break

    cv2.destroyAllWindows()
    
    if percentage > 2.5:
        return True
    return False


async def handle_connection(websocket, path):
    while True:
        try:
            message = await websocket.recv()
            print(f"Length of image: {len(message)}")
            if len(message) > 5000 and is_valid_image(message) and check_for_change(message, 1234):
                store_image(message, 1234)
            else:
                print(f"Received message from client: {message}.")

        except websockets.exceptions.ConnectionClosed:
            print("Connection closed.")
            break


async def main():
    print(f"Server starting at ws://{HOST}:{PORT}...")
    server = await websockets.serve(handle_connection, HOST, PORT)
    await server.wait_closed()


if __name__ == "__main__":
    asyncio.run(main())
