import asyncio
import websockets
from io import BytesIO
from PIL import Image, UnidentifiedImageError
import os
import datetime

IP = "0.0.0.0"
PORT = 3000


def is_valid_image(image_bytes):
    try:
        Image.open(BytesIO(image_bytes))
        return True
    except UnidentifiedImageError:
        print("\nError: image invalid\n")
        return False


async def handle_connection(websocket, path):
    while True:
        try:
            message = await websocket.recv()
            print(f"Length of image: {len(message)}")
            if len(message) > 5000:
                if is_valid_image(message):
                    time = datetime.datetime.now()
                    if not os.path.exists(f"/uploads/{time}.jpeg"):
                        with open(f"/uploads/{time}.jpeg", "wb") as f:
                            f.write(message)
                    else:
                        counter = 1
                        added = False
                        while added is False:
                            time = datetime.datetime.now()
                            if not os.path.exists(f"/uploads/{time}-{counter}.jpeg"):
                                added = True
                                with open(f"/uploads/{time}-{counter}.jpeg", "wb") as f:
                                    f.write(message)
                            else:
                                counter += 1
            else:
                print(f"Received message from client: {message}.")

        except websockets.exceptions.ConnectionClosed:
            break


async def main():
    print(f"Server starting at ws://{IP}:{PORT}...")
    server = await websockets.serve(handle_connection, IP, PORT)
    await server.wait_closed()


if __name__ == "__main__":
    asyncio.run(main())
