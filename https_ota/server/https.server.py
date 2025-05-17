from http.server import HTTPServer, SimpleHTTPRequestHandler
import ssl
import os

# Set working directory to this file's directory (so it serves files from server/)
os.chdir(os.path.dirname(os.path.abspath(__file__)))

# Server IP and port
HOST = '0.0.0.0'
PORT = 8081

# Certificate and key paths (adjust if yours are elsewhere)
CERT_FILE = '../cert/cert.pem'
KEY_FILE = '../cert/key.pem'

httpd = HTTPServer((HOST, PORT), SimpleHTTPRequestHandler)

# Use modern SSLContext instead of deprecated wrap_socket
context = ssl.SSLContext(ssl.PROTOCOL_TLS_SERVER)
context.load_cert_chain(certfile=CERT_FILE, keyfile=KEY_FILE)
httpd.socket = context.wrap_socket(httpd.socket, server_side=True)

print(f"HTTPS server running at https://{HOST}:{PORT}")
print("Serving OTA file: advanced_https_ota.bin")

try:
    httpd.serve_forever()
except KeyboardInterrupt:
    print("\nShutting down server...")
    httpd.server_close()
