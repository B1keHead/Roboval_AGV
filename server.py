import threading
import queue

from flask import Flask, render_template, request

try:
    from serial import Serial, SerialException
    arduino = serial.Serial('/dev/ttyUSB0', 9600, timeout=0)
    connected_uart = True
except (SerialException, NameError) as e:
    connected_uart = False

in_ = queue.Queue()
data = []
app = Flask(__name__)


@app.route('/', methods=['POST', 'GET'])
def handle_data():
    if request.method == 'POST':
        for field in request.form:
            if field.startswith('send_') and connected_uart:
                arduino.write(bytes(field.split("send_")[1], 'ASCII'))
    while not in_.empty():
        data.insert(0, in_.get())
    return render_template('index.html', data=data)


def serial_read():
    while True:
        if arduino.inWaiting():
                in_.put(str(arduino.readline(), 'ASCII'))


def main():
    flask = threading.Thread(target=app.run,
                             kwargs={'host': '0.0.0.0', 'port':'80'})
    reader = threading.Thread(target=serial_read)
    flask.start()
    if connected_uart: reader.start()

if __name__ == "__main__":
    main()
