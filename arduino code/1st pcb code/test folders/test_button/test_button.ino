// constants won't change. They're used here to set pin numbers:
const int buttonPin = PA_2;  // the number of the pushbutton pin
const int ledPin = PB_0;    // the number of the LED pin
const int buzzerPin = PA_1;   // Replace with your pin


// variables will change:
int buttonState = 0;  // variable for reading the pushbutton status


void setup() {
  // initialize the LED pin as an output:
  pinMode(ledPin, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(buttonPin, INPUT);
  tone(buzzerPin, 2000);     // 2 kHz tone
  delay(500);
  noTone(buzzerPin);
}

void loop() {
  // read the state of the pushbutton value:
  buttonState = digitalRead(buttonPin);

  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    digitalWrite(ledPin, HIGH);
    tone(buzzerPin, 2000); 
  } else {
    // turn LED off:
    noTone(buzzerPin);
    digitalWrite(ledPin, LOW);
  }
}

