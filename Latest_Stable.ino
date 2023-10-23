#include <Adafruit_SSD1306.h>
#include <splash.h>
#include <FastLED.h>
#include <Adafruit_GFX.h>
#include <MIDIUSB.h>

#define Gradient  0                //gradiente de color al bajar el slider 1= SI 0= No
#define MuteLight 1                //Led Rojo indicador de mute
#define MinMidiValue 80            //Valor Minimo del Midi

//Pots 
const int NPots = 4;
const int potPin[NPots] = {21, 20, 19, 18};
int potCState[NPots] = {0};       // Current state of the pot; delete 0 if 0 pots
int potPState[NPots] = {0};       // Previous state of the pot; delete 0 if 0 pots
int potVar = 0;                   // Difference between the current and previous state of the pot
int buttonCState[NPots] = {};        // stores the button current value
int buttonPState[NPots] = {};        // stores the button previous value
int midiCState[NPots] = {0}; // Current state of the midi value; delete 0 if 0 pots
int midiPState[NPots] = {0}; // Previous state of the midi value; delete 0 if 0 pots


const int TIMEOUT = 100;          //* Amount of time the potentiometer will be read after it exceeds the varThreshold
const int varThreshold = 10;      //* Threshold for the potentiometer signal variation
boolean potMoving = true;         // If the potentiometer is moving
unsigned long PTime[NPots] = {0}; // Previously stored time; delete 0 if 0 pots
unsigned long timer[NPots] = {0}; // Stores the time that has elapsed since the timer was reset; delete 0 if 0 pots

// MIDI Assignments 
byte midiCh = 1;                  //* MIDI channel to be used
byte cc = 1;                       //* Lowest MIDI CC to be used

//ws2812
#define NUM_LEDS 27        // Número de LEDs WS2812
#define LED_PIN 10         // Pin de datos de la tira de LEDs

//Encoder
#define PIN_A 1           //Pin A encoder
#define PIN_B 0           //Pin B Encoder
#define Encoder_button 7  //Pin Boton del encoder

volatile int encoderPos = 0;
volatile int lastEncoded = 0;
int pagecount = 0;

//Oled Screen
#define OLED_RESET     4
Adafruit_SSD1306 display(OLED_RESET);


const unsigned char Bitmap_01[] PROGMEM = { //setings
  // 'Settings, 25x25px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 
	0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x22, 0x00, 
	0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x02, 0x20, 0x00, 
	0x00, 0x04, 0x40, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 
	0x00, 0x4c, 0x00, 0x00, 0x03, 0x88, 0x00, 0x00, 0x04, 0x90, 0x00, 0x00, 0x08, 0x60, 0x00, 0x00, 
	0x08, 0x20, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x11, 0x80, 0x00, 0x00, 
	0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char Bitmap_02[] PROGMEM = {  //brush
	// 'Brush, 28x28px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 
	0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x22, 0x00, 
	0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x02, 0x20, 0x00, 
	0x00, 0x04, 0x40, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 
	0x00, 0x4c, 0x00, 0x00, 0x03, 0x88, 0x00, 0x00, 0x04, 0x90, 0x00, 0x00, 0x08, 0x60, 0x00, 0x00, 
	0x08, 0x20, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x11, 0x80, 0x00, 0x00, 
	0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
const unsigned char Bitmap_03[] PROGMEM = {  //brush
	// 'Brush, 28x28px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 
	0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x22, 0x00, 
	0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x02, 0x20, 0x00, 
	0x00, 0x04, 0x40, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 
	0x00, 0x4c, 0x00, 0x00, 0x03, 0x88, 0x00, 0x00, 0x04, 0x90, 0x00, 0x00, 0x08, 0x60, 0x00, 0x00, 
	0x08, 0x20, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x11, 0x80, 0x00, 0x00, 
	0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
  const unsigned char Bitmap_04[] PROGMEM = {  //brush
	// 'Brush, 28x28px
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0x80, 
	0x00, 0x00, 0x04, 0x80, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x22, 0x00, 
	0x00, 0x00, 0x44, 0x00, 0x00, 0x00, 0x88, 0x00, 0x00, 0x01, 0x10, 0x00, 0x00, 0x02, 0x20, 0x00, 
	0x00, 0x04, 0x40, 0x00, 0x00, 0x08, 0x80, 0x00, 0x00, 0x11, 0x00, 0x00, 0x00, 0x32, 0x00, 0x00, 
	0x00, 0x4c, 0x00, 0x00, 0x03, 0x88, 0x00, 0x00, 0x04, 0x90, 0x00, 0x00, 0x08, 0x60, 0x00, 0x00, 
	0x08, 0x20, 0x00, 0x00, 0x08, 0x20, 0x00, 0x00, 0x08, 0x40, 0x00, 0x00, 0x11, 0x80, 0x00, 0x00, 
	0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

const uint8_t *Bitmap[] = {
    Bitmap_01, Bitmap_02, Bitmap_03, Bitmap_04
};

CRGB leds[NUM_LEDS];      // Arreglo para almacenar los colores de los LEDs

void setup() {
  pinMode(PIN_A, INPUT_PULLUP);
  pinMode(PIN_B, INPUT_PULLUP);

  attachInterrupt(digitalPinToInterrupt(PIN_A), updateEncoder, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_B), updateEncoder, CHANGE);


  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);  // Configurar el tipo de tira de LEDs
  
  FastLED.setBrightness(255); // Establecer el brillo inicial de los LEDs
  FastLED.show();             // Actualizar los LEDs con el brillo inicial

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display.setRotation(1);
  display.clearDisplay();
  Serial.begin(9600);

}

void loop() {
  potentiometers();

  int MaxBrightness = 255;                                         // Establece el brillo máximo de los LEDs
  Led_Pot(21,  0,  4, CHS(21, 200, 255, MaxBrightness));           // Establece los parámetros de las luces del Pot(X), así como el color CHS
  Led_Pot(20,  5,  9, CHS(20, 200, 255, MaxBrightness));           // Establece los parámetros de las luces del Pot(X), así como el color CHS
  Led_Pot(19, 10, 14, CHS(19, 200, 255, MaxBrightness));           // Establece los parámetros de las luces del Pot(X), así como el color CHS
  Led_Pot(18, 15, 19, CHS(18, 200, 255, MaxBrightness));           // Establece los parámetros de las luces del Pot(X), así como el color CHS


  for (int i = 20; i < 27; i++) {                                  // Ajustar el brillo de cada LED en función de la posición actual del potenciómetro
    leds[i] = CHSV(200, 255, 255);                                 // Cambiar el color y el brillo del LED
  }
  FastLED.show();
  display.display();
  Serial.println(analogRead(18));
  Interface(encoderPos/4, 1);
  
  
}

CHSV CHS(int Pot, int C, int H, int B) {
  int potValue = analogRead(Pot);
    int S = 255;
    if(Gradient == 1){
      S = map(potValue, 1023, 0, 0, B);
    }
  return CHSV(C, H, S);
}

void Interface(int Page, int Layer){

      if(Page > 4 ){
        Page == 1;
      }
      if(Page < 0){
        Page == 4;
      }

    display.clearDisplay();

                   //Y  X  W   H    Color
    display.drawRect(0, 0, 32, 127, WHITE); //rectangulo interfaz
    for(int i = 0; i < 4; i++){           //cuadrados de la interfaz 
      display.drawRect(2, (29*i)+2, 28, 28, WHITE); 
    }


    if(Layer == 1){
      for(int i = 0; i < 4; i++){           //cuadrados de la interfaz 
       display.drawBitmap(3, (29*i)+3, Bitmap[i] , 28, 28, WHITE);
      }
      for(int i = 0; i < 4; i++){;
        display.drawRect(2, (29*Page)+2, 28, 28, BLACK);
        display.drawBitmap(3, (29*Page)+3, Bitmap[i], 28, 28, BLACK);
      }
    }


/*
  if(Page==1){
    display.drawBitmap(3, 2, Bitmap[1] , 25, 25, WHITE); // dibujamos Bitmap

  }
  if(Page==2){
    display.drawBitmap(3, 2, Bitmap[2] , 25, 25, WHITE); // dibujamos Bitmap

  }*/
  
}

void Led_Pot(int Pot, int Start, int End, CHSV color) {
  int potValue = 0;
  int smoothedValue = 0;
  int samples = 10;                                             // Número de muestras para promediar

  for (int i = 0; i < samples; i++) {
    potValue += analogRead(Pot);                                // Leer el valor del potenciómetro y acumularlo
    delay(2);                                                   // Retardo entre muestras para suavizar la entrada
  }

  smoothedValue = potValue / samples;                            // Calcular el promedio de las muestras
  int ledCount = map(smoothedValue, 1000, 0, Start, End);        // Mapear el valor promediado del potenciómetro al número de LEDs encendidos

  for (int i = End; i > ledCount; i--) {                       // Ajustar el brillo de cada LED en función de la posición actual del potenciómetro
    leds[i] = CHSV(0, 0, 0);                                            // Cambiar el color y el brillo del LED
  }

  for (int i = ledCount; i > Start; i--) {                         // Apagar los LEDs restantes
    leds[i] =   color;                                     // Establecer el color del LED como apagado
  }
  if(MuteLight==1){
    if(analogRead(Pot) > 1010){
    leds[ledCount] = CHSV(0, 255, 255);
    }
    else{
      leds[ledCount] = color;
    }
  }
  
  FastLED.show(); // Actualizar los LEDs
}

void updateEncoder() {
  int MSB = digitalRead(PIN_A);             // Lectura del pin A
  int LSB = digitalRead(PIN_B);             // Lectura del pin B
  int encoded = (MSB << 1) | LSB;           // Combina los valores de los pines A y B
  int sum = (lastEncoded << 2) | encoded;   // Calcula el movimiento del encoder
  delay(10);
  if (sum == 0b1101 || sum == 0b0100 || sum == 0b0010 || sum == 0b1011) { // Si el encoder gira en sentido horario
    encoderPos++;
  }
  if (sum == 0b1110 || sum == 0b0111 || sum == 0b0001 || sum == 0b1000) { // Si el encoder gira en sentido antihorario
    encoderPos--;
  }
  lastEncoded = encoded;  // Actualiza el valor del último estado
  return encoderPos;
}

void potentiometers() {
  for (int i = 0; i < NPots; i++) { // Loops through all the potentiometers
    potCState[i] = analogRead(potPin[i]); // reads the pins from arduino
    midiCState[i] = map(potCState[i], 0, 1023, 127, MinMidiValue); // Maps the reading of the potCState to a value usable in midi
    potVar = abs(potCState[i] - potPState[i]);
                                          // Calculates the absolute value between the difference between the current and previous state of the pot
    if (potVar > varThreshold) {          // Opens the gate if the potentiometer variation is greater than the threshold
      PTime[i] = millis();                // Stores the previous time
    }

    timer[i] = millis() - PTime[i];       // Resets the timer 11000 - 11000 = 0ms
    if (timer[i] < TIMEOUT) {             // If the timer is less than the maximum allowed time it means that the potentiometer is still moving
      potMoving = true;
    }
    else {
      potMoving = false;
    }
    if (potMoving == true) { // If the potentiometer is still moving, send the change control
      if (midiPState[i] != midiCState[i]) {

        // Sends  MIDI CC 
        // Use if using with ATmega32U4 (micro, pro micro, leonardo...)
        controlChange(midiCh, cc + i, midiCState[i]); //  (channel, CC number,  CC value)
        MidiUSB.flush();

        potPState[i] = potCState[i]; // Stores the current reading of the potentiometer to compare with the next
        midiPState[i] = midiCState[i];
      }
    }

      if(potCState[i] >= 1010 & buttonCState[i] == 0){
          buttonCState[i] = 1;
          noteOn(midiCh, 35 + i, 127);    // (canal, nota, velocidad)
          MidiUSB.flush();
      }
      if(potCState[i] < 1010 & buttonCState[i] == 1){
          buttonCState[i] = 0;
          noteOn(midiCh, 35 + i, 127);    // (canal, nota, velocidad)
          MidiUSB.flush();
      }
  }
}    
  
void controlChange(byte channel, byte control, byte value) {
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

