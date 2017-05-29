from time import sleep
import threading
import logging

from flask import Flask, render_template, request
import serial
import serial.tools.list_ports

app = Flask(__name__)
arduino = None
ack = bytes(chr(6), "ASCII")
lock = threading.Lock()


def auto_reconnect():
    global arduino
    if arduino:
        arduino.close()
    arduino = connect_arduino()
    logging.info("[SERIAL] Attempted connection, success: " +
                 str(bool(arduino)))
    print("[SERIAL] Attempted connection, success:", bool(arduino))
    arduino or threading.Timer(5, auto_reconnect).start()


def connect_arduino():
    ports = [obj.device for obj in serial.tools.list_ports.comports()
             if "Arduino" in obj.description]
    if not ports:
        return
    try:
        return serial.Serial(ports[0])
    except serial.SerialException:
        return


@app.route('/', methods=['POST', 'GET'])
def handle_data():
    print("[REQUEST]", request.method)
    if request.method == 'POST':
        threading.Thread(target=send_serial_data,
                         args=(request.form["cmd"],),
                         daemon=True).start()
        return "handled"
    return render_template('index.html')


def nb_read(device):
    return device.read() if device.inWaiting() else ''


def send_serial_data(data):
    if arduino and lock.acquire(False):
        try:
            first = True
            tries = 0
            while first or nb_read(arduino) != ack:
                first = False
                tries += 1
                arduino.write(bytes(data, 'ASCII'))
                arduino.flush()
                sleep(0.1)
                if tries > 20:
                    logging.error("[SERIAL] Timed out!")
                    raise Exception("[SERIAL] Timed out!")
            arduino.flushInput()
            logging.info("[SERIAL] Sent command & got ack")
            print("[SERIAL] Sent command & got ack")
        except (serial.SerialException, Exception) as e:
            print(e)
            threading.Thread(target=auto_reconnect, daemon=True).start()
            logging.warning("[SERIAL] Unable to send command or receive ack, Arduino may have been disconnected or not correctly programmed")
            print("[SERIAL] Unable to send command or receive ack, Arduino may have been disconnected or not correctly programmed")
        lock.release()


def main():
    logging.basicConfig(format='%(asctime)s [%(levelname)s] %(message)s',
                        filename='server.log',
                        level=logging.DEBUG,
                        filemode='w+',
                        datefmt='%I:%M:%S %p')
    print("[SERVER] Started")
    threading.Thread(target=auto_reconnect, daemon=True).start()
    app.run(host='0.0.0.0', port=8080)

if __name__ == "__main__":
    main()
