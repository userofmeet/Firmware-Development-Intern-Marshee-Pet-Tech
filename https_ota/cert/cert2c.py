# cert_to_c_array.py
with open("cert.pem", "rb") as f:
    cert = f.read()

with open("cert.c", "w") as f:
    f.write("const unsigned char cert_pem[] = {\n")
    for i in range(0, len(cert), 12):
        chunk = cert[i:i+12]
        line = ", ".join(f"0x{b:02x}" for b in chunk)
        f.write("  " + line + ",\n")
    f.write("};\n")
    f.write(f"const unsigned int cert_pem_len = {len(cert)};\n")
