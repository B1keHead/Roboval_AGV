import threading

from flask import Flask, render_template, request
import serial
import serial.tools.list_ports

app = Flask(__name__)
arduino = None
connected = False


def auto_reconnect():
    global arduino
    global connected
    if not connected:
        if arduino:
            arduino.close()
        arduino = connect_arduino()
        connected = bool(arduino)
        print("Attempted connection, success:", connected)
    threading.Timer(10, auto_reconnect).start()


def connect_arduino():
    ports = [obj.device for obj in serial.tools.list_ports.comports() if "Arduino" in obj.manufacturer]
    if not ports:
        return
    try:
        return serial.Serial(ports[0])
    except serial.SerialException:
        return


@app.route('/', methods=['POST', 'GET'])
def handle_data():
    if request.method == 'POST':
        if connected:
            try:
                arduino.write(bytes(request.form["cmd"], 'ASCII'))
                arduino.flush()
            except serial.SerialException:
                global connected
                connected = False
            print("sent")
    return render_template('index.html')


def main():
    auto_reconnect()
    app.run(host='0.0.0.0', port='80')

if __name__ == "__main__":
    main()
