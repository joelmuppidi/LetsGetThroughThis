int coolPin=11;
int heatPin=10;
int powr=12;
int off=13;

int fanblower=6;


int ambient_temp_sensor_pin = A0;
int ambient_temp;
int sunload_sensor_pin= A1;
int sunload_value;
int automan_pin= 2;
int automan_value;
int fanspeed_pin= A2;
int fanspeed_value_m;
int set_temp_pin= A3;

int set_temp_value;
int cooler_heater;
int set_ac_intensity;
int fanspeed_value_a;
int power_ac;
int fanspeed_final;



void setup()
{
  Serial.begin(9600);
  pinMode(A0,INPUT);
  pinMode(A1,INPUT);
  pinMode(2,INPUT);
  pinMode(A2,INPUT);
  pinMode(A3,INPUT);
  
  pinMode(13,OUTPUT);
  pinMode(12,OUTPUT);
  pinMode(11,OUTPUT);
  pinMode(10,OUTPUT);
  
  pinMode(6,OUTPUT);
  
}

void loop()
{
  ambient_temp=(int)read_temp(ambient_temp_sensor_pin);
  sunload_value=read_sunload(sunload_sensor_pin);
  automan_value=read_automan(automan_pin);
  fanspeed_value_m=fanspeed(fanspeed_pin);
  set_temp_value=set_temp(set_temp_pin);
  
  switch(automan_value){
    case 0:
     if(fanspeed_value_m){
       if (abs(ambient_temp-set_temp_value)<1) //fan only
      		power_ac=0;
    	else if(ambient_temp>set_temp_value){   //cooler
      		power_ac=1;
      		cooler_heater=0;
          if(sunload_value<2){
        	set_ac_intensity=0;
          }
          else{
        	set_ac_intensity=1;
          }
        }
    	else if (ambient_temp<set_temp_value){  //heater
      		power_ac=1;
      		cooler_heater=1;
          if(sunload_value<2){
        	set_ac_intensity=1;
          }
          else{
        	set_ac_intensity=0;
          }
        }
      }
     else{
        power_ac=0;
      
      }
     fanspeed_final=fanspeed_value_m;
    	break;
          
    case 1:
        if (abs(ambient_temp-set_temp_value)<1){ //fan only
      		power_ac=0;
          	fanspeed_value_a=1;
         }
    	else if(ambient_temp>set_temp_value){    //cooler
      		power_ac=1;
      		cooler_heater=0;
          if(abs(ambient_temp-set_temp_value)<2){ //cooler low temp diff
            fanspeed_value_a=1;
            if(sunload_value<2){
        	  set_ac_intensity=0;
          	}
          	else{
        	  set_ac_intensity=1;
            }
          }
          else if (abs(ambient_temp-set_temp_value)<4){ //cooler med temp diff
            fanspeed_value_a=2;
            if(sunload_value<2){
        	  set_ac_intensity=0;
          	}
          	else{
        	  set_ac_intensity=1;
            }
          }
          else{                         				//cooler high temp diff
            fanspeed_value_a=3;
            if(sunload_value<2){
        	  set_ac_intensity=0;
          	}
          	else{
        	  set_ac_intensity=1;
            }
          }
         
          
        }
    	else if (ambient_temp<set_temp_value){		//heater
      		power_ac=1;
      		cooler_heater=1;
          if(abs(ambient_temp-set_temp_value)<2){ //heater low temp diff
            fanspeed_value_a=1;
            if(sunload_value<2){
        	  set_ac_intensity=1;
          	}
          	else{
        	  set_ac_intensity=0;
            }
          }
          else if (abs(ambient_temp-set_temp_value)<4){ //heater med temp diff
            fanspeed_value_a=2;
            if(sunload_value<2){
        	  set_ac_intensity=1;
          	}
          	else{
        	  set_ac_intensity=0;
            }
          }
          else{                         				//heater high temp diff
            fanspeed_value_a=3;
            if(sunload_value<2){
        	  set_ac_intensity=1;
          	}
          	else{
        	  set_ac_intensity=0;
            }
          }
        }
    fanspeed_final=fanspeed_value_a;
    	break;
    }
  
  
  
 
    
  Serial.print("temp:");Serial.print(ambient_temp);
  Serial.print("| set temp:");Serial.print(set_temp_value);
  Serial.print("| sun:");Serial.print(sunload_value);
  Serial.print("| fan:");Serial.print(fanspeed_final);
  Serial.print("| acIn:");Serial.print(set_ac_intensity);
  Serial.print("| acPow:");Serial.print(power_ac);
  Serial.print("| c/h:");Serial.print(cooler_heater);
  Serial.print("| M/A:");Serial.print(automan_value);
  
  Serial.println("||");
  
  if(power_ac){
    digitalWrite(powr,1);
    digitalWrite(off,0);
    if(cooler_heater){
      digitalWrite(heatPin,1);
      digitalWrite(coolPin,0);
    }
    else{
      digitalWrite(heatPin,0);
      digitalWrite(coolPin,1);
    }
    }
  else{
    digitalWrite(powr,0);
    digitalWrite(off,1);
    digitalWrite(heatPin,0);
    digitalWrite(coolPin,0);
     
    
  
  }
  
  if(fanspeed_final==0){
    analogWrite(fanblower,0);
  }
  else{
    if (fanspeed_final==3)
      analogWrite(fanblower,255);
    else if (fanspeed_final==2)
      analogWrite(fanblower,168);
    else if (fanspeed_final==1)
      analogWrite(fanblower,84);
    else
      analogWrite(fanblower,0); 
  }
  
  
  
  
}

float read_temp(int pin){
  int data = analogRead(pin);
  float voltage = data * 5.0;
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100 ;
  return temperatureC;
}

int read_sunload(int pin){
  int data = analogRead(pin);
  int val;
  if (data>=54 && data<830)
    val=0;
  else if (data>=830 && data<950)
    val=1;
  else if (data>=950 && data<974)
    val=2;
  else
    val=0;
  return val;
}

int read_automan(int pin){
  int data = digitalRead(pin);
  return data;
}

int fanspeed(int pin){
  int data=analogRead(pin);
  int val;
  if (data>=0 && data<256)
    val=0;
  else if (data>=256 && data<512)
    val=1;
  else if (data>=512 && data<768)
    val=2;
  else if (data>=768 && data<1024)
    val=3;
  else
    val=0;
  return val;
}

int set_temp(int pin){
  int data=analogRead(pin);
  data=(data - 0) * (32 - 18) / (1024 - 0) + 18;
return data;

}


