/*
  ESP Slave 29.07.2021
*/
#define DEBUG          0
#define FTP            0

#define MasterID       1
#define ControlPanelID 2
#include <Ticker.h>

#include <ESP8266WiFi.h>                      // Библиотека для создания Wi-Fi подключения (клиент или точка доступа)
#include <ESP8266WebServer.h>                 // Библиотека для управления устройством по HTTP (например из браузера)
#include <FS.h>                               // Библиотека для работы с файловой системой
#if(FTP)
#include <ESP8266FtpServer.h>                 // Библиотека для работы с SPIFFS по FTP
#endif
#include <EEPROM.h>

Ticker RadData;
Ticker WriteData;
Ticker Debug;
Ticker StopDetectState;

uint8_t Command_Massive[20];
/*---------------------------------------------------------------------------------------------------------------*/
//массив данных панели управления получаемый по шине      
#define ID          0
#define Start       1
#define Prog        2
#define Temper      3
#define Spin        4
#define Status      5
#define FlagEnd     6
#define rez1        7
#define rez2        8
#define cTemp       9
#define sTemp      10
#define LevWat     11
#define NumProg    12
#define FlagDoor   13
#define CountProg  14
#define SpeedL     15
#define SpeedH     16
#define PauseL     17
#define PauseH     18
#define CRC        19

// file 0- del 1- read 2-write
#define READ_FILE      2
#define WRITE_FILE     1
#define DEL_FILE       0

int ProgNum             =-1;
unsigned int SpeedSpin  = 0;
unsigned int Tempr      = 0;
byte ButtonStart        = 0;
byte Endlocal           = 0;
byte StatusWash         = 0;
byte CountStart         = 0;
byte State              = 0;
byte modeWiFi           = 0;

#define DelayLan        50
/*---------------------------------------------------------------------------------------------------------------*/                                  
ESP8266WebServer HTTP(80);                      // Определяем объект и порт сервера для работы с HTTP
  #if (FTP)
  FtpServer ftpSrv;                             // Определяем объект для работы с модулем по FTP (для отладки HTML)
  #endif
String  ssidName = "OleSton"; String ssidPass = "3276832768";

bool handleFileRead(String); 
String getContentType(String); 
void writeDataMemm(int); 
void readDataMemm(int);
void handleGenericArgs();
void sendStatus ();
void StopDetectSt();
/*---------------------------------------------------------------------------------------*/

void setup()
{
      #if (DEBUG)
      pinMode(LED_BUILTIN, OUTPUT);
      #endif
      Serial.begin(9600);
      //Serial.println(F("\nStart!"));
      #if (DEBUG)
      Serial1.begin(115200); Serial1.println(F("\nDebug Start!"));
      #endif
  SPIFFS.begin();                                                       // Инициализируем работу с файловой системой     
  uint8_t i = 0;
  String tmpStr ="";
  modeWiFi = getFileWiFi("modeWiFi.txt", ssidName, READ_FILE).toInt();
  tmpStr   = getFileWiFi("ssidPass.txt", ssidPass, READ_FILE);
  tmpStr.trim();
  if (tmpStr != "") {ssidPass = tmpStr;} else {getFileWiFi("ssidPass.txt", "", DEL_FILE);}
  tmpStr = getFileWiFi("ssidName.txt", ssidName, READ_FILE);
  tmpStr.trim();
  if (tmpStr != "") {ssidName = tmpStr;} else {getFileWiFi("ssidName.txt", "", DEL_FILE);}
  #if (DEBUG)
  Serial1.print(F("\nSSID MODE: "));   Serial1.print(modeWiFi); Serial1.print(F("  SSID NAME: "));   
  Serial1.print(ssidName);     Serial1.print(F("  SSID PASS: "));   Serial1.println(ssidPass);                                                        
  #endif                                                          
      
 if (modeWiFi == 1)
                   {
                    WiFi.mode(WIFI_STA);
                    WiFi.begin(ssidName, ssidPass);                                     
                    while (WiFi.status() != WL_CONNECTED && i++ < 30) // Проверяем подключение
                      {
                        #if (DEBUG)
                        Serial1.print(F("."));
                        #endif 
                        delay(500);
                      }
                   }
 if (i > 30 || modeWiFi == 0)                                               // Если подключение не произошло, то:
                     { 
                      modeWiFi = 0;
                      WiFi.disconnect();                                    // Отключаем WIFI
                      delay(500);
                      WiFi.mode(WIFI_AP);                                   // Меняем на режим точки доступа
                      //  WiFi.softAPConfig(apIP, apIP,(255, 255, 255, 0)); // Задаем настройки сети 
                      WiFi.softAP("OleSton", ssidPass);                     // Включаем WIFI в режиме точки доступа 
                      #if (DEBUG)
                      Serial1.println(F("Could not connect to MikroTik2")); 
                      Serial1.print(F("My IP address: "));                  
                      Serial1.println(WiFi.softAPIP());                      // Выводим локальный IP-адрес ESP8266
                      #endif
                    } 
                      else {  
                            #if (DEBUG)
                            Serial1.println(F("My IP address: ")); Serial1.println(WiFi.localIP());
                            #endif
                            //WiFi.hostname("OleSton");
                            }     
     
      HTTP.begin();                                                      // Инициализируем Web-сервер
     
      #if (FTP)
      ftpSrv.begin("OleSton","OleSton");                                 // Поднимаем FTP-сервер для удобства отладки работы HTML 
      #endif                                                             // (логин: OleSton, пароль: OleSton)
      
// Обработка HTTP-запросов
/*------------------------------------------------------------------------------------*/
      HTTP.on("/genericArgs", handleGenericArgs);  
      HTTP.on("/getStatus"  , sendStatus); 
      HTTP.on("/getWiFiList", sendWiFiList);  
      HTTP.on("/getConnectWiFi", setConnectWiFi); 
      HTTP.onNotFound([](){                                             // Описываем действия при событии "Не найдено"
      if(!handleFileRead(HTTP.uri()))                                   // Если функция handleFileRead (описана ниже) возвращает значение false 
      HTTP.send(404, "text/plain", "Not Found");                        // в ответ на поиск файла в файловой системе возвращаем на запрос текстовое 
                      });                                               // сообщение "File isn't found" с кодом 404 (не найдено)
 /*------------------------------------------------------------------------------------*/                     
      #if (!FTP)
      RadData.attach_ms(DelayLan, readDataMemm, 1);

      #if (DEBUG)
      Debug.attach_ms(1000*5, DebugSerial1, 1);
      #endif
      StopDetectState.attach_ms(1000*5, StopDetectSt, 1);
      #endif

     EEPROM.begin(512);
     EEPROM.get(1,CountStart);    // чтение eeprom
     EEPROM.put(1,CountStart+1);  // запись eeprom
     EEPROM.commit();             // подтверждение записи
     EEPROM.get(1,CountStart);    // читаем

}

void loop()
{
   #if (!FTP)
   HTTP.handleClient();                                                // Обработчик HTTP-событий (отлавливает HTTP-запросы к устройству и обрабатывает их в соответствии с выше описанным алгоритмом)
   #endif

   #if (FTP)
   ftpSrv.handleFTP();                                                 // Обработчик FTP-соединений  
   #endif
   
} 


bool handleFileRead(String path){                                    // Функция работы с файловой системой
  if(path.endsWith("/")) path += "index.html";                       // Если устройство вызывается по корневому адресу, то должен вызываться файл index.html (добавляем его в конец адреса)
  String contentType = getContentType(path);                         // С помощью функции getContentType (описана ниже) определяем по типу файла (в адресе обращения) какой заголовок необходимо возвращать по его вызову
  if(SPIFFS.exists(path)){                                           // Если в файловой системе существует файл по адресу обращения
    File file = SPIFFS.open(path, "r");                              // Открываем файл для чтения
    size_t sent = HTTP.streamFile(file, contentType);                // Выводим содержимое файла по HTTP, указывая заголовок типа содержимого contentType
    file.close();                                                    // Закрываем файл
    return true;                                                     // Завершаем выполнение функции, возвращая результатом ее исполнения true (истина)
  }
  return false;                                                      // Завершаем выполнение функции, возвращая результатом ее исполнения false (если не обработалось предыдущее условие)
}

String getContentType(String filename){                              // Функция, возвращающая необходимый заголовок типа содержимого в зависимости от расширения файла
  if (filename.endsWith(".html")) return "text/html";                // Если файл заканчивается на ".html", то возвращаем заголовок "text/html" и завершаем выполнение функции
  else if (filename.endsWith(".css")) return "text/css";             // Если файл заканчивается на ".css", то возвращаем заголовок "text/css" и завершаем выполнение функции
  else if (filename.endsWith(".js")) return "application/javascript";// Если файл заканчивается на ".js", то возвращаем заголовок "application/javascript" и завершаем выполнение функции
  else if (filename.endsWith(".png")) return "image/png";            // Если файл заканчивается на ".png", то возвращаем заголовок "image/png" и завершаем выполнение функции
  else if (filename.endsWith(".jpg")) return "image/jpeg";           // Если файл заканчивается на ".jpg", то возвращаем заголовок "image/jpg" и завершаем выполнение функции
  else if (filename.endsWith(".gif")) return "image/gif";            // Если файл заканчивается на ".gif", то возвращаем заголовок "image/gif" и завершаем выполнение функции
  else if (filename.endsWith(".ico")) return "image/x-icon";         // Если файл заканчивается на ".ico", то возвращаем заголовок "image/x-icon" и завершаем выполнение функции
  return "text/plain";                                               // Если ни один из типов файла не совпал, то считаем что содержимое файла текстовое, отдаем соответствующий заголовок и завершаем выполнение функции
}
/*------------------------------------------------------------------------------------*/
void handleGenericArgs() { //Handler
    String message;// = "Number of args received: ";
    getHttpArgum ();
    message  ="/genericArgs?prog=" + (String)ProgNum + "&speed=" + (String)SpeedSpin + "&tempr=" + (String)Tempr + "&startSW=" + (String)ButtonStart + "&end=0" + "&macAddress=" + WiFi.macAddress();
    #if (DEBUG)
    Serial1.print("Update ");Serial1.println(message );
    #endif
    HTTP.send(200, "text/plain", message);       //Response to the HTTP request
    #if (DEBUG)
    Serial1.print("\nFrom HTML "); Serial1.println ("ButtonStart " + (String)ButtonStart + "ProgNum " + (String)ProgNum + "SpeedSpin " + (String)SpeedSpin + "Tempr" + (String)Tempr  );  
    #endif
}
/*------------------------------------------------------------------------------------*/
void sendStatus () // отправка данных в браузер
{    
    if (!State) return;
    String messageStatus, IP;
    if (!modeWiFi) {IP = WiFi.softAPIP().toString();} else {IP = WiFi.localIP().toString();}
    getHttpArgum ();
    messageStatus = "/getStatus?prog=" + (String)Command_Massive[Prog] +  "&startSW=" + (String)Command_Massive[Start] + 
                     "&statusWash=" + (String)StatusWash + "&speed=" + (String)(Command_Massive[Spin]*10) + "&tempr=" + (String)Command_Massive[Temper]+ 
                     "&IP=" + IP + "&cTemp=" + (String)Command_Massive[cTemp] + "&cTempSet=" + (String)Command_Massive[sTemp] + 
                     "&cWaterLevel=" + (String)Command_Massive[LevWat] + "&cNumProg=" + (String)Command_Massive[NumProg] + 
                     "&cPosCmd=" + (String)"101" + "&cStatusDoor=" + (String)Command_Massive[FlagDoor] + 
                     "&cSpeed=" + (String)((Command_Massive[SpeedH] << 8) + Command_Massive[SpeedL]) + 
                     "&cSecPause=" + (String)((Command_Massive[PauseH] << 8) + Command_Massive[PauseL]) + 
                     "&CountStart=" + (String)CountStart  + "&cTotalProg=" + (String)"15";
    #if (DEBUG)
    Serial1.print("Send to HTML ");Serial1.println(messageStatus );
    #endif
    HTTP.send(200, "text/plain", messageStatus );       //Response to the HTTP request
}
/*------------------------------------------------------------------------------------*/
void getHttpArgum ()      // from http
{
  for (int i = 0; i < HTTP.args(); i++)  
  {
    if (HTTP.argName(i) == "prog")    {ProgNum = HTTP.arg(i).toInt();} 
    if (HTTP.argName(i) == "speed")   {SpeedSpin = HTTP.arg(i).toInt();} 
    if (HTTP.argName(i) == "tempr")   {Tempr = HTTP.arg(i).toInt();}  
    if (HTTP.argName(i) == "startSW") {ButtonStart = HTTP.arg(i).toInt();} 

  } 
}
/*------------------------------------------------------------------------------------*/
void sendWiFiList()   // отправляет доступные WiFi сети
{
  String StrWiFiList = getWiFiStation();
  #if (DEBUG)
  Serial1.print(F("\nSend to HTML JSON "));Serial1.println(StrWiFiList );
  #endif
  HTTP.send(200, "application/json", StrWiFiList); 
}
/*------------------------------------------------------------------------------------*/
void setConnectWiFi()    // получаем название WiFi сети и пароль для следующего подключения к ней
{
    String modeWiFi_loc = "";
    String tmp_ssidPass = ssidPass;
    for (int i = 0; i < HTTP.args(); i++)  
    {
      if (HTTP.argName(i) == "ssidName")    {ssidName = HTTP.arg(i);}  
      if (HTTP.argName(i) == "ssidPass")    {ssidPass = HTTP.arg(i);}  
      if (HTTP.argName(i) == "modeWiFi")    {modeWiFi_loc = HTTP.arg(i);}   // 0 - AP; 1- STA; 2 - default (AP, ssid & passw = default); 3 - RESET; 
    }   
   if (modeWiFi_loc.toInt() != 3){  
                                  getFileWiFi("ssidName.txt", "", DEL_FILE);  
                                  getFileWiFi("ssidPass.txt", "", DEL_FILE);  
                                  getFileWiFi("modeWiFi.txt", "", DEL_FILE);                                     // delet files
                                  if (modeWiFi_loc.toInt() != 2){
                                                                  getFileWiFi("ssidName.txt", ssidName, WRITE_FILE);  
                                                                  getFileWiFi("ssidPass.txt", ssidPass, WRITE_FILE);  
                                                                  getFileWiFi("modeWiFi.txt", modeWiFi_loc, WRITE_FILE);// save data to files
                                                                 }
                                  WiFi.mode(WIFI_AP_STA);
                                  WiFi.softAP("OleSton", tmp_ssidPass);
                                  WiFi.begin(ssidName,ssidPass);
                                  //while (WiFi.status() != WL_CONNECTED) {;} //delay(100);Serial1.print(".");   
                                }
  modeWiFi = modeWiFi_loc.toInt();                              
  String SSID_argum = "?ssidName=" + ssidName + "&ssidPass=" + ssidPass + "&modeWiFi=" + modeWiFi_loc + "&ipSTA=" + WiFi.localIP().toString();
  HTTP.send(200, "text/plain", SSID_argum);                             
  if (modeWiFi_loc.toInt() > 1){ESP.reset();}
}
/*------------------------------------------------------------------------------------*/
void readDataMemm(int state) 
{
  if (Serial.available() > (byte)sizeof(Command_Massive)) // Если пришло сообщение размером с нашу структуру
  {
    byte tmp[(byte)sizeof(Command_Massive)] = {0,};
    Serial.readBytes((byte*)&tmp, (byte)sizeof(Command_Massive));
    byte crc = 0;
    for (byte i = 0; i < 20 - 1; i++)  crc ^= tmp[i];
    crc ^= 20 + 2;


    if (tmp[(byte)sizeof(Command_Massive) - 1] != crc || tmp[0] != ControlPanelID)  
     {
      #if(DEBUG)
      Serial1.print("\nerr crc ");
      #endif       
      return;
      }
    
    #if(DEBUG)
    Serial1.print("\ncrc "); Serial1.print(crc);
    Serial1.print("\n>> "); for (uint8_t i = 0; i < sizeof(tmp); i++){Serial1.print(tmp[i]); Serial1.print(" ");}
    #endif  
      
    memcpy((byte*)&Command_Massive, tmp, sizeof(tmp));

    #if(DEBUG)
    Serial1.print("\n>> ");
    for (uint8_t i = 0; i < sizeof(Command_Massive); i++) 
    {Serial1.print(Command_Massive[i]); Serial1.print(" ");}
    #endif
    RadData.detach();
    WriteData.attach_ms(DelayLan, writeDataMemm, 1);// Отвечаем контроллеру (мастеру) через 20 ms
  }
       
    Endlocal = Command_Massive[FlagEnd];
    if (Endlocal == 1) {ButtonStart = 0; Command_Massive[Start] = 0;}
    StatusWash = Command_Massive[Status];
    if (StatusWash == 222) {getFileWiFi("ssidName.txt", "", DEL_FILE);  // if status = 222 then deleted files and reset ESP 
                            getFileWiFi("ssidPass.txt", "", DEL_FILE);
                            getFileWiFi("ssidMode.txt", "", DEL_FILE);
                            ESP.reset();}
    if (!State) { 
                ButtonStart = Command_Massive[Start]; 
                Tempr       = Command_Massive[Temper];  
                ProgNum     = Command_Massive[Prog];  
                SpeedSpin   = Command_Massive[Spin]* 10; 
                }                        
    #if(DEBUG)    
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    #endif 
}
/*---------------------------------------------------------------------------------------*/
void writeDataMemm(int state)
{              
    WriteData.detach();            
    Command_Massive[ID]       = MasterID;
    Command_Massive[Start]    = ButtonStart;
    Command_Massive[Temper]   = Tempr;
    Command_Massive[Prog]     = ProgNum;
    Command_Massive[Spin]     = SpeedSpin / 10;
    byte crc = 0;
    for (byte i = 0; i < 20 - 1; i++)  crc ^= Command_Massive[i];
    crc ^= 20 + 2;
    Command_Massive[crc] = crc;
    Serial.write((byte*)&Command_Massive, sizeof(Command_Massive));           // Отвечаем контроллеру (мастеру)
    RadData.attach_ms(DelayLan, readDataMemm, 1);      
}
/*---------------------------------------------------------------------------------------*/
#if(DEBUG)
void DebugSerial1(int state)
{
  Serial1.print(F("\n\n\nCount Start ESP: ")); Serial1.println (CountStart);
  //Serial1.print(F("Stop Detect Start: "));       Serial1.println (State);
  //Serial1.print(F("Mem size Free: "));         Serial1.println (ESP.getFreeHeap());
  
  String ipSTA = WiFi.softAPIP().toString();
  String ipAP  = WiFi.localIP().toString();
  //const String IPunset = "(IP unset)";
  Serial1.print(F("My IP address STA: ")); Serial1.println(ipSTA);
  Serial1.print(F("My IP address  AP: ")); Serial1.println(ipAP);
}
#endif  
/*---------------------------------------------------------------------------------------*/
void StopDetectSt(int stste)    // завершение фазы старта системы нужно, если произошла перезагрузка модуля
{
  State = 1;
  if (WiFi.softAPIP() == WiFi.localIP())  {  // если они равны то перегружаем модуль пока пусть будет здесь
                                          #if (DEBUG)
                                          Serial1.println(F(" IP unset NOT Connect ")); 
                                          #endif
                                          ESP.reset();} 
}  
/*---------------------------------------------------------------------------------------*/
String getFileWiFi(String fileName, String data, byte command) // open file read and write file or del file 0- del 1- read 2-write
{
  fileName = "/" + fileName;
  File file;
  String result = "";
  bool success = SPIFFS.begin();
  if (success)                        // File system mounted???
  {;                                  //Serial1.println("File system mounted with success");
  } else {                            //Serial1.println("Error mounting the file system");
    return "";
  }
  if(command == DEL_FILE)                      // del file
  {
    SPIFFS.remove(fileName);
    return "";
   }

  if(command == WRITE_FILE)                    // write after read file (command = 3)
  {
      file = SPIFFS.open(fileName, "w");
      file.println(data);                     //write to file data
      file.close();                           //close file
      command = READ_FILE;
  } 

  if(command == READ_FILE)                    //read
  {
      file = SPIFFS.open(fileName, "r");      //open file for read
       if(file) 
      {
           while(file.available()) 
          {
            result = file.readStringUntil('\n');//read file
            file.close();
           }
      }else
      {
        return "";
      }
  }
 return result; 
}
/*---------------------------------------------------------------------------------------*/
String getWiFiStation() // get Json data wifi station
{
  int n = WiFi.scanNetworks();
  String StJson = "";
  // WiFi.scanNetworks will return the number of networks found
  if (n == 0) 
  {
    //Serial1.println("no networks found");
    StJson ="{"":""}";
  } 
    else 
  {
    StJson +="{";
    for (int i = 0; i < n; ++i) 
    {
       StJson +="\""+ WiFi.SSID(i)+"\":\"" + String(WiFi.RSSI(i))+"\",";
       delay(10); 
    }
    //StJson = StJson.substring(0, StJson.length()-1); //del last char ","
    StJson +="\"modeWIFI\":\""+ String(modeWiFi)+"\",\"IPSTA\":\""+ WiFi.localIP().toString() + "\",\"IPAP\":\"" + WiFi.softAPIP().toString() + "\"}";
  }
  
 return StJson;
}
/*---------------------------------------------------------------------------------------*/
