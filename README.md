# hack-box

A physical and portable "hack box" game with two stages that demonstrates simple and entertaining robotic applications.

This project documentation describes a portable game with two stages and a win sequence. The game utilizes the correct user input from a keypad and the correct sequence of buttons pressed in order to progress through the stages. Once it is complete, the user's reward is a note of congratulations. It was created in order to grab attention and spark curiosity in those interested in technology.

## demo
Here is a brief visual demo of the transition between one stage and another.

![Hack Box demo](media/hackbox%20gif.gif)

## overview

- The project allows for users to interact with a personal robotics project that is fun and not absurdly complicated. By guessing the hardcoded passcode of "1892" that alludes to the year our school Winston-Salem State University was founded, and by playing a fun game of Simon Says, the game is not complicated or difficult for them to master.
- This was a 3 day project I built for the purposes of the 2026 Fall Organization Fair at WSSU, in order to promote two new technology oriented student organizations: *Women in Technology* and *CyberRamz*. This was a live demo at our booth of one of the possible projects our members could engage in and create if they were so inclined.
- This "hack box" was featured in a social media post as well as 

## how it works

- During the first stage, the user guesses a four digit code "1892" using a 4x4 keypad, and the user is told after every wrong attempt whether or not they should guess higher or lower. Upon winning, they get visual feedback and are told to progress onto the next stage.
- Afterwards, the user is directed to the second stage due to the flashing lights sequence and the indication on the LCD. They can begin to play a "Simon Says" memory game where they have to select a correct sequence of 5 randomized button presses. The pattern is both auditory (different notes on a buzzer) as well as visual (the color of the light flashing that corresponds with the button). If they get one wrong, the game starts over. Upon winning, they receive auditory and visual feedback that they have succeeded.
- Once the user has finished both stages, a mechanical swinging door shoves out a piece of paper with a customized congratulating note on it. Once the note has been pushed out, the door retracts back into the enclosure.

## hardware

Here is a list of all of the things I've used in this project.

| Quantity | Component |
|---------|----------|
|1| Arduino R3 |
|1| Breadboard |
|1| Positional Micro Servo |
|1| Keypad 4x4 |
|1| LCDisplay I2C PCF 8574, (0x27)|
|1| Green LED |
|1| Yellow LED |
|1| Red LED |
|26| M-M Jumper Wires |
|10| F-M Jumper Wires |
|3| Push Button R13-507-5C-X |
|3| 220 ohm Resistor |
|1| Piezoelectric Buzzer |
|1| Hard Plastic Case |

### wiring

(insert wiring diagram)

### pin mapping
| Keypad | LCD Display |Push Buttons| LEDs| Feedback Devices | Shared Rails |
|---|---|--|--|--|--|
| Row 1 > 2| SDA > A4| Red > 12| Red > A0| Buzzer > A3| Power > 5V|
| Row 2 > 3| SCL > A5| Yellow > 11| Yellow > A1| Servo > 13|
| Row 3 > 4| VCC > 5V| Green > 10| Green > A2| Ground > GND2|
| Row 4 > 5| GND > GND1|
| Col 1 > 6|
| Col 2 > 7|
| Col 3 > 8|
| Col 4 > 9|



## debugging and notes

( keypad mapping, keypad button fix, memory fix, servo fix )

## credits

## license
