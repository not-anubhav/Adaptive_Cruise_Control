#include<LiquidCrystal.h>        //header file for lcd
LiquidCrystal lcd(2,3,4,5,6,7);  // sets the interfacing pins for lcd


unsigned long echo =14;
int accSensorInput = 12;                  
float distance = 0;
float duration = 0;
float speed;
float Reducespeed;
float PreviousSpeed;
float accCruisingSpeed;
float accPrevCruisingSpeed;
float accDistanceLastValue;
int settingsCHangeFlag = 1;
int accBreakLightOutput = 1;
int accVehicleMass = 1500;

float accBrakingKE;
float accBrakingForce;
float accVelocityRateChange;

char accCruiseSpeedSelcSw1;
char accCruiseSpeedSelcSw2;
char accCruiseSpeedSelcSw3;
char accCruiseSpeedSelcSw4;

int cruiseControlEnable = 0;

void setup() 
{
  Serial.begin(9600);					// for debugging
  lcd.begin(16, 2);                    	// initializes the 16x2 LCD
  pinMode(accSensorInput,OUTPUT);      	// ultra pin which is PB4 of ultrasonic sensor is made output
  pinMode(echo,INPUT);
  pinMode(23,INPUT);                   	//sw1 pin PC0 made as input
  pinMode(24,INPUT);                   	//sw2 pin PC1 made as input
  pinMode(9,OUTPUT);
  pinMode(8,OUTPUT);
  pinMode(10,OUTPUT);
  pinMode(11,OUTPUT);
  
  pinMode(accBreakLightOutput, OUTPUT);	//p13 as a break light
  
  pinMode(13,OUTPUT);
  
  digitalWrite(13,LOW);
}

void velocity(unsigned long distance,int accCruisingSpeed);

//Function for ultrasonic sensor
 unsigned long ping()
{
   pinMode(accSensorInput,OUTPUT);                      //ultrasonic sensor pin first made as output
   digitalWrite(accSensorInput,LOW);                    // send low on pin 12 i.e. ultra
   delayMicroseconds(2);
   digitalWrite(accSensorInput,HIGH);                   // send high on pin 12 i.e. ultra
   delayMicroseconds(5);
   digitalWrite(accSensorInput,LOW);                    // send low on pin 12 i.e. ultra
   pinMode(accSensorInput,INPUT);                       //ultrasonic sensor pin made as input
   digitalWrite(accSensorInput,HIGH);                   // send high on pin 12 i.e. ultra
   //echo=pulseIn(accSensorInput,HIGH);                   // check duration of high pulse returned by ultrasonic sensor
   //distance=(pulseIn(accSensorInput,HIGH)/58.823);        // convert timing of pulse to distance using speed of sound
   duration = pulseIn(echo, HIGH);
  // Calculating the distance
  distance = duration * 0.034 / 2;
   return(distance);    
}


// the loop runs over and over again:
void loop() 
{
  lcd.setCursor(0,0);

  
  
  if(digitalRead(13)==HIGH)
  {
  	//lcd.print("Cruise Enable");
	cruiseControlEnable = 1;
	
	  distance=ping();                                                     //Function call 
  
		
		  if((digitalRead(8)==HIGH) &&(digitalRead(9)==LOW))                   //sw1:Car cruising at 40kmph  
		  {
			accCruisingSpeed=60;
			// lcd.print("sw1 ");
			velocity(distance,accCruisingSpeed);                                           //Function call for speed reduction
		  }
		  else if((digitalRead(9)==HIGH) && (digitalRead(8)==LOW))              //sw2:Car cruising at 60kmph
		  {
			accCruisingSpeed=80;                                         
			//lcd.print("sw2 ");
			velocity(distance,accCruisingSpeed);                     					      //Function call for speed reduction
		  }
		  else if((digitalRead(9)==LOW &&(digitalRead(8)==LOW)) || (digitalRead(9)==HIGH &&(digitalRead(8)==HIGH)))                   //No switch pressed car in normal mode and not in cruise mode
		  {
			lcd.setCursor(0,0);
			//Serial.println("car in normal mode");  
            
			lcd.print("Normal Mode       ");
		   
		  }
  
		//velocity(distance,accCruisingSpeed); 
		Serial.print("AccSpeed:");
		Serial.println(accCruisingSpeed);
	  
	  if(accPrevCruisingSpeed != accCruisingSpeed)
	  {
		settingsCHangeFlag = 1;
		accPrevCruisingSpeed = accCruisingSpeed;
	  }
  
  }else if(digitalRead(13)==LOW)
  {
	lcd.setCursor(0,0);
	lcd.print("Cruise Disable");
	cruiseControlEnable = 0;
	
	accPrevCruisingSpeed = 0;
	distance = 0;
	
	lcd.setCursor(0,1);
	lcd.print("Speed is:");
    lcd.print(speed);
    lcd.print("kmph");
  }
}

//Function for speed reduction

void velocity(unsigned long distance,int accCruisingSpeed)   //Function definition
{
	float factor = 0;
   lcd.setCursor(4,0);
   lcd.setCursor(0,0);

	if(distance < 300.0)
	{
		accDistanceLastValue = distance;
	}
	
  if ((accDistanceLastValue < 30.0) && (distance > 300.0))
	{
		accCruisingSpeed = 0;
		speed=accCruisingSpeed;
	}
  

  lcd.print("Cruising:");
  lcd.print(accCruisingSpeed);
  lcd.print("kmph");
  
  
  if(settingsCHangeFlag == 1)
  {
	  speed=accCruisingSpeed;
	  settingsCHangeFlag = 0;
  }
 
	if (((distance > 30.0) || (distance < 300.0)) && (accDistanceLastValue > 30.0) && (speed <= accCruisingSpeed))
	{
		factor = (300.0/distance)*0.5;
		speed = speed-((factor)*(speed/6.0)) +7; 
		if (speed > accCruisingSpeed)
		{
			speed = accCruisingSpeed;
		}
	}
	
	if (accDistanceLastValue < 30.0)
	{
		speed = 0;
	}

 //Break Light Logic
  if ((speed) < PreviousSpeed)
  {
	  //Apply Brake
	  digitalWrite(accBreakLightOutput, HIGH);
	  
		accBrakingKE = (float)(0.5 * (float)accVehicleMass * (speed/3.6) * (speed/3.6));
		accBrakingForce = (float)(accBrakingKE/distance);
  }
  else
  {
	  ////Release Brake
	  digitalWrite(accBreakLightOutput, LOW);
	  accBrakingForce = 0;
  }

	accVelocityRateChange = (2 * (speed - (PreviousSpeed * 1))) / 1;
	
	PreviousSpeed = speed;
	
	lcd.setCursor(0,1);
	lcd.print("Speed is:");
    lcd.print(speed);
    lcd.print("kmph");
}
