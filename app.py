from flask import Flask, render_template, request, jsonify,send_from_directory
from flask_cors import CORS
import socket

app = Flask(__name__)
CORS(app)  # Enable CORS

# Data received from ESP8266
esp_data = {
    "machine_status": "OFF",
    "cycle": 0,
    "cycle_time": 0,
    "qty_ms": 0
}

@app.route('/')
def index():
    return render_template('index.html', data=esp_data)



@app.route('/images/cnc.png')
def serve_image(filename):
    return send_from_directory('images', filename)

@app.route('/data', methods=['GET'])
def get_data():
    return jsonify(esp_data)

@app.route('/update-data', methods=['POST'])
def update_data():
    global esp_data
    data = request.get_json()
    if not data:
        return "No JSON received", 400

    esp_data = data
    print("Received data:", data)
    return jsonify({"message": "Data received successfully"}), 200

if __name__ == '__main__':

        # Get the local IP address
    hostname = socket.gethostname()
    local_ip = socket.gethostbyname(hostname)

    # Print the local IP and port
    print(f"Server is running on: http://{local_ip}:5000")

    # Start the Flask server
    app.run(host='0.0.0.0', port=5000)