import serial
import time
import json
import threading

TEENSY_PORT = '/dev/cu.usbmodem14101' 
BAUD_RATE = 115200

def listen_to_teensy(ser):
    """Background thread to cleanly print incoming JSON telemetry."""
    while True:
        if ser.in_waiting > 0:
            try:
                line = ser.readline().decode('utf-8').strip()
                data = json.loads(line)
                print(f"\n[TELEMETRY] Channel: {data.get('channel')} | "
                      f"V_diff: {data.get('v_diff_mV', 0):.4f} mV | "
                      f"Taps (C/M/F): {data.get('coarse_tap')}/{data.get('medium_tap')}/{data.get('fine_tap')} | "
                      f"Status: {data.get('status')}")
            except json.JSONDecodeError:
                print(f"\n[TEENSY LOG] {line}")
            except Exception as e:
                pass

def main():
    print("Connecting to AARL Capstone Prototype...")
    try:
        ser = serial.Serial(TEENSY_PORT, BAUD_RATE, timeout=1)
        time.sleep(2) # Allow Teensy to boot
    except Exception as e:
        print(f"Failed to connect to {TEENSY_PORT}. Please check your USB connection.")
        return

    # Start the listening thread
    listener = threading.Thread(target=listen_to_teensy, args=(ser,), daemon=True)
    listener.start()

    print("\n--- Capstone Hardware CLI ---")
    print("Commands:")
    print("  SET <0-47>  : Switches MUX to desired sensor channel.")
    print("  CALIBRATE   : Runs the 3-stage auto-calibration on active channel.")
    print("  READ        : Requests a single telemetry frame.")
    print("  STREAM ON   : Starts a continuous 50Hz telemetry feed.")
    print("  STREAM OFF  : Stops the telemetry feed.")
    print("-----------------------------\n")

    while True:
        try:
            user_input = input(">> ").strip().upper()
            if user_input:
                ser.write((user_input + '\n').encode('utf-8'))
                time.sleep(0.1) # Small delay to allow Teensy to process
        except KeyboardInterrupt:
            print("\nExiting CLI...")
            ser.write(b'STREAM OFF\n')
            ser.close()
            break

if __name__ == "__main__":
    main()