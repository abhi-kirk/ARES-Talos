import re
import time
import socket
import orjson
import serial

# Example line:
# vref=300 vL=166.7 vR=333.3 uL=69.6 uR=60.5 pL=1569 pR=1440

LINE_RE = re.compile(
    r"vref=(?P<vref>-?\d+(\.\d+)?)\s+"
    r"vL=(?P<vL>-?\d+(\.\d+)?)\s+"
    r"vR=(?P<vR>-?\d+(\.\d+)?)\s+"
    r"uL=(?P<uL>-?\d+(\.\d+)?)\s+"
    r"uR=(?P<uR>-?\d+(\.\d+)?)\s+"
    r"pL=(?P<pL>-?\d+)\s+"
    r"pR=(?P<pR>-?\d+)"
)

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

        m = LINE_RE.search(line)
        if not m:
            continue

        d = m.groupdict()

        eL = float(d["vref"]) - float(d["vL"])
        eR = float(d["vref"]) - float(d["vR"])

        # PlotJuggler needs a timestamp key; we'll call it "ts"
        msg = {
            "ts": round(time.time(), 6),
            "vref": float(d["vref"]),
            "vL": float(d["vL"]),
            "vR": float(d["vR"]),
            "uL": float(d["uL"]),
            "uR": float(d["uR"]),
            "pL": int(d["pL"]),
            "pR": int(d["pR"]),
            "eL": eL,
            "eR": eR,
        }

        sock.sendto(orjson.dumps(msg), (PJ_HOST, PJ_PORT))

if __name__ == "__main__":
    main()