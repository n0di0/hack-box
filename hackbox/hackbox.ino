#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <Servo.h>

// ---------- LCD ----------
LiquidCrystal_I2C lcd(0x27, 16, 2);

// ---------- Keypad ----------
const byte ROWS = 4;
const byte COLS = 4;

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

byte rowPins[ROWS] = {13, 3, 4, 5};
byte colPins[COLS] = {6, 7, 8, 9};

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

char wholeKey[5]; // 4 digits + null terminator, fixed-size, no dynamic memory
const int SECRET_CODE = 1892;

// ---------- Simon Says ----------
#define CHOICE_OFF    0
#define CHOICE_NONE   0
#define CHOICE_RED    (1 << 0)
#define CHOICE_YELLOW (1 << 1)
#define CHOICE_GREEN  (1 << 2)

#define LED_RED     A0
#define LED_YELLOW  A1
#define LED_GREEN   A2

#define BUTTON_RED     12
#define BUTTON_YELLOW  11
#define BUTTON_GREEN   10

#define BUZZER  A3

#define ROUNDS_TO_WIN     5
#define ENTRY_TIME_LIMIT  3000

byte gameBoard[32];
byte gameRound = 0;

// ---------- Servo ----------
Servo doorServo;
#define SERVO_PIN 2

// ---------- Idle blink (passcode phase) ----------
unsigned long lastBlinkTime = 0;
byte blinkIndex = 0;
const unsigned long BLINK_INTERVAL = 700;

void setup(){
  lcd.init();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Code:");

  pinMode(BUTTON_RED, INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);
  pinMode(BUTTON_GREEN, INPUT_PULLUP);

  pinMode(LED_RED, OUTPUT);
  pinMode(LED_YELLOW, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);

  pinMode(BUZZER, OUTPUT);

  doorServo.attach(SERVO_PIN);
  doorServo.write(70); // start closed/home
}

void loop(){
  runPasscodeStage(); // blinks LEDs one at a time, returns once correct code entered
  runSimonStage();     // twinkles + plays, returns once Simon is won
  play_winner();
  openDoor();
  // loop repeats -> back to passcode stage for the next person
}

//===================== PASSCODE STAGE =====================

void runPasscodeStage(void)
{
  bool solved = false;

  while (!solved)
  {
    int digitsEntered = 0;
    wholeKey[0] = '\0'; // reset to empty string

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Code:");

    while (digitsEntered < 4)
    {
      idleBlink(); // keep cycling LEDs while waiting for keys, non-blocking

      char singleKey = keypad.getKey(); // non-blocking check

      if (singleKey && singleKey >= '0' && singleKey <= '9')
      {
        wholeKey[digitsEntered] = singleKey;
        digitsEntered++;
        wholeKey[digitsEntered] = '\0'; // keep string properly terminated

        lcd.setCursor(0, 0);
        lcd.print("Code: ");
        lcd.print(wholeKey);
      }
    }

    int guess = atoi(wholeKey); // convert char array to int

    if (guess == SECRET_CODE)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Correct!");
      lcd.setCursor(0, 1);
      lcd.print("Level 1 Solved");
      delay(3000);
      solved = true;
    }
    else if (guess < SECRET_CODE)
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Too low!");
      lcd.setCursor(0, 1);
      lcd.print("Try higher");
      delay(2000);
    }
    else
    {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Too high!");
      lcd.setCursor(0, 1);
      lcd.print("Try lower");
      delay(2000);
    }
  }

  setLEDs(CHOICE_OFF);
}

void idleBlink(void)
{
  unsigned long currentTime = millis();

  if (currentTime - lastBlinkTime >= BLINK_INTERVAL)
  {
    blinkIndex = (blinkIndex + 1) % 3; // cycle through 0, 1, 2

    if (blinkIndex == 0) setLEDs(CHOICE_RED);
    else if (blinkIndex == 1) setLEDs(CHOICE_YELLOW);
    else setLEDs(CHOICE_GREEN);

    lastBlinkTime = currentTime;
  }
}

//===================== SIMON SAYS STAGE =====================

void runSimonStage(void)
{
  bool won = false;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Level 2:");
  lcd.setCursor(0, 1);
  lcd.print("Simon Says!");
  delay(1500);

  while (!won)
  {
    attractMode(); // twinkle red -> yellow -> green until a button is pressed

    setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_YELLOW);
    delay(1000);
    setLEDs(CHOICE_OFF);
    delay(250);

    if (play_memory() == true)
      won = true;
    else
      play_loser(); // let them retry Simon without redoing the passcode
  }
}

boolean play_memory(void)
{
  randomSeed(millis());
  gameRound = 0;

  while (gameRound < ROUNDS_TO_WIN)
  {
    add_to_moves();
    playMoves();

    for (byte currentMove = 0; currentMove < gameRound; currentMove++)
    {
      byte choice = wait_for_button();
      if (choice == 0) return false;
      if (choice != gameBoard[currentMove]) return false;
    }

    delay(1000);
  }

  return true;
}

void playMoves(void)
{
  for (byte currentMove = 0; currentMove < gameRound; currentMove++)
  {
    toner(gameBoard[currentMove], 150);
    delay(150);
  }
}

void add_to_moves(void)
{
  byte newButton = random(0, 3);

  if (newButton == 0) newButton = CHOICE_RED;
  else if (newButton == 1) newButton = CHOICE_YELLOW;
  else if (newButton == 2) newButton = CHOICE_GREEN;

  gameBoard[gameRound++] = newButton;
}

void setLEDs(byte leds)
{
  digitalWrite(LED_RED,    (leds & CHOICE_RED)    ? HIGH : LOW);
  digitalWrite(LED_YELLOW, (leds & CHOICE_YELLOW) ? HIGH : LOW);
  digitalWrite(LED_GREEN,  (leds & CHOICE_GREEN)  ? HIGH : LOW);
}

byte wait_for_button(void)
{
  long startTime = millis();

  while ((millis() - startTime) < ENTRY_TIME_LIMIT)
  {
    byte button = checkButton();
    if (button != CHOICE_NONE)
    {
      toner(button, 150);
      while (checkButton() != CHOICE_NONE) ;
      delay(10);
      return button;
    }
  }

  return CHOICE_NONE;
}

byte checkButton(void)
{
  if (digitalRead(BUTTON_RED) == 0) return CHOICE_RED;
  else if (digitalRead(BUTTON_YELLOW) == 0) return CHOICE_YELLOW;
  else if (digitalRead(BUTTON_GREEN) == 0) return CHOICE_GREEN;

  return CHOICE_NONE;
}

void toner(byte which, int buzz_length_ms)
{
  setLEDs(which);

  switch (which)
  {
    case CHOICE_RED:    tone(BUZZER, 440, buzz_length_ms); break;
    case CHOICE_YELLOW: tone(BUZZER, 587, buzz_length_ms); break;
    case CHOICE_GREEN:  tone(BUZZER, 880, buzz_length_ms); break;
  }

  delay(buzz_length_ms);
  noTone(BUZZER);
  setLEDs(CHOICE_OFF);
}

void play_winner(void)
{
  setLEDs(CHOICE_GREEN | CHOICE_YELLOW);
  winner_sound();
  setLEDs(CHOICE_RED);
  winner_sound();
  setLEDs(CHOICE_GREEN | CHOICE_YELLOW);
  winner_sound();
  setLEDs(CHOICE_RED);
  winner_sound();
  setLEDs(CHOICE_OFF);
}

void winner_sound(void)
{
  for (int freq = 300; freq < 1200; freq += 15)
  {
    tone(BUZZER, freq, 3);
    delay(3);
  }
  noTone(BUZZER);
}

void play_loser(void)
{
  setLEDs(CHOICE_RED | CHOICE_GREEN);
  tone(BUZZER, 200, 255);
  delay(300);

  setLEDs(CHOICE_YELLOW);
  tone(BUZZER, 150, 255);
  delay(300);

  setLEDs(CHOICE_RED | CHOICE_GREEN);
  tone(BUZZER, 200, 255);
  delay(300);

  setLEDs(CHOICE_YELLOW);
  tone(BUZZER, 150, 255);
  delay(300);

  noTone(BUZZER);
  setLEDs(CHOICE_OFF);
}

void attractMode(void)
{
  while (1)
  {
    setLEDs(CHOICE_RED);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;

    setLEDs(CHOICE_YELLOW);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;

    setLEDs(CHOICE_GREEN);
    delay(100);
    if (checkButton() != CHOICE_NONE) return;
  }
}

//===================== SERVO =====================

void openDoor(void)
{
  doorServo.write(0);   // move to open position
  delay(4000);           // linger open for 4 seconds
  doorServo.write(70);  // move back to closed/home
}