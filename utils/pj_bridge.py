import time
import socket
import orjson
import serial

# CSV format (Phase 2):
# mcu_ts,loop_dt,pitch,gyro_x,vref,vL,vR,uL,uR,pL,pR  (11 fields)
# gyro_x = pitch rate axis (validated Phase 2 bench test; gyro_y is orthogonal)

def main():
    SERIAL_PORT = "/dev/tty.usbmodem11103"
    BAUD = 115200

    PJ_HOST = "127.0.0.1"
    PJ_PORT = 5005          # must match PlotJuggler UDP Server port

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

        if line.startswith("#"):
            print(f"[info] {line}")
            continue

        parts = line.split(",")
        if len(parts) != 11:
            print(f"[no match] {line}")
            continue

        try:
            mcu_ts, loop_dt, pitch, gyro_x, vref, vL, vR, uL, uR, pL, pR = parts
            loop_dt = int(loop_dt)
            pitch, gyro_x  = float(pitch),  float(gyro_x)
            vref, vL, vR   = float(vref),   float(vL),   float(vR)
            uL, uR         = float(uL),     float(uR)
            pL, pR         = int(pL),       int(pR)
        except ValueError:
            print(f"[parse error] {line}")
            continue

        msg = {
            "timestamp": round(time.time(), 6),
            # IMU state
            "pitch":    pitch,
            "gyro_x":   gyro_x,
            # Wheel velocity
            "vref":     vref,
            "vL":       vL,
            "vR":       vR,
            "vel_avg":  (vL + vR) / 2.0,
            # Controller outputs
            "uL":       uL,
            "uR":       uR,
            "pL":       pL,
            "pR":       pR,
            # Derived errors
            "eL":       vref - vL,
            "eR":       vref - vR,
            # Diagnostics
            "loop_dt":  loop_dt,
        }

        payload = orjson.dumps(msg)
        sock.sendto(payload, (PJ_HOST, PJ_PORT))
        print(f"[sent] {payload}")

if __name__ == "__main__":
    main()