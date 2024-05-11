#include "WString.h"
#include "HTTPClient.h"
#include "CloudSpeechClient.h"
#include "network_param.h"
#include <base64.h>
#include <ArduinoJson.h>

const char* chatgpt_token = CHATGPT_API_KEY;

CloudSpeechClient::CloudSpeechClient(Authentication authentication) 
{
    this->authentication = authentication;
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) delay(1000);
    client.setCACert(root_ca);
    if (!client.connect(server, 443)) Serial.println("Connection failed!");
}

String ans;

CloudSpeechClient::~CloudSpeechClient() {
    client.stop();
    WiFi.disconnect();
}

void CloudSpeechClient::PrintHttpBody2(Audio* audio)
{
    String enc = base64::encode(audio->paddedHeader, sizeof(audio->paddedHeader));
    enc.replace("\n", "");
    client.print(enc);
    char** wavData = audio->wavData;
    for (int j = 0; j < audio->wavDataSize / audio->dividedWavDataSize; ++j) {
        enc = base64::encode((byte*)wavData[j], audio->dividedWavDataSize);
        enc.replace("\n", "");
        client.print(enc);
    }
}

void CloudSpeechClient::Transcribe(Audio* audio) {
    String HttpBody1 = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"en-IN\"},\"audio\":{\"content\":\"";
    String HttpBody3 = "\"}}\r\n\r\n";
    int httpBody2Length = (audio->wavDataSize + sizeof(audio->paddedHeader)) * 4 / 3; // 4/3 is from base64 encoding
    String ContentLength = String(HttpBody1.length() + httpBody2Length + HttpBody3.length());
    String HttpHeader;

    HttpHeader = String("POST /v1/speech:recognize?key=") + ApiKey
                 + String(" HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");

    client.print(HttpHeader);
    client.print(HttpBody1);
    PrintHttpBody2(audio);
    client.print(HttpBody3);
    String My_Answer="";
    while (!client.available());
     
    while (client.available())
    {
        char temp = client.read();
        My_Answer = My_Answer + temp;
    }

    int postion = My_Answer.indexOf('{');
    ans = My_Answer.substring(postion);
  
    StaticJsonDocument<384> doc;
    DeserializationError error = deserializeJson(doc, ans);
    if (error)
        Serial.println("Deserialization failed");
  
    JsonObject results = doc["results"][0];
    String res = results["alternatives"][0]["transcript"];

    Serial.println(res);
  
    HTTPClient http;
    String url = "http://" + String(serverAddress) + ":" + String(serverPort) + "/data";
  
    http.begin(url);
    http.addHeader("Content-Type", "text/plain");
  
    int httpCode = http.POST(res);
  
    if(httpCode > 0)
    {
        Serial.print("HTTP Resposnse Code: ");
        Serial.println(httpCode);
        String payload = http.getString();
        Serial.println("Server resposne: " + payload);
    }
    else
    {
        Serial.print("HTTP POST Error: ");
        Serial.println(httpCode);
    }
    http.end();
}
