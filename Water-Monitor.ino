
#define relayOn LOW
#define relayOff HIGH

boolean freshwater_fill_flag = false; //freshwater is topping off flag    `
boolean saltwater_fill_flag = false;
boolean sump_fill_flag = false; //sump needs topping off flag
boolean water_change_flag = false; //water change in process
boolean add_salt_flag = false; //add salt request sent to user..wait for flag to be cleared
boolean mix_saltwater_flag = false; //saltwater mixing in process
boolean drain_sump_flag = false; //sump draining in process
boolean fill_sump_from_reservoir_flag = false; //sump filling in process
boolean dosing_pump_one_flag = false;//dosing in process
int wc_stage = 0;
const int freshwater_high_float_switch = 2;
const int freshwater_fill_float_switch = 3;
const int freshwater_low_float_switch = 4;
const int saltwater_high_float_switch = 5;
const int saltwater_low_float_switch = 6;
const int sump_high_float_switch = 7;
const int sump_fill_float_switch = 8;
const int sump_low_float_switch = 9;
const int salt_added_button = 10;
const int fresh_to_sump_pump = 12;
//const int fresh_to_salt_pump = 13;
const int dosing_one = 13;
const int sump_to_drain_pump = 14;
const int salt_to_sump_pump = 15;
const int freshwater_relay_valve = 19;
const int saltwater_mixer = 17;
unsigned long mixTime;
unsigned long sump_fill;
unsigned long dosing_pump_one_time;




void setup()
{
  
  
  Serial.begin(115200);
  //set D2 through D10 as input with weak pullup enabled
  for(int i = 2; i<11; i++)
  {
    pinMode(i,INPUT);
    digitalWrite(i,HIGH);
  }
  //set D11 through D16 as OUTPUTS and initialize to logic high
  //which turns the relay off...logic low turns the relay on
  for(int i = 12; i<20; i++)
  {
    pinMode(i,OUTPUT);
    digitalWrite(i, relayOff);
  }
  dosing_pump_one_time = millis() + 3600000UL;
  
}

void loop()
{
  freshwater_level();
  sump_level();
  //water_change();
  dosing_pump_one();
}



void freshwater_level()
{
  //if freshwater fill float switch is logic low and if freshwater high 
  //float switch is logic low and if freshwater filling flag is not set 
  //then set it and turn the water filter on
  //boolean fwf_sw = digitalRead(freshwater_fill_float_switch);
  //boolean fwh_sw = digitalRead(freshwater_high_float_switch);

  if(digitalRead(freshwater_fill_float_switch) == false && 
     freshwater_fill_flag == false && 
     digitalRead(freshwater_high_float_switch) == false)
     {
       freshwater_fill_flag = true;
       digitalWrite(freshwater_relay_valve, relayOn); //turns the water on
       Serial.println("fill flag = true");
       Serial.println("freshwater relay = on");
     }
  
  //if freshwater high float switch is logic high and if freshwater
  //filling flag is set, then freshwater filling flag is false and turn the 
  //water filter off
  
  if(digitalRead(freshwater_high_float_switch) == true && 
     freshwater_fill_flag == true)
  {
    freshwater_fill_flag = false;
    digitalWrite(freshwater_relay_valve, relayOff); //turns the water off
    Serial.println("fill flag = false");
    Serial.println("relay is off");
  }
  
}

void sump_level()
{
  //if sump fill float switch is logic low and if sump high 
  //float switch is logic low and if sump filling flag is not set 
  //then set it and turn the freshwater topoff pump on.  
  //However, if the freshwater low float switch is logic low, 
  //then shut the pump off until the freshwater is replenished 
  //to avoid burning it up
  
  if(digitalRead(freshwater_low_float_switch) == false && 
           sump_fill_flag == true)
          {
            digitalWrite(fresh_to_sump_pump, relayOff);
            sump_fill_flag = false;
          }
          

  if(digitalRead(sump_fill_float_switch) == false && 
     sump_fill_flag == false && 
     digitalRead(sump_high_float_switch) == false &&
     digitalRead(freshwater_low_float_switch) == true && 
     drain_sump_flag == false) //drain sump flag suppresses this function if
                               //the sump is low as a result of a water change
                               //that is in progress
     {
       sump_fill = millis() + 60000UL;
       sump_fill_flag = true;
       digitalWrite(fresh_to_sump_pump, relayOn); //turns the water on
     }
  
  //if sump high float switch is logic high or if sump
  //filling flag is set and 2 minute sump fill time has expired, 
  //then filling flag is false and turn the 
  //sump freshwater topoff pump off
  
//  if(digitalRead(sump_high_float_switch) == true && 
//     sump_fill_flag == true)
  if(sump_fill_flag == true && (timer(sump_fill) || 
     digitalRead(sump_high_float_switch) == true))
  {
    sump_fill_flag = false;
    digitalWrite(fresh_to_sump_pump, relayOff); //turns the water off
  }
  
}


//void water_change()
//{
//  
//  if(water_change_flag == true)
//  {
//    switch(wc_stage)
//    {
//      case 0: //top off freshwater reservoir
//       if(freshwater_fill_flag == false && 
//       digitalRead(freshwater_high_float_switch) == false)
//        {
//          freshwater_fill_flag = true;
//          digitalWrite(freshwater_relay_valve, relayOn); //turns the water on
//        }
//        
//        if(digitalRead(freshwater_high_float_switch) == true)
//        {
//          wc_stage = 1;
//        }
//        break;
//        
//      case 1: //fill saltwater reservoir
//        if(digitalRead(freshwater_low_float_switch) == false && 
//         saltwater_fill_flag == true)
//        {
//          digitalWrite(fresh_to_salt_pump, relayOff);
//          saltwater_fill_flag = false;
//          break;
//        }
//        if(digitalRead(saltwater_high_float_switch) == false &&
//        saltwater_fill_flag == false && 
//        digitalRead(freshwater_low_float_switch) == true)
//        {
//          saltwater_fill_flag = true;
//          digitalWrite(fresh_to_salt_pump, relayOn);
//        }
//        if(digitalRead(saltwater_high_float_switch) == true)
//        {
//          saltwater_fill_flag = false;
//          digitalWrite(fresh_to_salt_pump, relayOff);
//          wc_stage = 2;
//        }
//        break;
//        
//      case 2: //add salt
//        if(add_salt_flag == false)
//        { 
//          Serial.println("Add Salt = True");
//          add_salt_flag = true;
//        }
//        if(digitalRead(salt_added_button) == false) //push when salt is added
//        {
//          add_salt_flag = false;
//          wc_stage = 3;
//        }
//        break;
//        
//      case 3: //mix saltwater for 10 Hours
//        if(mix_saltwater_flag == false)
//        {
//          mix_saltwater_flag = true;
//          mixTime = millis() + 36000000UL;
//          digitalWrite(saltwater_mixer, relayOn);
//        }
//        
//        if(timer(mixTime))
//        {
//          mix_saltwater_flag = false;
//          digitalWrite(saltwater_mixer, relayOff);
//          wc_stage = 4;
//        }
//        break;
//        
//      case 4: //top off sump
//        if(digitalRead(sump_high_float_switch) == false && 
//         sump_fill_flag == false && 
//         digitalRead(freshwater_low_float_switch) == true)
//        {
//          sump_fill_flag = true;
//          digitalWrite(fresh_to_sump_pump, relayOn);
//        }
//        
//        if(digitalRead(sump_high_float_switch) == true)
//        {
//          wc_stage = 5;
//        }
//        break;
//        
//      case 5: //drain sump
//        if(drain_sump_flag == false) 
//        {
//          drain_sump_flag = true;
//          digitalWrite(sump_to_drain_pump, relayOn);
//        }
//        if(digitalRead(sump_low_float_switch) == false)
//        {
//          digitalWrite(sump_to_drain_pump, relayOff);
//          wc_stage = 6;
//        }
//        break;
//        
//      case 6: //fill sump but don't let the saltwater reservoir run dry
//        if(fill_sump_from_reservoir_flag == true &&
//        digitalRead(saltwater_low_float_switch == false))
//        {
//          fill_sump_from_reservoir_flag = false;
//          digitalWrite(salt_to_sump_pump, relayOff);
//        }
//        
//        if(fill_sump_from_reservoir_flag == false && 
//        digitalRead(saltwater_low_float_switch == true))
//        {
//          fill_sump_from_reservoir_flag = true;
//          digitalWrite(salt_to_sump_pump, relayOn);
//        }
//        
//        if(digitalRead(sump_high_float_switch) == true)
//        {
//          digitalWrite(salt_to_sump_pump, relayOff);
//          drain_sump_flag = false;
//          wc_stage = 7;
//        }
//        break;
//        
//      case 7: //clean up
//        //send message to user notifying them of a completed water change
//        wc_stage = 0;
//        water_change_flag = false;
//        
//    }
//  }
//  
//  
//}

//run the dosing pump for 12.677 seconds every hour.  This should dose 40ml of 
//each solution per day.
void dosing_pump_one()
{
  if(dosing_pump_one_flag == false && timer(dosing_pump_one_time))
  {
    dosing_pump_one_flag = true;
    digitalWrite(dosing_one,relayOn);
    dosing_pump_one_time = millis() + 12677UL; //12.677 seconds
  }
  
  if(dosing_pump_one_flag == true && timer(dosing_pump_one_time))
  {
    dosing_pump_one_flag = false;
    digitalWrite(dosing_one, relayOff);
    dosing_pump_one_time = millis() + 3600000UL;
  }
}

/*void check_for_instructions()
{
  if(Serial.available())
  {
    char c = Serial.read();
    switch c
  }
  
  return;
}*/

//keep track of time and handle millis() rollover
boolean timer(unsigned long timeout)
  {
    return (long)(millis() - timeout) >= 0;
  }
