
//PROGRAMMA PROJECT MECHATRONICA; FRITU

//include libraries
#include <Stepper.h>
#include <Button.h>


//declare variables:
//Algemene variablen
int Statevar = 1;
int LDR1 = 5;
int LDR2 = 6;
int LDR3 = 7;
int Mandhoek;

//SELECTIESTATE
Button button1 = Button(1, PULLDOWN);
Button button2 = Button(2, PULLDOWN);
Button button3 = Button(3, PULLDOWN);
Button button4 = Button(4, PULLDOWN);
Button button5 = Button(5, PULLDOWN);
byte pressCount1 = 0;
byte pressCount2 = 0;
byte pressCount3 = 0;
byte pressCount4 = 0;
int maxpersnack = 5;
int maxtotal = 10;

// STEPPERSTATE
int in11Pin = 12;
int in12Pin = 11;
int in13Pin = 10;
int in14Pin = 9;
int in21Pin = 8;
int in22Pin = 7;
int in23Pin = 6;
int in24Pin = 5;
int in31Pin = 4;
int in32Pin = 3;
int in33Pin = 2;
int in34Pin = 1;
int StepsPerRev = 200;
Stepper motor1(StepsPerRev, in11Pin, in12Pin, in13Pin, in14Pin);
Stepper motor2(StepsPerRev, in21Pin, in22Pin, in23Pin, in24Pin);
Stepper motor3(StepsPerRev, in31Pin, in32Pin, in33Pin, in34Pin);

//PIDSTATE
int ActPos = A0;      // input pin feedback signal
int DesPos = A1;      // input pin control signal
int PWMOutput;
long Error[10];
long Accumulator;
long PID;
int PTerm;
int ITerm;
int DTerm;
int Divider;
int motorPin1 = 9;
int motorPin2 = 10;


void setup()
{
  //SerieleMonitor
  Serial.begin(9600);
  //STEPPERPINS
  pinMode(in11Pin, OUTPUT);
  pinMode(in12Pin, OUTPUT);
  pinMode(in13Pin, OUTPUT);
  pinMode(in14Pin, OUTPUT);
  pinMode(in21Pin, OUTPUT);
  pinMode(in22Pin, OUTPUT);
  pinMode(in23Pin, OUTPUT);
  pinMode(in24Pin, OUTPUT);
  pinMode(in31Pin, OUTPUT);
  pinMode(in32Pin, OUTPUT);
  pinMode(in33Pin, OUTPUT);
  pinMode(in34Pin, OUTPUT);
  motor1.setSpeed(60);
  motor2.setSpeed(60);
  motor3.setSpeed(60);

}


//PID Functies
void GetError(void)
{
  byte i = 0;
  // read analogs
  word ActualPosition = analogRead(ActPos);
  // comment out to speed up PID loop
  Serial.print("ActPos= ");
  Serial.println(ActualPosition, DEC);

  word DesiredPosition = analogRead(DesPos);
  // comment out to speed up PID loop
  Serial.print("DesPos= ");
  Serial.println(DesiredPosition, DEC);

  // shift error values
  for (i = 9; i > 0; i--)
    Error[i] = Error[i - 1];
  // load new error into top array spot
  Error[0] = (long)DesiredPosition - (long)ActualPosition;
  // comment out to speed up PID loop
  Serial.print("Error= ");
  Serial.println(Error[0], DEC);
}

/* CalculatePID():
  Error[0] is used for latest error, Error[9] with the DTERM
*/
void CalculatePID(void)
{
  // Set constants here
  PTerm = 1000;
  ITerm = 25;
  DTerm = 0;
  Divider = 10;

  // Calculate the PID
  PID = Error[0] * PTerm;               // start with proportional gain
  Accumulator += Error[0];              // accumulator is sum of errors
  PID += ITerm * Accumulator;           // add integral gain and error accumulation
  PID += DTerm * (Error[0] - Error[9]); // differential gain comes next
  PID = PID >> Divider;                 // scale PID down with divider

  // comment out to speed up PID loop
  Serial.print("PID= ");
  Serial.println(PID, DEC);

  // limit the PID to the resolution we have for the PWM variable


  if (PID >= 255)
    PID = 255;
  if (PID <= -255)
    PID = -255;

  //PWM output should be between 1 and 254 so we add to the PID

  PWMOutput = PID;

  if (PWMOutput > 0)
  {
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    analogWrite(motorPin1, PWMOutput);
  }
  if (PWMOutput < 0)
  {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    analogWrite(motorPin2, abs(PWMOutput));
  }
  else
  {
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
  }


  // comment out to speed up PID loop
  Serial.print("PWMOutput= ");
  Serial.println(PWMOutput, DEC);
}



void loop() {
  switch (Statevar) {

    case 1:              //STATE 1: DIAGNOSTICS
      {
        Serial.print(Statevar);
        //Snack voorraad checken
        int drempelwaarde = 700;

        while (analogRead(LDR1) >= drempelwaarde) {
          Serial.print("Geen bitterballen");
          delay(500);
        }

        while (analogRead(LDR2) >= drempelwaarde) {
          Serial.print("Geen kipnuggets");
          delay(500);
        }

        while (analogRead(LDR3) >= drempelwaarde) {
          Serial.print("Geen bami");
          delay(500);
        }
        Statevar++;

      }
      break;

    case 2:              //STATE 2: SELECTIEPROCES
      {
        Serial.print(Statevar);
        if (button1.uniquePress()) {
          pressCount1++;
        }
        if (button2.uniquePress()) {
          pressCount2++;
        }
        if (button3.uniquePress()) {
          pressCount3++;
        }
        if (button4.uniquePress()) {
          Serial.println("RESET");
          pressCount1 = 0;
          pressCount2 = 0;
          pressCount3 = 0;
        }
        if (button5.uniquePress()) {
          Statevar++;
        }
        if (pressCount1 || pressCount2 || pressCount3 >= maxpersnack) {
          pressCount1 == maxpersnack;
          pressCount2 == maxpersnack;
          pressCount3 == maxpersnack;
          Serial.println("Maximaal 5 per type snack");
        }
        if (pressCount1 + pressCount2 + pressCount3 == maxtotal) {
          Serial.println("Maximaal 10 snacks, RESET or PROCEED (OK)");
          if (button4.uniquePress()) {
            Serial.println("RESET");
            pressCount1 = 0;
            pressCount2 = 0;
            pressCount3 = 0;
          }
          if (button5.uniquePress()) {
            Statevar++;
          }
        }
        Serial.print("Aantal bitterballen = ");
        Serial.println(pressCount1);
        Serial.print("Aantal kipnuggets = ");
        Serial.println(pressCount2);
        Serial.print("Aantal bami = ");
        Serial.println(pressCount3);

        delay(50);
      }
      break;

    case 3: //STEPPERMOTORs
      {
        Serial.print(Statevar);
        motor1.step(pressCount1 * StepsPerRev);
        delay(1000);

        motor2.step(pressCount2 * StepsPerRev);
        delay(1000);

        motor3.step(pressCount3 * StepsPerRev);
        delay(1000);
        Statevar++;
      }
      break;

    case 6:              //STATE 4: FRITUUR
      {
        GetError();
        CalculatePID();
      }
      break;


  }
  delay(1);
}

