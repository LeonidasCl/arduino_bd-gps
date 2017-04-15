#include <SoftwareSerial.h>
//#define GpsSerial  Serial
#define  DebugSerial Serial
#include <aJSON.h>

#include <Math.h>
const static double a = 6378245.0;  
const static double ee = 0.00669342162296594323; 
SoftwareSerial GpsSerial(10, 11); 
int L = 13; //LED指示灯引脚
String DEVICEID="1426"; //设备编号
String  APIKEY="05c0a2ff6"; //密码
unsigned long lastCheckInTime = 0; //记录上次报到时间
const unsigned long postingInterval = 20000; // 每隔60秒向服务器报到一次
String inputString = "";
char cid[5]={'U','1','2','8','0'};
char says[8]={'E','X','C','I','T','E','D','!'};
String strtmp="";
boolean stringComplete = false;
boolean CONNECT = true; 
char* parseJson(char *jsonString);
struct
{
	char GPS_Buffer[80];
	bool isGetData;		//是否获取到GPS数据
	bool isParseData;	//是否解析完成
	char UTCTime[11];		//UTC时间
	char latitude[11];		//纬度
	char N_S[2];		//N/S
	char longitude[12];		//经度
	char E_W[2];		//E/W
	bool isUsefull;		//定位信息是否有效
} Save_Data;

const unsigned int gpsRxBufferLength = 600;
char gpsRxBuffer[gpsRxBufferLength];
unsigned int ii = 0;


void setup()	//初始化内容
{
	GpsSerial.begin(9600);			//定义波特率9600，和GPS模块输出的波特率一致
	DebugSerial.begin(115200);
	DebugSerial.println("initialing");
	DebugSerial.println("Wating...");

	Save_Data.isGetData = false;
	Save_Data.isParseData = false;
	Save_Data.isUsefull = false;

        delay(15000);
}

void loop()		//主循环
{
	gpsRead();	//获取GPS数据
	parseGpsBuffer();//解析GPS数据
	printGpsBuffer();//输出解析后的数据
}

void errorLog(int num)
{
	DebugSerial.print("ERROR");
	DebugSerial.println(num);
	while (1)
	{
		digitalWrite(L, HIGH);
		delay(300);
		digitalWrite(L, LOW);
		delay(300);
	}
}

void printGpsBuffer()
{
	if (Save_Data.isParseData)
	{
		Save_Data.isParseData = false;
		
		DebugSerial.print("Save_Data.UTCTime = ");
		DebugSerial.println(Save_Data.UTCTime);

		if(Save_Data.isUsefull)
		{
			Save_Data.isUsefull = false;
			DebugSerial.print("Save_Data.latitude = ");
			DebugSerial.println(Save_Data.latitude);
			DebugSerial.print("Save_Data.N_S = ");
			DebugSerial.println(Save_Data.N_S);
			DebugSerial.print("Save_Data.longitude = ");
			DebugSerial.println(Save_Data.longitude);
			DebugSerial.print("Save_Data.E_W = ");
			DebugSerial.println(Save_Data.E_W);
                        checkIn();
		}
		else
		{
			DebugSerial.println("not usefull!");
                        //temp check
                        //checkIn();
		}
		
	}
}

void parseGpsBuffer()
{
	char *subString;
	char *subStringNext;
	if (Save_Data.isGetData)
	{
		Save_Data.isGetData = false;
		DebugSerial.println("**************");
		DebugSerial.println(Save_Data.GPS_Buffer);

		
		for (int i = 0 ; i <= 6 ; i++)
		{
			if (i == 0)
			{
				if ((subString = strstr(Save_Data.GPS_Buffer, ",")) == NULL)
					errorLog(1);	//解析错误
			}
			else
			{
				subString++;
				if ((subStringNext = strstr(subString, ",")) != NULL)
				{
					char usefullBuffer[2]; 
					switch(i)
					{
						case 1:memcpy(Save_Data.UTCTime, subString, subStringNext - subString);break;	//获取UTC时间
						case 2:memcpy(usefullBuffer, subString, subStringNext - subString);break;	//获取UTC时间
						case 3:memcpy(Save_Data.latitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 4:memcpy(Save_Data.N_S, subString, subStringNext - subString);break;	//获取N/S
						case 5:memcpy(Save_Data.longitude, subString, subStringNext - subString);break;	//获取纬度信息
						case 6:memcpy(Save_Data.E_W, subString, subStringNext - subString);break;	//获取E/W

						default:break;
					}

					subString = subStringNext;
					Save_Data.isParseData = true;
					if(usefullBuffer[0] == 'A')
						Save_Data.isUsefull = true;
					else if(usefullBuffer[0] == 'V')
						Save_Data.isUsefull = false;

				}
				else
				{
					errorLog(2);	//解析错误
				}
			}


		}
	}
}


void gpsRead() {
	while (GpsSerial.available())
	{
		gpsRxBuffer[ii++] = GpsSerial.read();
		if (ii == gpsRxBufferLength)clrGpsRxBuffer();
	}

	char* GPS_BufferHead;
	char* GPS_BufferTail;
	if ((GPS_BufferHead = strstr(gpsRxBuffer, "$GPRMC,")) != NULL || (GPS_BufferHead = strstr(gpsRxBuffer, "$GNRMC,")) != NULL )
	{
		if (((GPS_BufferTail = strstr(GPS_BufferHead, "\r\n")) != NULL) && (GPS_BufferTail > GPS_BufferHead))
		{
			memcpy(Save_Data.GPS_Buffer, GPS_BufferHead, GPS_BufferTail - GPS_BufferHead);
			Save_Data.isGetData = true;

			clrGpsRxBuffer();
		}
	}
}

void clrGpsRxBuffer(void)
{
	memset(gpsRxBuffer, 0, gpsRxBufferLength);      //清空
	ii = 0;
}

void checkIn() {
  /*if (!CONNECT) {
    DebugSerial.print("+++");
    delay(500);  
    DebugSerial.print("\r\n"); 
    delay(1000);
    DebugSerial.print("AT+RST\r\n"); 
    delay(6000);
    CONNECT=true;
    lastCheckInTime=0;
  }
  else{*/
  //DebugSerial.print("ready to restart\r\n");
    delay(500);
    DebugSerial.print("+++\r\n");
    delay(2000);  
    DebugSerial.print("AT+CIPSEND\r\n");

    if(millis() - lastCheckInTime > postingInterval || lastCheckInTime==0){
      DebugSerial.print("{\"M\":\"checkin\",\"ID\":\"");
      DebugSerial.print(DEVICEID);
      DebugSerial.print("\",\"K\":\"");
      DebugSerial.print(APIKEY);
      DebugSerial.print("\"}\r\n");
      lastCheckInTime = millis(); 
      //delay(100);
      //sayToClient(cid,says);
      //delay(1000);
    }
     
     delay(200);
     
         serialEvent();
    if (stringComplete) {
      inputString.trim();
      //Serial.println(inputString);
      if(inputString=="CLOSED"){
        DebugSerial.println("connect closed!");
        CONNECT=false;        
      }else{
        int len = inputString.length()+1;    
        if(inputString.startsWith("{") && inputString.endsWith("}")){
          char jsonString[len];
          inputString.toCharArray(jsonString,len);
          aJsonObject *msg = aJson.parse(jsonString);
          processMessage(msg);
          aJson.deleteItem(msg);          
        }
      }      
      // clear the string:
      inputString = "";
      stringComplete = false;    
  }
  
    strtmp="";
    for(int j=0;j<9;j++){
      strtmp += Save_Data.latitude[j];
    }
    double lat0=strtmp.toFloat();
    strtmp="";
    for(int i=0;Save_Data.longitude[i]!='\0';i++){
    strtmp += Save_Data.longitude[i];
    }
    double lon0=strtmp.toFloat();
    strtmp="";
    
    double lon=transTodddd(lon0);
    double lat=transTodddd(lat0);
    double tlat=Wgs_Gcj_lat(lat,lon);
    double tlon=Wgs_Gcj_lon(lat,lon);
    delay(1000);
    DebugSerial.print("{\"M\":\"update\",\"ID\":\"");
    DebugSerial.print(DEVICEID);
    DebugSerial.print("\",\"V\":{\"1425\":\"");
    //DebugSerial.print(Save_Data.longitude);
    DebugSerial.print(tlon,6);
    DebugSerial.print(",");
    //DebugSerial.print(Save_Data.latitude);
    DebugSerial.print(tlat,6);
    DebugSerial.print("\"}}\r\n");
    
    delay(200);    
    DebugSerial.print("{\"M\":\"say\",\"ID\":\"");
    DebugSerial.print("D1442");
    DebugSerial.print("\",\"C\":\"");
    DebugSerial.print(tlon,6);
    DebugSerial.print("-");
    DebugSerial.print(tlat,6);
    DebugSerial.print("\"}\r\n");
    
    delay(500);
    DebugSerial.print("+++\r\n");
    delay(100); 
}
void processMessage(aJsonObject *msg){
  aJsonObject* method = aJson.getObjectItem(msg, "M");
  aJsonObject* content = aJson.getObjectItem(msg, "C");     
  aJsonObject* client_id = aJson.getObjectItem(msg, "ID");  
  //char* st = aJson.print(msg);
  if (!method) {
    return;
  }
    //Serial.println(st); 
    //free(st);
    String M=method->valuestring;
    String C=content->valuestring;
    String F_C_ID=client_id->valuestring;
    if(M=="say"){
      if(C=="play"){
        //digitalWrite(LED, HIGH);
        sayToClient(F_C_ID,"LED on!");    
      }
      if(C=="stop"){
        //digitalWrite(LED, LOW);
        sayToClient(F_C_ID,"LED off!");    
      }
    }
}
void sayToClient(String client_id, String content){
  DebugSerial.print("{\"M\":\"say\",\"ID\":\"");
  DebugSerial.print(client_id);
  DebugSerial.print("\",\"C\":\"");
  DebugSerial.print(content);
  DebugSerial.print("\"}\r\n");
  lastCheckInTime = millis();
}
void serialEvent() {
  while (DebugSerial.available()) {
    char inChar = (char)DebugSerial.read();
    inputString += inChar;
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

double Wgs_Gcj_lat(double lat,double lon){  

        double dLat = transformLat(lon - 105.0, lat - 35.0);  
        double dLon = transformLon(lon - 105.0, lat - 35.0);  
        double radLat = lat / 180.0 * PI;  
        double magic = sin(radLat);  
        magic = 1 - ee * magic * magic;  
        double sqrtMagic = sqrt(magic);  
        dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * PI);  
        dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * PI);  
        double mgLat = lat + dLat;  
        double mgLon = lon + dLon;  
        return mgLat;  
}

double Wgs_Gcj_lon(double lat,double lon) {  
  
        double dLat = transformLat(lon - 105.0, lat - 35.0);  
        double dLon = transformLon(lon - 105.0, lat - 35.0);  
        double radLat = lat / 180.0 * PI;  
        double magic = sin(radLat);  
        magic = 1 - ee * magic * magic;  
        double sqrtMagic = sqrt(magic);  
        dLat = (dLat * 180.0) / ((a * (1 - ee)) / (magic * sqrtMagic) * PI);  
        dLon = (dLon * 180.0) / (a / sqrtMagic * cos(radLat) * PI);  
        double mgLat = lat + dLat;  
        double mgLon = lon + dLon;  
        return mgLon;  
}

double transformLat(double x, double y) {  
        double ret = -100.0 + 2.0 * x + 3.0 * y + 0.2 * y * y + 0.1 * x * y  
                + 0.2 * sqrt(abs(x));  
        ret += (20.0 * sin(6.0 * x * PI) + 20.0 * sin(2.0 * x * PI)) * 2.0 / 3.0;  
        ret += (20.0 * sin(y * PI) + 40.0 * sin(y / 3.0 * PI)) * 2.0 / 3.0;  
        ret += (160.0 * sin(y / 12.0 * PI) + 320 * sin(y * PI / 30.0)) * 2.0 / 3.0;  
        return ret;  
}
  
double transformLon(double x, double y) {  
        double ret = 300.0 + x + 2.0 * y + 0.1 * x * x + 0.1 * x * y + 0.1  
                * sqrt(abs(x));  
        ret += (20.0 * sin(6.0 * x * PI) + 20.0 * sin(2.0 * x * PI)) * 2.0 / 3.0;  
        ret += (20.0 * sin(x * PI) + 40.0 * sin(x / 3.0 * PI)) * 2.0 / 3.0;  
        ret += (150.0 * sin(x / 12.0 * PI) + 300.0 * sin(x / 30.0  
                * PI)) * 2.0 / 3.0;  
        return ret;  
}

double transTodddd(double x){//example x=2517.0217
    int x1= (int) (x/100);//多少度25
    long x2= (x*10000);// 25170217
    long x3=x2%1000000;//170217
    double x4=((double)x3)/10000;//17.0217
    double x5=x4/60;//十进制换算为多少度--0.283695
    return (double)x1+x5;//25.283695
}



