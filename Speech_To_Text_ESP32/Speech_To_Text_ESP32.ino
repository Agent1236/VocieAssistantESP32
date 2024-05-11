#include "Audio.h"
#include "CloudSpeechClient.h"

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 1, 3);
  delay(5000);
  Serial.println("\r\nRecord start!\r\n");
  Audio* audio = new Audio(ICS43434);
  //Audio* audio = new Audio(M5STACKFIRE);
  audio->Record();
  Serial.println("Recording Completed. Now Processing...");
  CloudSpeechClient* cloudSpeechClient = new CloudSpeechClient(USE_APIKEY);
  cloudSpeechClient->Transcribe(audio);
  delete cloudSpeechClient;
  delete audio;
}

void loop() {
}
