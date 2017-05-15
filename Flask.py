import threading
import queue

from flask import Flask, render_template, request
import serial

in_ = queue.Queue()
out = queue.Queue()
#arduino = serial.Serial('/dev/ttyUSB0', 9600, timeout=0)
data = []
app = Flask(__name__)


@app.route('/', methods=['POST', 'GET'])
def handle_data():
    if request.method == 'POST':
        for field in request.form:
            if field.startswith('send_'):
                out.put(field.split("send_")[1])
    while not in_.empty():
        data.insert(0, in_.get())
    return render_template('index.html', data=data)


def serial_send():
    while True:
        if not out.empty():
            arduino.write(bytes(out.get(), 'ASCII'))
            out.task_done()


def serial_read():
    while True:
        if arduino.inWaiting():
                in_.put(str(arduino.readline(), 'ASCII'))


def main():
    flask = threading.Thread(target=app.run, kwargs={'host': '0.0.0.0'})
    sender = threading.Thread(target=serial_send)
    reader = threading.Thread(target=serial_read)
    flask.start()
    sender.start()
    reader.start()

if __name__ == "__main__":
    main()
