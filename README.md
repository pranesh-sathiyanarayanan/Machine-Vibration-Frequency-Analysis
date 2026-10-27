# Machine Vibration Frequency Analysis

## Project Overview
Machine Vibration Frequency Analysis is an IoT-based condition monitoring project designed to detect abnormal operating patterns in industrial machines, particularly bearings and gears. The system uses a 40 kHz ultrasonic receiver and a piezoelectric Acoustic Emission (AE) sensor to collect machine-related signals for analysis.

## Technical Approach
- **Signal Collection:** Collect airborne ultrasonic signals and structure-borne acoustic emissions using two sensors.
- **Data Acquisition:** Use an ESP32 and suitable signal-conditioning circuits to acquire and preprocess sensor signals.
- **Frequency Analysis:** Apply Fast Fourier Transform (FFT) to analyze frequency components and identify abnormal signal patterns.
- **AI/ML Analysis:** Use trained machine-learning models to detect abnormal conditions and classify possible faults.
- **IoT Connectivity:** Transmit machine data through Wi-Fi to a central server for monitoring.
- **Dashboard and Alerts:** Display machine health status, analysis results, and warnings through a dashboard and LCD.

## Key Features
- Dual-sensor acoustic monitoring
- Frequency-domain signal analysis using FFT
- AI/ML-based abnormality detection
- Machine-specific monitoring using unique Machine IDs
- Wi-Fi-enabled data transmission
- Centralized monitoring and warning system

## Applications
- Industrial bearing and gear condition monitoring
- Early detection of abnormal machine operation
- Predictive maintenance research and prototyping
- Industrial IoT monitoring systems

## Project Goal
To develop a smart machine-monitoring prototype that supports early abnormal-condition detection, improves maintenance decisions, and helps reduce unexpected industrial downtime.

## Development Status
Prototype under development. Detection performance and fault-classification capabilities require testing and validation with suitable sensor data.

## Technologies
ESP32 | IoT | FFT | Signal Processing | AI/ML | Wi-Fi | Machine Condition Monitoring
