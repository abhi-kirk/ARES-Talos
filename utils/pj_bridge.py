import time
import socket
import orjson
import serial

# Example line:
# 1727497,500.00,0.00,-0.00,200.00,200.00,1700,1300
# columns: mcu_ts,vref,vL,vR,uL,uR,pL,pR

def main():
    SERIAL_PORT = "/dev/tty.usbmodem11103"
    BAUD = 115200

    PJ_HOST = "127.0.0.1"
    PJ_PORT = 5005                          # must match PlotJuggler UDP Server port

    sock = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
    ser = serial.Serial(SERIAL_PORT, BAUD, timeout=1)

    while True:
        raw = ser.readline()
        if not raw:
            continue

        try:
            line = raw.decode("utf-8", errors="ignore").strip()
        except Exception:
            continue

        parts = line.split(",")
        if len(parts) != 8:
            print(f"[no match] {line}")
            continue

        try:
            _, vref, vL, vR, uL, uR, pL, pR = parts
            vref, vL, vR, uL, uR = float(vref), float(vL), float(vR), float(uL), float(uR)
            pL, pR = int(pL), int(pR)
        except ValueError:
            print(f"[parse error] {line}")
            continue

        msg = {
            "timestamp": round(time.time(), 6),
            "vref": vref,
            "vL": vL,
            "vR": vR,
            "uL": uL,
            "uR": uR,
            "pL": pL,
            "pR": pR,
            "eL": vref - vL,
            "eR": vref - vR,
        }

        payload = orjson.dumps(msg)
        sock.sendto(payload, (PJ_HOST, PJ_PORT))
        print(f"[sent] {payload}")

if __name__ == "__main__":
    main()