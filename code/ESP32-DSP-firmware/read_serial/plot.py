import matplotlib.pyplot as plt
import numpy as np
from scipy.fft import fft

# Constants
reference_voltage = 1.25 * 3.3  # ADC reference voltage in volts
bit_resolution = 16  # ADC bit resolution (adjust this according to your ADC)
adc_max_value = 2**bit_resolution - 1  # Maximum ADC value

# Reading the data
with open("serial_out2.txt", 'r') as file:
    content = file.readlines()

# Extracting the sample rate
sample_rate_line = content[1]
sample_rate_hz = float(sample_rate_line.split()[2]) * 1000 
data = [float(line.strip().rstrip(',')) for line in content[3:-1]]  #remove BEGIN and END
time_interval = 1 / sample_rate_hz  # Time interval between samples in seconds
time_points = np.array([i * time_interval for i in range(len(data))])

# Convert ADC values to voltage
voltage_data = [adc_value * (reference_voltage / adc_max_value) for adc_value in data]

# Window the signal from 0.2 to 0.3 seconds
start_index = np.where(time_points >= 0.2)[0][0]
end_index = np.where(time_points <= 0.3)[0][-1]
windowed_time_points = time_points[start_index:end_index+1]
windowed_voltage_data = voltage_data[start_index:end_index+1]

# Performing FFT on the windowed voltage data
fft_data = fft(windowed_voltage_data)
freq = np.fft.fftfreq(len(fft_data), time_interval)
magnitude = np.abs(fft_data)

# Creating subplots
fig, axs = plt.subplots(2, 1, figsize=(12, 12))

# Plotting the windowed time-domain voltage data
axs[0].plot(windowed_time_points, windowed_voltage_data)
axs[0].set_xlabel("Time (s)")
axs[0].set_ylabel("Voltage (V)")
axs[0].set_title("Voltage vs Time (Windowed)")
axs[0].grid(True)

axs[1].plot(freq, 10 * np.log10(magnitude), color='red')
axs[1].set_xlabel("Frequency (Hz)")
axs[1].set_ylabel("Magnitude (dB)")
axs[1].set_title("FFT of Voltage")
axs[1].grid(True)
axs[1].set_xlim([0, max(freq)/2])

# Display the plots
plt.tight_layout()
plt.subplots_adjust(hspace=0.5) 
plt.show()
