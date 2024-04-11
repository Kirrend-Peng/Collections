#include<Servo.h>
const int PIRsensor = 10;
const int trig = 11;
const int echo = 12;
const int infrared = 9;
int level = 0;
Servo myservo;
Servo push;
int sensorPIR ;
unsigned long start;
float mindistance = 15;
typedef struct GOCAR{
  float gotime;
  int toward; 
};
GOCAR gocar;

typedef struct DIN{
  bool hasobstacle = false;
  float distance;
} ; // DIN

void carstop() {
      digitalWrite(0,LOW);         
      digitalWrite(1,LOW);        
  
      digitalWrite(2,LOW);
      digitalWrite(3,LOW);
  
      digitalWrite(4,LOW);
      digitalWrite(5,LOW);
  
      digitalWrite(6,LOW);
      digitalWrite(7,LOW);  // car stop
}

void cargo() {
      digitalWrite(0,HIGH);         
      digitalWrite(1,LOW);        
  
      digitalWrite(2,HIGH);
      digitalWrite(3,LOW);
  
      digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
  
      digitalWrite(6,HIGH);
      digitalWrite(7,LOW); 
}

void CTleft() {
      digitalWrite(0,LOW);         
      digitalWrite(1,HIGH);        
  
      digitalWrite(2,LOW);
      digitalWrite(3,HIGH);
  
      digitalWrite(4,HIGH);
      digitalWrite(5,LOW);
  
      digitalWrite(6,HIGH);
      digitalWrite(7,LOW);  // car go

}


void CTright() {
      digitalWrite(0,HIGH);         
      digitalWrite(1,LOW);        
  
      digitalWrite(2,HIGH);
      digitalWrite(3,LOW);
  
      digitalWrite(4,LOW);
      digitalWrite(5,HIGH);
  
      digitalWrite(6,LOW);
      digitalWrite(7,HIGH);  

}

void cargoback() {
        digitalWrite(0,LOW);         
      digitalWrite(1,HIGH);        
  
      digitalWrite(2,LOW);
      digitalWrite(3,HIGH);
  
      digitalWrite(4,LOW);
      digitalWrite(5,HIGH);
  
      digitalWrite(6,LOW);
      digitalWrite(7,HIGH);  

}

int infrared_test( int coin ) {
  int newcoin = coin;
  int testinfrared  = digitalRead(9);  
   if ( testinfrared == LOW ) {
      newcoin = coin+1;
      level = 4;
      delay(1000);
   } // if
   
   return newcoin;
}

int throwmode( int coin ) {
    int newcoin2 = coin;
    for ( int oldcoin = newcoin2 , start = millis() ; millis() - start <= 5000;  ) { // if 5秒內未投入硬幣即結束迴圈 )
      newcoin2 = infrared_test(coin);
      if ( oldcoin < newcoin2 ) {
        start = millis();   // 因為又投了coin,so reset "start = now time" . 
        oldcoin = newcoin2;
      } // if
    } // for
    
    return newcoin2;
}

float Sounddistance( int trig,int echo ) {
  float duration;
  digitalWrite(trig, HIGH);
  delayMicroseconds(1000);
  digitalWrite(trig, LOW);
  duration = pulseIn(echo, HIGH);
  return (duration/2)/29;  
} // Sound



void setup() {
 pinMode(0,OUTPUT);
 pinMode(1,OUTPUT);
 pinMode(2,OUTPUT);
 pinMode(3,OUTPUT);
 pinMode(4,OUTPUT);
 pinMode(5,OUTPUT);
 pinMode(6,OUTPUT);
 pinMode(7,OUTPUT);
 pinMode(infrared,INPUT);
  push.attach(13);
  myservo.attach(8);
  pinMode(trig,OUTPUT);
  pinMode(echo,INPUT);

  pinMode(PIRsensor,INPUT);
  delay(6000000000000000);
}

void loop() {
boolean PIRtrue = false;
boolean goback = false;
int coin = 0;
float x,y,limitdistance;
DIN *din;
din = new DIN[127];
boolean hasobstacle = false;
float distance = 0;  // soundsensor)
  if ( level == 0 )  {              //  detect forward;

      carstop(); // car stop
    for(int i = 10,x = 30-((30/90)*44) ; i <= 120 ; ++i,x = x-(30/90) ){  //  x = 44-((44/90)*40) --------->  -34 degrees is 0 degree;
       if ( i == 56 ) { 
         x = 0;
       } // if
       
      sensorPIR = digitalRead(PIRsensor);
      for (start = millis();sensorPIR == HIGH && coin == 0 ;  ) {
        if (millis()-start >= 20000 ) {
          sensorPIR = digitalRead(PIRsensor);
          if ( sensorPIR == HIGH ) {
            start = millis();
          }
        } // if
        if ( digitalRead(9) == LOW ) 
          coin = 1; 
        PIRtrue = true;
      } // for   PIR   
    
    myservo.write(i);// 使用write，傳入角度，從180度轉到0度
    delay(30);
    distance =  Sounddistance( trig,echo ) ;
    y = (sq(x)/(-22.5))+40;
    limitdistance = sqrt(sq(x)+sq(y));
    if ( distance >= 10 && distance <= limitdistance) {
       din[i].distance = distance;
       din[i].hasobstacle = true;   
       hasobstacle = true; 
       } // if
     delay(30);
  } // for
  
  if ( hasobstacle ) {
    level = 2;
  } // if
  else {
    level = 1;  
  } // else  
  
  if ( PIRtrue )
    level = 0;
    
  if (coin != 0 ) 
    level = 4;
  } // if level == 0
  
   else if ( level == 1 ) {                              // cargo
     cargo(); // car go   
     
      for(int i = 120,ufcd = 26,x = 30-(30/90)*26; i >=10 && !hasobstacle && coin == 0; i-=1,x = x -(30/90) ){  // ufcd:
         
      sensorPIR = digitalRead(PIRsensor);
      for (start = millis();sensorPIR == HIGH && coin == 0 ;  ) {
        carstop();
        if (millis()-start >= 20000 ) {
          sensorPIR = digitalRead(PIRsensor);
          if ( sensorPIR == HIGH ) {
            start = millis();
          }
        } // if
        if ( digitalRead(9) == LOW ) 
          coin = 1; 
        PIRtrue = true;
      } // for   PIR  
      
       if ( i == 56 ) {
         x = 0;
       } // if
       myservo.write(i); // 使用write，傳入角度，從0度轉到180度
       delay(30);
       distance =  Sounddistance( trig,echo ) ;
       y = (sq(x)/(-22.5))+40;
       limitdistance = sqrt(sq(x)+sq(y));
       if ( distance >= 10  && distance <= limitdistance ) {             // 近距離
           hasobstacle = true;    
         
       } // if
     
      if ( hasobstacle || coin != 0) {     
        carstop(); // car stop
        for(int k = i ; k >= 10 ; --k) {
            myservo.write(i);// 轉至 level 1 servo的初始角 )
            delay(10);
         } // for
      } // if
  }  // for
  
   for(int i = 10,x = 30-((30/90)*44) ; i <= 120 && !hasobstacle && coin == 0; i+=1,x = x-(30/90) ){
       if ( i == 56 ) 
         x = 0;
       
      sensorPIR = digitalRead(PIRsensor);
      for (start = millis();sensorPIR == HIGH && coin == 0 ;  ) {
        carstop();
        if (millis()-start >= 20000 ) {
          sensorPIR = digitalRead(PIRsensor);
          if ( sensorPIR == HIGH ) {
            start = millis();
          }
        } // if
        if ( digitalRead(9) == LOW ) 
          coin = 1; 
        PIRtrue = true;
      } // for   PIR  
      
    myservo.write(i);// 使用write，傳入角度，從180度轉到0度
    delay(30);
    distance =  Sounddistance( trig,echo ) ;
    y = (sq(x)/(-22.5))+40;
    limitdistance = sqrt(sq(x)+sq(y));
    if (  distance >= 10 && distance <= limitdistance) {
       if (  distance <= limitdistance ) {             // 近距離
           hasobstacle = true;    
       } //  if
    } // if
     
     if ( hasobstacle || coin != 0) {
         carstop() ; // car stop
         for(int k = i ; k >= 10 ; --k) {
         myservo.write(i);// 使用write，傳入角度，從180度轉到0度
         delay(10);
         } // for
      } // if
      
      
  } // for
  
    if ( hasobstacle )
      level = 0;
    else 
      level = 1;
    if ( PIRtrue )
      level = 0;
    if (coin != 0 ) 
      level = 4;  
   } // if  level == 1
   
   
 if ( level == 2 ) {                          // caculate
   int leftobdegree = 0; 
   int rightobdegree = 0;
   mindistance = 25;
   goback = false;
   for ( int i = 10; i <= 120 ;i++) {                    // 檢查servo在10~120度時，超音波感測器測到的物)
     if ( din[i].hasobstacle && din[i].distance <= mindistance ) {                // 是否有倒退的必要(是否不能迴轉)
          mindistance = din[i].distance;
          goback = true;
      } // if
     if ( i>= 46 && din[i].hasobstacle  && leftobdegree == 0)  // 我們的servo 0度在車體右方，越靠近56的角度越好)
       leftobdegree = i;
     else if ( i < 46 && din[i].hasobstacle )  
       rightobdegree = i;
    
   } // for
   
    if ( !goback ) {          //
       if ( leftobdegree != 0 && rightobdegree != 0 )  { 
         gocar.toward = 2;
         gocar.gotime = (rightobdegree+34) *(16250/90);
       } // if
       else if ( leftobdegree != 0 ) {
         gocar.toward = 3;
         gocar.gotime = (146-leftobdegree) *(11000/90);
       } // if
       else {
         gocar.toward = 2;
         gocar.gotime = (rightobdegree+34) *(16250/90);
       }
    }             // hello here
    else {
      gocar.toward = 4;
      gocar.gotime = (26-mindistance)*150 ;
    } // else   
       level = 3;
 } // if  (level == 2 )
  
 if ( level == 3 ) {          //  decide the car direction
    int level3start;
    if ( gocar.toward == 2 ) 
      CTleft();
    else if ( gocar.toward == 3 ) 
      CTright();
    else if ( gocar.toward == 4 )
      cargoback();
      for ( int level3start = millis() ; millis()-level3start < gocar.gotime && !sensorPIR ; ) {
        sensorPIR = digitalRead(PIRsensor);
        for ( start = millis();sensorPIR == HIGH && coin == 0 ;  ) {
          carstop();
          if (millis()-start >= 20000 ) {
          sensorPIR = digitalRead(PIRsensor);
          if ( sensorPIR == HIGH ) {
            start = millis();
          }
        } // if
        if ( digitalRead(9) == LOW ) 
          coin = 1; 
        PIRtrue = true;
        } // for   PIR  
      } // implement
    if ( !PIRtrue && coin == 0 )  {
      delay( gocar.gotime) ;
   }
   carstop();
   level = 0;
  for(int k = 120 ; k >= 10 ; --k) {
     myservo.write(k);// 使用write，傳入角度，從180度轉到0度
     delay(10);
  } // for
     
   if (coin != 0 )
       level = 4;
   
 }   // if    (level = 3)
 
if (level == 4 ){
  carstop();    
  if ( coin > 0 ) {
     push.writeMicroseconds(1475);
     delay(11000);
    --coin;
  }
   push.writeMicroseconds(1500);
     sensorPIR = digitalRead(PIRsensor);
      for (start = millis();sensorPIR == HIGH && coin == 0 ;  ) {
        if (millis()-start >= 20000 ) {
          sensorPIR = digitalRead(PIRsensor);
          if ( sensorPIR == HIGH ) {
            start = millis();
          }
        } // if
        if ( digitalRead(9) == LOW ) 
          coin = 1; 
        PIRtrue = true;
      } // for   PIR  
      
  level = 0;
  
  if ( coin != 0 )
    level = 4;
   //推物機功能)
   
} // level4

 delete[] din;
}
