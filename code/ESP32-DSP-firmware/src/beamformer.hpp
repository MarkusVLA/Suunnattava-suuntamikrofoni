#include <iostream>
#include <cmath>
#include "buffer.h"
#include "playbackQueue.h"

class CircularArrayBeamformer {
private:
    int numSensors;
    double Sensorarrayradius;
    double* sensorPositions;
    playbackQueue* Playback;

public:
    CircularArrayBeamformer() : numSensors(6), Sensorarrayradius(16.0) {
        sensorPositions = new double[numSensors];
        
        // Initialize sensor positions evenly around the circle
        for (int i = 0; i < numSensors; ++i) {
            sensorPositions[i] = i * (360.0 / numSensors);  // Assuming angles are in degrees
        }
    }

    ~CircularArrayBeamformer() {
        delete[] sensorPositions;
    }

    // Calculate the steering vector for a given angle
    double* calculateSteeringVector(double angle) {
        double* steeringVector = new double[numSensors];
        for (int i = 0; i < numSensors; ++i) {
            double angleDifference = angle - sensorPositions[i];
            // Convert angle to radians
            double angleInRadians = angleDifference * PI / 180.0;
            steeringVector[i] = std::cos(angleInRadians);
        }
        return steeringVector;
    }

    // Perform delay-and-sum beamforming on a Buffer
    void beamform(Buffer& buffer, double angle) {
        int xDim = buffer.getXDim();
        int yDim = buffer.getYDim();

        double* steeringVector = calculateSteeringVector(angle);

        for (int i = 0; i < xDim; ++i) {
            double sum = 0.0;
            for (int j = 0; j < yDim; ++j) {
                sum += buffer.getValue(i, j) * steeringVector[j];
            }
            Playback ->pushBack(sum);
        }

        delete[] steeringVector;
    }
};
