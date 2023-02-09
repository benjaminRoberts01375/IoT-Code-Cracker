// include the library code:
#include <LiquidCrystal.h>


// LCD
LiquidCrystal lcd(4, 5, 6, 7, 8, 9);
const int X_pin = 0;
const int Y_pin = 1;
const int Button_pin = 2;
const int codeLen = 4;
const int maxNumSize = 9;
unsigned long timeSinceScroll = 0;

// LED
#define BLUE 3
#define GREEN 10
#define RED 11

// Code generation and management
int selectedIndex = 0;
int correctCode[codeLen] = { 0, 0, 0, 0 };
int currentCode[codeLen] = { 0, 0, 0, 0 };


void Reset() {
  for (int i = 0; i < codeLen; i++) {
    currentCode[i] = 0;               // Reset the user's input
  }

  lcd.noBlink();                      // Prevent cursor from blinking
  lcd.clear();                        // Clear the screen
  for (int i = 0; i < codeLen; i++) {
    correctCode[i] = random(10);      // Generate a new code
  }
  lcd.setCursor(0, 0);                // Move cursor to top
  lcd.print("Code Cracker");          // Show game's name
  lcd.setCursor(0, 1);                // Move cursor to bottom
  for (int i = 0; i < codeLen; i++) {
    lcd.print(currentCode[i]);        // Display the user's current code
  }
  lcd.setCursor(0, 1);                // Move the cursor back to the beginning of the bottom row
  lcd.blink();                        // Allow the cursor to blink
}

void Controls() {
  if (millis() - timeSinceScroll > 500) {                                                 // Only allow inputs every 500ms
    if (analogRead(X_pin) > 700) {                                                        // If joy stick is going to the right
      selectedIndex = (selectedIndex + 1) % codeLen;                                      // Add to the index
      lcd.setCursor(selectedIndex, 1);                                                    // Track cursor forward 1
      timeSinceScroll = millis();                                                         // Track when the input occured
    } 
    else if (analogRead(X_pin) < 300) {                                                   // If joy stick is going to the left
      if (selectedIndex == 0) {                                                           // If we need to wrap around
        selectedIndex = codeLen - 1;                                                      // Then wrap around
      } else {  
        selectedIndex = (selectedIndex - 1) % codeLen;                                    // Track cursor back 1
      }
      lcd.setCursor(selectedIndex, 1);                                                    // Move cursor back 1
      timeSinceScroll = millis();                                                         // Track when the last input occured
    } 
    else if (analogRead(Y_pin) > 700) {                                                   // If joystick was moved up 1
      currentCode[selectedIndex] = (currentCode[selectedIndex] + 1) % (maxNumSize + 1);   // Track digit increase
      lcd.print(currentCode[selectedIndex]);                                              // Display new number
      lcd.setCursor(selectedIndex, 1);                                                    // Move cursor back to the selected digit
      timeSinceScroll = millis();                                                         // Track when the last input occured
    } 
    else if (analogRead(Y_pin) < 300) {                                                   // If joystick was moved down 1
      if (currentCode[selectedIndex] <= 0) {                                              // If we need to wrap the digit around
        currentCode[selectedIndex] = maxNumSize;                                          // Then wrap around
      } else {
        currentCode[selectedIndex] = (currentCode[selectedIndex] - 1) % (maxNumSize + 1); // Add 1 to the selected digit
      }
      lcd.print(currentCode[selectedIndex]);                                              // Print the update
      lcd.setCursor(selectedIndex, 1);                                                    // Move cursor back to selected digit
      timeSinceScroll = millis();                                                         // Track when the last input occured
    } 
  }
}

void LED() {
  int difference = abs(currentCode[selectedIndex] - correctCode[selectedIndex]);
  analogWrite(GREEN, map(difference, 0, 9, 50, 0));
  analogWrite(RED, map(difference, 0, 9, 0, 50));
}

void IsEnd() {
  if (digitalRead(Button_pin) == 0) {         // Submit button pressed
    lcd.noBlink();                            // Prevent blinking cursor
    bool isCorrect = true;
    for (int i = 0; i < codeLen; i++) {       // Check each digit... 
      if (correctCode[i] != currentCode[i]) { // ...to ensure it's correct
        isCorrect = false;                    // If one isn't then track it
        break;
      }
    }

    lcd.clear();                              // Remove all text from screen 
    lcd.setCursor(0, 0);                      // Move cursor to top

    if (isCorrect == true) {                  // If the user one...
      lcd.print("You win!");                  // ...display they one
    } else {                                  // If the user lost...
      lcd.print("You lose!");                 // ...display they lost
    }
    lcd.setCursor(0, 1);                      // Go to second line
    lcd.print("Code: ");                      // Show prefix for correct code
    for (int i = 0; i < codeLen; i++) {
      lcd.print(correctCode[i]);              // Show each digit of the code...
    }
    delay(5000);                              // ...for 5 seconds
    Reset();                                  // Reset the game
  }
}

void setup() {
  // Setup printing to console
  Serial.begin(9600);


  // Setup random seed
  randomSeed(analogRead(5));      // Setup random seed based off of analog input 5


  // Setup joy stick
  pinMode(Button_pin, INPUT);
  digitalWrite(Button_pin, HIGH); // Set default state of joystick button


  // Setup LCD
  lcd.begin(16, 2);               // Setup the LCD's number of columns and rows
  Reset();


  // Setup LED
  pinMode(RED, OUTPUT);
  pinMode(GREEN, OUTPUT);
  pinMode(BLUE, OUTPUT);
  digitalWrite(RED, LOW);
  digitalWrite(GREEN, LOW);
  digitalWrite(BLUE, LOW);
}


void loop() {
  Controls();
  LED();
  IsEnd();
}

