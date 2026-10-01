// Serial Interfaces Lab
/ Practice 3 – I²C/SPI Monitoring Station
Objective
The objective of this practice is to integrate the I²C and SPI communication interfaces into a
complete monitoring station. The system will combine an RTC, a temperature sensor, a
keypad, an LCD, a MAX7219 display, and an alarm system.
The practice will be developed progressively. Each section introduces a new component or
functionality that will later become part of the complete monitoring station.
Required Hardware
The following components are required:
● KL25Z
● DS3231 RTC module
● I²C temperature sensor
● MAX7219 module
● Four 7-segment display(s)
● 4×4 matrix keypad
● LCD display
● Buzzer
● LED
● Resistors and other components required for the connections
● Breadboard and jumper wires
Note: The temperature sensor must communicate through I²C.
It is suggested to use the BME280. However, it is not required for the basic practice. It may
be used as an additional sensor as part of the Extra Challenges.
Step 1 — RTC: Date and Time
Begin the practice by implementing communication with the DS3231 RTC through I²C.
The system must:
1. Connect the DS3231 to the microcontroller using I²C.
2. Identify the I²C address of the DS3231.
3. Initialize the I²C peripheral.
4. Configure the RTC with a valid date and time.
5. Read the current date and time from the RTC.
6. Display the current date and time on the LCD.
7. Verify that the time continues to advance correctly.
The LCD should provide enough information to verify that the RTC is working correctly.
For example:
Date: 09/09/26
Time: 15:30:25
The exact LCD format may be modified according to the display being used.
Step 2 — SPI Display with MAX7219
In this section, integrate the MAX7219 using SPI.
The system must:
1. Connect the MAX7219 to the microcontroller using SPI.
2. Initialize the MAX7219.
3. Configure the required registers.
4. Display the current time obtained from the DS3231.
5. Use the 7-segment display to show the time in HH:MM format.
6. Keep the LCD displaying additional information such as the date.
The time displayed on the MAX7219 must come directly from the DS3231.
Step 3 — Keypad Configuration
The keypad will now be added as the main user input device.
Create a Configuration Mode that allows the user to configure the RTC using the keypad.
The system must allow the user to:
● Set the current hours and minutes.
● Set the current date.
● Confirm the configuration.
● Cancel the configuration and return to the previous state.
The LCD should provide instructions to the user during the configuration process.
For example:
SET TIME
HH:MM
12:35
and:
SET DATE
DD/MM/YY
09/09/26
The keypad mapping may be selected by the team, but it must be clearly documented and
consistently used throughout the application.
The system should distinguish between at least:
● Normal Mode
● Configuration Mode
Step 4 — RTC Alarm and Interrupt
Use the alarm functionality of the DS3231 to create an alarm system.
The system must:
1. Configure one of the DS3231 alarms.
2. Allow the user to configure the alarm time using the keypad.
3. Use the INT/SQW pin of the DS3231.
4. Connect the alarm output to a GPIO capable of generating an interrupt.
5. Detect the alarm event using an interrupt.
6. Activate the buzzer when the alarm occurs.
7. Activate an LED as a visual alarm indicator.
8. Display an appropriate alarm message on the LCD.
9. Allow the user to acknowledge or deactivate the alarm using the keypad.
For example:
ALARM SET
07:30
When the alarm is triggered:
*** ALARM ***
Press # to stop
The alarm must be triggered by the RTC rather than by continuously comparing the current
time in software.
Step 5 — I²C Temperature Monitoring
Add an I²C temperature sensor to the system.
The sensor must share the same I²C bus with the DS3231.
The system must:
1. Connect the temperature sensor to the existing I²C bus.
2. Identify its I²C address.
3. Initialize and configure the sensor.
4. Read the temperature measurement.
5. Process the sensor data according to its datasheet.
6. Convert the raw data into the appropriate temperature units.
7. Display the temperature on the LCD.
For example:
Time: 15:30
Temp: 24.6 C
The temperature sensor must be a separate I²C device from the DS3231.
The implementation should demonstrate that both devices can operate on the same I²C bus.
Step 6 — Complete Monitoring Station
Finally, integrate all the components into a single application.
The completed system must operate as a Monitoring Station with at least the following
functionality.
Normal Mode
The system should continuously display:
● Current time
● Current date
● Current temperature
● Alarm status
The 7-segment display must show the current time in:
HH:MM
The LCD should display additional monitoring information.
For example:
09/09/26
Temp: 24.6 C
Alarm: ON
The exact screen layout may be designed by the team.
Configuration Mode
Using the keypad, the user must be able to:
● Configure the RTC date.
● Configure the RTC time.
● Configure the alarm time.
● Enable or disable the alarm.
● Return to Normal Mode.
The LCD must provide enough information for the user to understand the current
configuration option.
Alarm Mode
When the configured alarm time is reached:
1. The DS3231 generates the alarm signal.
2. The microcontroller detects the alarm through an interrupt.
3. The buzzer is activated.
4. The LED is activated.
5. The LCD indicates that the alarm is active.
6. The user can acknowledge the alarm using the keypad.
After the alarm is acknowledged, the system must return to its normal monitoring operation.
Extra Challenges — Up to 30 Points
The following challenges are optional. They are not required to complete the basic
practice.
Each challenge must be fully implemented and demonstrated to receive its corresponding
points.
Extra Challenge points are awarded in addition to the 100-point base grade and may be
used toward the I²C Device Research activity.
Extra Challenge 1 — I²C LCD Interface (+10)
Replace the standard parallel LCD connection with an I²C LCD module.
The team must:
● Connect the LCD to the I²C bus.
● Identify the LCD's I²C address.
● Initialize and control the LCD through I²C.
● Maintain the monitoring station's LCD functionality.
● Demonstrate that the LCD operates together with the DS3231 and temperature
sensor on the same I²C bus.
Points: +10
Extra Challenge 2 — Additional Environmental
Measurements (+5)
Extend the temperature monitoring functionality by obtaining additional measurements from
the sensor.
For a BME280, this may include:
● Humidity
● Atmospheric pressure
The team must:
● Configure the additional measurements.
● Read the corresponding data.
● Process the sensor output correctly.
● Display the measurements on the LCD.
Points: +5
The BME280 may be used for this challenge, but it is not required for the
basic practice.
Extra Challenge 3 — Temperature Threshold Alarm (+5)
Add an independent temperature threshold to the monitoring station.
The system must:
1. Allow the user to configure a temperature limit using the keypad.
2. Continuously compare the measured temperature with the configured limit.
3. Activate the buzzer and LED when the limit is exceeded.
4. Display a warning message on the LCD.
For example:
Temp: 31.2 C
WARNING!
Points: +5
Extra Challenge 4 — Additional I²C Sensor (+5)
Add a second sensor that communicates through I²C.
The sensor must be different from the required temperature sensor.
The team must:
● Connect the sensor to the existing I²C bus.
● Identify its I²C address.
● Initialize and configure the sensor.
● Read at least one measurement.
● Process the measurement correctly.
● Display the measurement on the LCD.
● Demonstrate that the new sensor operates together with the existing I²C devices.
Points: +5
Extra Challenge 5 — Additional Sensor Using Another
Interface (+5)
Add a sensor that uses a different communication interface, such as SPI, analog input, or
another interface previously covered in the course.
The team must:
● Connect and configure the sensor.
● Read at least one measurement.
● Process the measurement correctly.
● Integrate the sensor into the monitoring station.
● Display its measurement on the LCD or 7-segment display, as appropriate.
Points: +5
Extra Challenge Summary
Extra Challenge Points
I²C LCD Interface +10
Additional Environmental Measurements +5
Temperature Threshold Alarm +5
Additional I²C Sensor +5
Additional Sensor Using Another Interface +5
Maximum Extra Points +30
Teams may complete any combination of the extra challenges. For example:
● I²C LCD + Temperature Threshold = +15
● BME280 additional measurements + Additional I²C Sensor = +10
● All five challenges = +30
Deliverables
Each team must submit:
1. Source Code
The complete source code used for the practice.
The code should be organized into appropriate functions or modules for:
● I²C communication
● SPI communication
● DS3231
● Temperature sensor
● MAX7219
● LCD
● Keypad
● Alarm/interrupt handling
● Main application
2. Demonstration
The complete system must be demonstrated to the instructor.
The demonstration must show:
1. RTC configuration.
2. Correct date/time display.
3. MAX7219 displaying the time.
4. Keypad configuration.
5. Alarm configuration.
6. RTC alarm interrupt.
7. Buzzer and LED activation.
8. Temperature measurement.
9. Complete monitoring station operation.
Any completed Extra Challenges must also be demonstrated.
