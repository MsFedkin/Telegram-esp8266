#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <dht11.h>

// Для подключения к WIFI
#define _SSID "TP-Link_6A88"
#define _PASSWORD "07741203"

#define BOT_TOKEN  "1138203036:AAFEosSBUTexaTVQSkDZAvNvrOaTp8G-tD0"

#define FAN_PIN D3
#define LED_PIN D2
#define DHT_PIN D1
  
WiFiClientSecure esp; // вайфай на ESP
UniversalTelegramBot bot(BOT_TOKEN,esp);

int8_t numNewMessages; //переменная для хронения количесвто новых сообщений 

struct // структура для разрешений
{
  bool fan;
  bool led;
} permition;


dht11 dht;

void setup() {
  Serial.begin(9600);

  pinMode(FAN_PIN,OUTPUT);
  pinMode(LED_PIN,OUTPUT); 
  
  esp.setInsecure();
  WiFi.mode(WIFI_STA);// устанавливаем вайфай в режим станции
  WiFi.disconnect();
  
  WiFi.begin(_SSID,_PASSWORD); // подключение ESP к WiFi
  Serial.print(F("Connecting to WiFi.."));
  while (WiFi.status() != WL_CONNECTED) // ожидания подключения к WiFi
  { 
    delay(500);
    Serial.print('.');
  }
  Serial.println("");
  Serial.println("Connected");
}

void loop() {

  dht.read(DHT_PIN); // чтение DHT
 
  numNewMessages = bot.getUpdates(bot.last_message_received + 1); // читаем количество сообщений

  for(int i = 0; i < numNewMessages; i++) // Обработка сигнала
  {
    
    String chat = String(bot.messages[i].chat_id);
    String txt = bot.messages[i].text;  

    if (txt == "/start" || txt == "Main Menu")
    {
      String keyboardJson = "[[\"/LED\"],"; // 1 строка кнопки
      keyboardJson += "[\"/FAN\"],"; // 2 строка кнопки
      keyboardJson += "[\"/DHT\"]]"; // 3 строка кнопки
      bot.sendMessageWithReplyKeyboard(chat, " Main Menu", "", keyboardJson, true);
      Serial.println("Start");
      // обнуление разрешений
      permition.fan = false;
      permition.led = false;
    }    
    
    if(txt == "/FAN")
    {
      String keyboardJson = "[[\"/ON\",\"/OFF\"],";
      keyboardJson +="[\"Main Menu\"]]"; // возврат в главное меню
      bot.sendMessage(chat,"FAN = " + String(digitalRead(FAN_PIN)? "ON" : "OFF"),"");
      bot.sendMessageWithReplyKeyboard(chat, "FAN Menu", "", keyboardJson, true); 
      permition.fan = true;     
    }
    else if (txt == "/LED")
    {
      String keyboardJson = "[[\"/ON\",\"/OFF\"],";
      keyboardJson +="[\"Main Menu\"]]"; // возврат в главное меню
      bot.sendMessage(chat,"LED = " + String(digitalRead(LED_PIN)? "ON" : "OFF"),"");
      bot.sendMessageWithReplyKeyboard(chat, "LED Menu", "", keyboardJson, true);
      permition.led = true;
    }
    else if (txt == "/DHT")
    { 
      bot.sendMessage(chat,"Temperature = " + String(dht.temperature) + "\n" + "Humidity = " + String(dht.humidity) + "\n","");
    }  
    else if (txt == "/ON") 
    {
      if(permition.led) digitalWrite(LED_PIN,true);
      if(permition.fan) digitalWrite(FAN_PIN,true);
    }
    else if (txt == "/OFF") 
    {
      if(permition.led) digitalWrite(LED_PIN,false);
      if(permition.fan) digitalWrite(FAN_PIN,false);
    }

  }// конец обработки программы 


}// конец loop
