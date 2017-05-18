import queue

from flask import Flask, render_template, request

app = Flask(__name__)


@app.route('/', methods=['POST', 'GET'])
def handle_data():
    if request.method == 'POST':
        for field in request.form:
            if field.startswith('send_'):
                print("RECEIVED FROM WEBPAGE:", field.split("send_")[1])
    return render_template('index.html')


def main():
    app.run(host='127.0.0.1', port='80')

if __name__ == "__main__":
    main()
