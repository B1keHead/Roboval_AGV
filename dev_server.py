import queue

from flask import Flask, render_template, request

app = Flask(__name__)


@app.route('/', methods=['POST', 'GET'])
def handle_data():
    if request.method == 'POST':
        print("RECEIVED FROM WEBPAGE:", request.form['cmd'])
    return render_template('index.html')


def main():
    app.run(host='127.0.0.1', port='80')

if __name__ == "__main__":
    main()
