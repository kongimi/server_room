#ifndef MYCODE_H
#define MYCODE_H

byte RelayAirCon1 = 14;  // กำหนดขา Relay เปิด/ปิด แอร์ตัวที่1
byte RelayAirCon2 = 12;  // กำหนดขา Relay เปิด/ปิด แอร์ตัวที่2

String string_time(int my_dateTime){
    String myResult;
    if(my_dateTime < 10){
        myResult = "0"+ String(my_dateTime);
        return myResult;
    }else{
        return String(my_dateTime);
    }
}

String PrintTime(){
    
    String myTime,my_hour,my_minute,my_month,my_date,my_sec;
    my_month = string_time(DateTime.month);
    my_date = string_time(DateTime.day);
    my_hour = string_time(DateTime.hour);
    my_minute = string_time(DateTime.minute);
    my_sec = string_time(DateTime.second);
    myTime = my_date + "-" + my_month + "-" + (String)DateTime.year + " " + my_hour + ":" + my_minute + ":" +  my_sec + " ";
    return myTime;
}

void press_remote(int i){
  if(i==0){
    digitalWrite(RelayAirCon1,HIGH);
    delay(200);
    digitalWrite(RelayAirCon1,LOW);
    Serial.println("Did AirCon1.");
  }else{
    digitalWrite(RelayAirCon2,HIGH);
    delay(200);
    digitalWrite(RelayAirCon2,LOW);
    Serial.println("Did AirCon2.");
  }
}
#endif
