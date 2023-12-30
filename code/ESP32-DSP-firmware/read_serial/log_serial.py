import serial as srl
import sys

class SerialLogger:

    def __init__(self, PORT, BAUD_RATE, output_file) -> None:
        self.port = PORT
        self.baud_rate = BAUD_RATE
        self.out_file = output_file

    def read_and_save(self):
        with srl.Serial(self.port, self.baud_rate, timeout=1) as ser:
            with open(self.out_file, "w") as out_f:

                # Wait for the line to contain "START"
                while True:
                    try:
                        line = ser.readline().decode('utf-8').strip()
                        if line == "BEGIN":
                            out_f.write(f"{line}\n")
                            break

                    except Exception as e:
                        print(f"Error: {e}")
                        break


                while True:
                    try:
                        line = ser.readline().decode('utf-8').strip()
                        out_f.write(f"{line}\n")
                        if line == "END":
                            break

                    except Exception as e:
                        print(f"Error: {e}")
                        break

        print(f"Data saved to {self.out_file}")
        return



def usage():
    print(
        f"Usage:"
        f"python3 log_serial.py <port> <baud rate>"
        f"\n----------------------------------------------\n"
    )
    return


def main(args):

    if ('-h' in args) or len(args) <= 2:
        usage()
        return
    
    port = sys.argv[1]
    baud = int(sys.argv[2])

    logger = SerialLogger(PORT=port, BAUD_RATE=baud, output_file="serial_out.txt")

    print(f"Reading from serial port {port} at baud {baud}")
    logger.read_and_save()

    return

if __name__ == "__main__":
    main(sys.argv)