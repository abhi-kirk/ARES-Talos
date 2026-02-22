# Sabertooth 2x12 RC User's Guide (Dimension Engineering)
For use in ARES project.

## Overview
- Input voltage: 6V to 25V (2s to 6s lithium)
- Output current: Up to 12A per motor
- Peak output current: Up to 25A per motor
- Weight: 1.5oz/43 grams
- Size: 2.35" x 1.85" x 6"
- Max wire size: 16 gauge

Sabertooth 2x12 RC is designed for combot robots from 16oz to 12lb, and general-purpose robots up to 100lb. It controls two motors in mixed or independent modes. Its synchronous regenrative drive allows for high-efficiency operation and recharges your battery when you slow down or reverse. It is internally protected from damage due to overheating and over-current. 

## Installation Overview:
- **Battery connections**: The positive (usually red) wire connects to the B+ terminal and the negative (usually black) wire connects to the B- terminal. Using a polarized mating battery connector such as Deans Ultra is recommended to ensure a reliable connection. 
- **Motor connections**: Connect Motor 1 to the M1A and M1B terminals, and Motor 2 to the M2A and M2B terminals. 
- **Fwd/CH1 connector**: This connector controls the forward/backwards motion of your robot in Mixed mode, and Motor 1 in independent mode. It is usually connected to the elevator channel on an R/C reciever. 
- **Turn/CH2 connector**: This connector controls the turning motion of your robot in Mixed mode, and Motor 2 in independent mode. It is usually connected to the aileron channel on an R/C receiver. 
- **Mouting**: Sabertooth 2x12 RC mounts with four 4-40 machine screws (included) on a 1.5"x2" hole pattern. For best thermal performance it should be mounted with the bottom aluminium heat spreader connected directly to a metal chassis if possible. 
- **Indicator LEDs**: The red _Error_ LED will light to indicate overheating or current limit. The green _Status1_ LED will glow dimly when power is applied, and brightly whne a radio signal is present. The green _Status2_ LED will flash out the detected number of lithium cells when lithium mode is enabled. The green _Status2_ LED and red _Error_ LED will blink simultaneously when a low voltage condition has been detected. 

## Switch Configurations
- **Mixing Mode**: When switch 1 in the UP position, the controller is in mixed mode. In this mode the signal sent to the FWD channel controls the forward/back motion of both motors, while the Turn channel controls the difference in speed between the two motors. Switch 1 in the DOWN position allows control of the motors independently. 
- **Exponential**: If switch 2 is in the UP position, the controller will be in exponential mode. This makes the response less sensitive in the center. This is useful to bring very useful robots under control. If switch 2 is in the DOWN position, exponential is disabled and the response is linear. 
- **Lithium Mode**: Switch 3 in the DOWN position enables lithium mode, for use with lithium batteries. This will shut the controller down at 3.0 volts per cell, preventing damage to a lithium battery pack. The detected cell count is flashed on the Status 2 LED. Switch 3 should be in the UP position when using NiCd, NiMH or lead-acid batteries. 
- **Acceleration Ramping**: Switch 4 in the DOWN position turns on acceleration ramping. This will cause the Sabertooth to smoothly change speed over an approximately quarter second. Switch 4 in the UP position means no acceleration ramp is active. Ramping enables smoother control and reduces peak current draw in heavy robots. 
- **Auto-Calibrate**: Switch 5 in the UP position sets auto-calibrate mode. In auto-calibrate mode the neutral position is read at power-up and the end points are automatically detected. Switch 5 in the DOWN position uses the saved calibration settings. The default saved settings are 1500us center, 1000us min and 2000us max. The saved settings can be changed to accomodate different radios. 
- **Calibrate and Store**: Switch 6 DOWN and switch 5 UP puts the controller in Calibrate and Store mode. In this mode, the first signals sent to the controller sets the stored center point when using saved calibration settings. The longest and shortest signals sent to the controller on each channel set the endpoints. Turn switch 6 UP before powering down to save the settings. 
- **Microcontroller Mode**: Switches 5 and 6 both DOWN enables microcontroller mode. In this mode, timeout is disabled and the controller will run at the last commanded speed until a new command is given, using the saved calibration settings. This is useful when running from a micrcontroller like a Basic Stamp. 

## Steps to Restore Calibration
- Switch 5 -> UP, 6 -> DOWN. All other DOWN.
- Send 1500 $\mu$s, hold 5-10 secs.
  - Motor is stopped.
- Transition to 1000 $\mu$s, hold 5-10 secs.
  - Motor spins in positive direction.
- Transition to 2000 $\mu$s, hold 5-10 secs.
  - Motor spins in negative direction.
- While still powered, switch 6 -> UP. 
- Power down to commit settings.
- Normal operation: switch 5 -> DOWN, 6 -> DOWN.
- Verify calibration with pulses: 1000, 1400, 1500, 1600, 2000.
