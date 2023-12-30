import numpy as np
import matplotlib.pyplot as plt

def circular_array_elements(N, radius):
    #positions of N microphones in a circular array with radius.
    angles = np.linspace(0, 2 * np.pi, N, endpoint=False)
    x = radius * np.cos(angles)
    y = radius * np.sin(angles)
    return np.stack((x, y), axis=-1)

def beam_pattern_with_steering(array, freq, c, theta, steering_angle, N):
    wavelength = c / freq
    k = 2 * np.pi / wavelength
    steering_vector = np.exp(-1j * k * (array[:, 0]*np.cos(steering_angle) + array[:, 1]*np.sin(steering_angle)))
    response = np.exp(1j * k * (array[:, 0]*np.cos(theta) + array[:, 1]*np.sin(theta)))
    pattern = np.abs(np.sum(steering_vector * response)) / N
    return pattern


N = 6 # Number of microphones
radius = 0.08 # Radius of the circular array in meters (8 cm)
freq = 2000 # Frequency of the incoming signal in Hz
c = 343 # Speed of sound in air 
steering_angle = 0 # Steering angle 

# Generate circular array
array = circular_array_elements(N, radius)

# Calculate beam pattern for different angles
angles = np.linspace(0, 2*np.pi, 360)
beam_patterns_steered_1kHz = np.array([beam_pattern_with_steering(array, freq, c, theta, steering_angle, N) for theta in angles])

# Plotting the beam pattern for 1 kHz signal
plt.polar(angles, beam_patterns_steered_1kHz)
plt.title("Beam Pattern 4 kHz")
plt.show()
