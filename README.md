# Speedometer
Simple speedometer sketch
Uses Arduino Nano in Texas Northern install. We have three speedometers, one for each main line, Red, Green and Blue.
Also uses NCE BD20 (or close) to detect occupance on two track segments
Results are shown with Adafruit 7-segamnet display: Adafruit 0.56" 4-Digit 7-Segment Display w/I2C Backpack - Blue PRODUCT ID: 881 (Several colors available)
We prefer the Nano breakout board instead of direct soldering to Nano, easuer to swap out bad parts as needed. Ebay seatch key: Arduino Nano V3.0 I/O Expansion Breakout Board For NANO I/O Shield
So complete BOM:
Arduino Nano
Arduino Breakout Board
Adafruit display
Two NCE BD20
(Plus wires, crimp connections, etc)

The sketch logic:
On startup. calibrate BD20 resting value. (In testing, no two sensors give same value, and values fluxuate for each sensor.)
Calculate trigger values inad initialize display
In loop():
Read current sensor values
Based on state, determine state change if sensor trigger conditions are met
States: 
idle: Looking for trigger conditions of first sensor. If triggered, start timer and change state to Start or Start_reversed depending on which sensor triggered first..
Start: Look for trigger condition on second sensor. When triggered, capture second timer, send both timers and track segment length to speed calc function. Display speed and change to End state.
Start_reversed: same as Start, but looking for other sensor trigger.
End: Look for all trigger conditions to clear. When clear, set reset timer and change to reset state.
Reset: When timer expires, clear display, changte to Idle state.

Several visual cues used to denote states to observer: 
Idle: Colon shows steady
Start: flashing colon
End: Speed value shown
Reset: Speed value flashes

Customizing for your installation:
float TrapLength is length of track monitores by each sensor. 
  * This is in inches, for use in calc function. 
  * Must be touching track segments. 
  * Must have insulating track joiners on each end of measured rail.

Several debug Serial.print lines are commented. May help debug installation. 
fudge is unified value for miles to inches, hours to miliseconds, and scale (HO, Value for N scale is provided in comment)

Note that using BD20 or clone, no trackwork needs to change, and nothing is visible on top of layout, except for speed display location. Only wiring change is to route track feed into BD20 'tombstone". Other sensor options are possible, just adjust the trigger code.

For more information, check out texasnorthern.org
