#include <Wire.h>
#include <Adafruit_Si4713.h>

#define RESETPIN 12
#define SCANPIN 1


Adafruit_Si4713 radio = Adafruit_Si4713(RESETPIN);
int freq=9710;      // 10230 == 102.30 MHz

void setup() {
  Serial.begin(9600);
  Serial.println("SCRUB FM Test");

  if (! radio.begin()) {  // begin with address 0x63 (CS high default)
    Serial.println("you haven't connected the reset pin have you you ponce?");
    while (1);
  }
  
  radio.setGPIOctrl(_BV(1) | _BV(2));  // set GP1 and GP2 to output
  radio_start();
  freq_scan();
  radio_start();
}

void radio_start()
{
  Serial.print("\nTuning into "); 
  Serial.print(freq/100); 
  Serial.print('.'); 
  Serial.println(freq % 100);
  radio.tuneFM(freq); // 102.3 mhz

  // This will tell you the status in case you want to read it from the chip
  radio.readTuneStatus();
  Serial.print("\tCurr freq: "); 
  Serial.println(radio.currFreq);
  Serial.print("\tCurr freqdBuV:"); 
  Serial.println(radio.currdBuV);
  Serial.print("\tCurr ANTcap:"); 
  Serial.println(radio.currAntCap);
  
  Serial.print("\nSet TX power");
  radio.setTXpower(115);  // dBuV, 88-115 max


  // begin the RDS/RDBS transmission
  radio.beginRDS();
  radio.setRDSstation("SCRUB FM");
  radio.setRDSbuffer( "JULIAN CHARLES IS A WASTER");

  Serial.println("RDS on!");  

}

void freq_scan()
{
  int k = 40;     //this is the starting noise threshold
  int cont = 1;   //used to control frequency search
  
  while(cont == 1)
  {
    //scan frequency range from 87.5MHz to 10.8MHz
    for (int f  = 8750; f<10800; f+=10) 
    {
      radio.readTuneMeasure(f);
      Serial.print("Measuring "); Serial.print(f); Serial.print("...");
      radio.readTuneStatus();
      Serial.println(radio.currNoiseLevel);
      
      //checks if current noise level is at or below threshold
      if(radio.currNoiseLevel <= k)
      {
        //sets new threshold value
        k = radio.currNoiseLevel;
        //writes new frequency value
        freq = f;
      }
       
    }
    Serial.print("\nnew k value is :\n");
    Serial.print(k);
    Serial.print("\nnew FM value is :\n");
    Serial.print(freq);
    k--;    //reduce threshold
    
    radio.setGPIO(_BV(2));
    //it is unlikely for the noise to be below 25 
    //so at this point we will break the loop
    if (k <= 25)
    {
      cont = 0;
    }
  }
}



void loop() {
  radio.readASQ();
  //Serial.print("\tCurr ASQ: 0x"); 
  //Serial.println(radio.currASQ, HEX);
  //Serial.print("\tCurr InLevel:"); 
  //Serial.println(radio.currInLevel);
  // toggle GPO1 and GPO2
  radio.setGPIO(_BV(1));
  delay(500);
  radio.setGPIO(_BV(2));
  delay(500);
  
}
