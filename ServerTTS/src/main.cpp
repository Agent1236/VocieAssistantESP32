#include "WiFi.h"
#include "Audio.h"

const char* ssid = "JioFiber-003NV";
const char* pass = "aloobhindi";

#define I2S_DOUT    25
#define I2S_BCLK    27
#define I2S_LRC     26

Audio audio;
WiFiServer server(3000);

void setup()
{
    Serial.begin(115200);

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(1000);
    }
    Serial.println("");

    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    audio.setPinout(I2S_BCLK, I2S_LRC, I2S_DOUT);
    audio.setVolume(100);

    server.begin();
}

void loop()
{
    WiFiClient client = server.available();
    if (client)
    {
        Serial.println("Client connected!");
        
        String reqData = client.readString();
        
        Serial.println("Received data:");
        Serial.println(reqData);
        int mainTextStart = reqData.indexOf("\r\n\r\n");
        String mainText = reqData.substring(mainTextStart);
        mainText.trim();
        Serial.println("----------------------------");
        Serial.println(mainText);

        client.stop();

        audio.connecttospeech(mainText.c_str(), "en");
    }
    audio.loop();
}

void audio_info(const char *info)
{
    Serial.println(info);
}