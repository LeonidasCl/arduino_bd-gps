/*
  Blink
  Turns on an LED on for one second, then off for one second, repeatedly.

  Most Arduinos have an on-board LED you can control. On the Uno and
  Leonardo, it is attached to digital pin 13. If you're unsure what
  pin the on-board LED is connected to on your Arduino model, check
  the documentation at http://arduino.cc

  This example code is in the public domain.

  modified 8 May 2014
  by Scott Fitzgerald
 */
    float lon0=11019.642000;
    double lat0=2517.0217;
    char testch[14]={'2','5','1','7','.','0','2','1','7','3','2','5','5'};
// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin 13 as an output.
  //pinMode(13, OUTPUT);
  Serial.begin(115200);
  delay(1000);
  String str="";
    for(int i=0;testch[i]!='\0';i++){
    str += testch[i];
    }
  Serial.println(str);

  float FloatEx = str.toFloat();
  
  Serial.println(FloatEx,6);
  
  /*long test=102340000;
  Serial.println(test);
    // double lon=transTodddd(lon0);
    delay(1000);
    double lat=transTodddd(lat0);
    delay(1000);
    //Serial.println(lon,6);
    Serial.println(lat,6);*/
    
    
    
}

// the loop function runs over and over again forever
void loop() {

}

double transTodddd(double x){//example x=2517.0217
    Serial.println(x,6);
    int x1= (int) (x/100);//多少度25
    Serial.println(x1);
    long x2= x*10000;// 25170217
    Serial.println(x2);
    long x3=x2%1000000;//170217
    Serial.println(x3);
    double x4=((double)x3)/10000;//17.0217
    Serial.println(x4,6);
    double x5=x4/60;//十进制换算为多少度--0.283695
    Serial.println(x5,6);
    double x6=x1+x5;//25.283695
    Serial.println(x6,6);
    return x6;
}
