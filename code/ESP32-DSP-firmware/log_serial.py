import serial as srl
import matplotlib.pyplot as plt
import sys
import wave


class SerialLogger:

    def __init__(self, PORT, BAUD_RATE, serial_read_buffer_size, output_file) -> None:
        self.port = PORT
        self.baud_rate = BAUD_RATE
        self.out_file = output_file
        self.buffer_size = serial_read_buffer_size
        self.serial_read_buffer = []

    def fill_read_buffer(self):
        self.serial_read_buffer = []
        with srl.Serial(self.port, self.baud_rate, timeout=1) as ser:
            for _ in range(self.buffer_size):
                try:
                    line = float(ser.readline().decode('utf-8').strip())
                    if line > 1:
                        line = 0.4

                    self.serial_read_buffer.append(line)
                except Exception as e:
                    #print(e)
                    pass

    def save_as_wav(self, filename="output.wav"):
        # Convert the serial read buffer to bytes
        buffer_bytes = bytearray()
        for value in self.serial_read_buffer:
            scaled_value = int(value * 32767)  # Scale float to 16-bit integer range
            buffer_bytes.extend(scaled_value.to_bytes(2, 'little', signed=True))

        # Save as WAV
        with wave.open(filename, 'wb') as wf:
            # Set parameters: (nchannels, sampwidth, framerate, nframes, comptype, compname)
            wf.setparams((1, 2, 45000, 0, 'NONE', 'NONE'))  # Assuming 45kHz sample rate
            wf.writeframes(buffer_bytes)

        print(f"Data saved to {filename}")





    def save_read_buffer(self):
        with open(self.out_file, "w") as out_f:
            for value in self.serial_read_buffer:
                out_f.write(f"{value}\n")

        print(f"Data saved to {self.out_file}")

    def plot_and_save(self, filename="plot.png"):
        
        print("Creating plot")
        # Plot the data
        plt.figure(figsize=(10, 6))
        plt.plot(self.serial_read_buffer)
        plt.title("Serial Data Plot")
        plt.xlabel("Sample Number")
        plt.ylabel("Value")
        plt.grid(True)
        plt.tight_layout()

        # Save the plot as a PNG
        plt.savefig(filename)
        plt.close()

    def print_buffer_sample(self):
        print(self.serial_read_buffer[:16])


def usage():
    print(

        f"Usage:"
        f"python3 log_serial.py <port> <baud rate>"
        f"\n----------------------------------------------\n"
        f"Flags:\n"
        f"-G    -->     Graph data\n"
        f"-W    -->     Save WAV\n" 
        f"\n----------------------------------------------\n"

    )
    return


def main(args):

    if ('-h' in args):
        usage()
        return
    
    if len(args) <= 2: # Arguments must contain port and baud rate
        usage()
        return
    
    port = sys.argv[1]
    baud = int(sys.argv[2])

    logger = SerialLogger(PORT=port, BAUD_RATE=baud, serial_read_buffer_size=45000, output_file="serial_out.txt")

    print(f"Reading from serial port {port} at baud {baud}")
    logger.fill_read_buffer()
    logger.save_read_buffer()

    if ("-G") in args:
        logger.plot_and_save()

    if ("-W") in args:
        logger.save_as_wav()

    return

if __name__ == "__main__":
    main(sys.argv)
