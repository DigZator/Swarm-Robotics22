unsigned int counter = 0;
volatile byte pulses;
unsigned long timeold;
unsigned int rpm;
// int bla = 6;
// int blb = 9;
int nhole = 20;

int inPin = 2;
int val = 0;

void docount()
{
  counter++;
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  
  pinMode(inPin, INPUT);

  attachInterrupt(0, docount, FALLING);
  pulses = 0;
  rpm = 0;
  timeold = 0;
}

void loop() {
  // put your main code here, to run repeatedly:
  if (millis() - timeold >= 1000){  /*Uptade every one second, this will be equal to reading frecuency (Hz).*/
 
    //Don't process interrupts during calculations
    detachInterrupt(0);
    //Note that this would be 60*1000/(millis() - timeold)*pulses if the interrupt
    //happened once per revolution
    rpm = (60 * 1000 / nhole )/ (millis() - timeold)* pulses;
    timeold = millis();
    pulses = 0;
    
    //Write it out to serial port
    Serial.print("RPM = ");
    Serial.println(rpm,DEC);
    //Restart the interrupt processing
    attachInterrupt(0, docount, FALLING);
  }
}
