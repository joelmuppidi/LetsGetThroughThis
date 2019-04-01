// HVAC PROTOTYPE - Proof of Concept
// with Ambient temperature and Sun-load 
// Automatic and Manual function implemented

#define coolPin PB3
#define heatPin PB2
#define powr PB4
#define off PB5
#define ac_intensity PB1
#define fanblower PD6
#define ambient_temp_sensor_pin PC0
#define sunload_sensor_pin PC1
#define automan_pin PD2
#define fanspeed_pin PC2
#define set_temp_pin PC3


#include <avr/io.h>
#include <util/delay.h>
///////////////////////////////////////////////
////////Variables Defined for pins and pin values


int ambient_temp;
int sunload_value;
int automan_value;
int fanspeed_value_m;


int set_temp_value;
int cooler_heater;
int set_ac_intensity;
int fanspeed_value_a;
int power_ac;
int fanspeed_final;


/// Function Definitions //
///////////////////////////////////////////////////////////////


void set_pwm(uint8_t pin, int val){
  TCCR0A|=_BV(WGM01);
  TCCR0A|=_BV(WGM00);
  TCCR0B|=_BV(CS01);
  TCCR0B|=_BV(CS00);
  TIMSK0|=_BV(TOIE0);
  
  TCCR0A=(1<<COM0A1);
    
 
  OCR0A = val;

}

void adc_init(){
  //AREF = AVcc;
  ADMUX = (1<<REFS0);
  // ADC Enable and prescalar of 128
  // 16000000/128 = 128000
  ADCSRA = (1<<ADEN)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
}

uint16_t adc_read(uint8_t ch){
  // select the corrosponding channel 0~7
  // ANDing with '7' will always keep the value of 'ch' under 7
  ch &=0b00000111;
  ADMUX = (ADMUX & 0xF8)|ch;
  // start single conversion
  // writer '1' to ADSC
  ADCSRA |= (1<<ADSC);
  // wait for the conversion to complete
  // ADSC becomes '0' again
  // till then run loop continously
  while(ADCSRA & (1<<ADSC));
  return (ADC);
}

float read_temp(int pin){
  uint16_t data = adc_read(pin);
  float voltage = data * 5.0;     //reading temperature from the sensor
  voltage /= 1024.0;
  float temperatureC = (voltage - 0.5) * 100 ;
  return temperatureC;
}

int read_sunload(int pin){
  uint16_t data = adc_read(pin);
  int val;
  if (data>=54 && data<830)      // reading sunload data
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
  int data = (PIND&(1<<automan_pin));   //Auto/Manual mode selection
  return data/4;
}

int fanspeed(int pin){
  uint16_t data = adc_read(pin);
  int val;
  if (data>=0 && data<256)
    val=0;
  else if (data>=256 && data<512) //Fanspeed Selection for Manual mode
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
  uint16_t data = adc_read(pin);
  data=(data - 0) * (32 - 18) / (1024 - 0) + 18; //Set temperature in the cabin
return data;

}
////////////////////////////////////////////////////////////


int main(void) 
{
  
  adc_init(); 
  DDRB = (1<<ac_intensity)|(1<<heatPin)|(1<<coolPin)|(1<<powr)|(1<<off); // setting output ports
  DDRD |= (1<<fanblower);
 
  
while(1)
{
  ambient_temp=(int)read_temp(ambient_temp_sensor_pin);
  sunload_value=read_sunload(sunload_sensor_pin);
  automan_value=read_automan(automan_pin);
  fanspeed_value_m=fanspeed(fanspeed_pin);               //reading data
  set_temp_value=set_temp(set_temp_pin);
  
  switch(automan_value){
    
    case 1:
        if (abs(ambient_temp-set_temp_value)<1){ //fan only
      		power_ac=0;
          	fanspeed_value_a=1;
         }
    	else if(ambient_temp>set_temp_value){    //cooler
      		power_ac=1;
      		cooler_heater=0;
          if(abs(ambient_temp-set_temp_value)<2){ //cooler mode - low temp difference
            fanspeed_value_a=1;
            if(sunload_value<2){
        	  set_ac_intensity=0;
          	}
          	else{
        	  set_ac_intensity=1;
            }
          }
          else if (abs(ambient_temp-set_temp_value)<4){ //cooler mode - medium temp difference
            fanspeed_value_a=2;
            if(sunload_value<2){
        	  set_ac_intensity=0;
          	}
          	else{
        	  set_ac_intensity=1;
            }
          }
          else{                         				//cooler mode - high temp difference
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
          if(abs(ambient_temp-set_temp_value)<2){ //heater mode - low temp difference
            fanspeed_value_a=1;
            if(sunload_value<2){
        	  set_ac_intensity=1;
          	}
          	else{
        	  set_ac_intensity=0;
            }
          }
          else if (abs(ambient_temp-set_temp_value)<4){ //heater mode - medium temp difference
            fanspeed_value_a=2;
            if(sunload_value<2){
        	  set_ac_intensity=1;
          	}
          	else{
        	  set_ac_intensity=0;
            }
          }
          else{                         				//heater mode - high temp difference
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
    
    default:
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
          
    }
  

  if(power_ac){
    
    PORTB |= _BV(powr); //on
    PORTB &= ~_BV(off); //off
    if(cooler_heater){
      
      PORTB |= _BV(heatPin);
      PORTB &= ~_BV(coolPin);
    }
    else{
      PORTB &= ~_BV(heatPin);
      PORTB |= _BV(coolPin);
    }
    }
  else{
    
    PORTB &= ~_BV(powr);
    PORTB |= _BV(off);
    PORTB &= ~_BV(heatPin);
    PORTB &= ~_BV(coolPin);
     
    
  
  }
  
  if(fanspeed_final==0){
    set_pwm(fanblower,0);
  }
  else{
    if (fanspeed_final==3)
      set_pwm(fanblower,255);
    else if (fanspeed_final==2)
      set_pwm(fanblower,168);
    else if (fanspeed_final==1)
      set_pwm(fanblower,84);
    else
     set_pwm(fanblower,0);
      
  }
  
  if(set_ac_intensity)
    PORTB |= _BV(ac_intensity);
  else
    PORTB &= ~_BV(ac_intensity);

}
  
}

