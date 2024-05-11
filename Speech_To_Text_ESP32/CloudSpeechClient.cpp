#include "WString.h"
#include "HTTPClient.h"
#include "CloudSpeechClient.h"
#include "network_param.h"
#include <base64.h>
#include <ArduinoJson.h>

const char* chatgpt_token = "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";
//const char* claudeKey = "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";

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
  enc.replace("\n", "");  // delete last "\n"
  client.print(enc);      // HttpBody2
  char** wavData = audio->wavData;
  for (int j = 0; j < audio->wavDataSize / audio->dividedWavDataSize; ++j) {
    enc = base64::encode((byte*)wavData[j], audio->dividedWavDataSize);
    enc.replace("\n", "");// delete last "\n"
    client.print(enc);    // HttpBody2
  }
}

void CloudSpeechClient::Transcribe(Audio* audio) {
  String HttpBody1 = "{\"config\":{\"encoding\":\"LINEAR16\",\"sampleRateHertz\":16000,\"languageCode\":\"en-IN\"},\"audio\":{\"content\":\"";
  String HttpBody3 = "\"}}\r\n\r\n";
  int httpBody2Length = (audio->wavDataSize + sizeof(audio->paddedHeader)) * 4 / 3; // 4/3 is from base64 encoding
  String ContentLength = String(HttpBody1.length() + httpBody2Length + HttpBody3.length());
  String HttpHeader;
  // if (authentication == USE_APIKEY)
  HttpHeader = String("POST /v1/speech:recognize?key=") + ApiKey
               + String(" HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
  //  else if (authentication == USE_ACCESSTOKEN)
  //    HttpHeader = String("POST /v1beta1/speech:syncrecognize HTTP/1.1\r\nHost: speech.googleapis.com\r\nContent-Type: application/json\r\nAuthorization: Bearer ")
  //   + AccessToken + String("\r\nContent-Length: ") + ContentLength + String("\r\n\r\n");
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
   // Serial.write(client.read());
  }

//   Serial.print("My Answer - ");Serial.println(My_Answer);
  int postion = My_Answer.indexOf('{');
//   Serial.println(postion);
  ans = My_Answer.substring(postion);
//   Serial.print("Json daata--");
//   Serial.print(ans);

  StaticJsonDocument<384> doc;
  DeserializationError error = deserializeJson(doc, ans);
  if (error)
    Serial.println("Deserialization failed");

  JsonObject results = doc["results"][0];
  String res = results["alternatives"][0]["transcript"];

//   res = res + " in 40 words";
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

// ------------------------------------------------------------------------------------------------------------------------------
// CHATGPT REQUEST

//   HTTPClient https;

// //   //Serial.print("[HTTPS] begin...\n");
//   if (https.begin("https://api.openai.com/v1/completions")) {  // HTTPS
//     Serial.println("Connected to API...");
//     https.addHeader("Content-Type", "application/json");
//     String token_key = String("Bearer ") + chatgpt_token;
//     https.addHeader("Authorization", token_key);

//     String payload = String("{\"model\": \"gpt-3.5-turbo-instruct\", \"prompt\": ") + res + String(", \"temperature\": 0.15, \"max_tokens\": 40}"); //Instead of TEXT as Payload, can be JSON as Paylaod

//     //Serial.print("[HTTPS] GET...\n");

//     // start connection and send HTTP header
//     int httpCode = https.POST(payload);
//     Serial.println(httpCode);
//     // httpCode will be negative on error
//     // file found at server
//     if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
//       Serial.println("Getting answer...");
//       String payload = https.getString();
//       //Serial.println(payload);

//       DynamicJsonDocument doc(1024);


//       deserializeJson(doc, payload);
//       String Answer = doc["choices"][0]["text"];
//       Answer = Answer.substring(2);
//       Serial.print("Answer : "); Serial.println(Answer);

//     }
//     else {
//       Serial.printf("[HTTPS] GET... failed, error: %s\n", https.errorToString(httpCode).c_str());
//     }
//     https.end();
//   }
//   else {
//     Serial.printf("[HTTPS] Unable to connect\n");
//   }
// ------------------------------------------------------------------------------------------------------------------------------

// ------------------------------------------------------------------------------------------------------------------------------
// CLAUDE REQUEST

//   if (https.begin("https://api.anthropic.com/v1/messages")) {  // HTTPS
//     Serial.println("Connected to Claude API...");
    
//     // Add headers
//     https.addHeader("x-api-key", claudeKey);
//     https.addHeader("anthropic-version", "2023-06-01");
//     https.addHeader("content-type", "application/json");

//     // Construct JSON payload
//     DynamicJsonDocument payloadDoc(512);
//     payloadDoc["model"] = "claude-3-opus-20240229";
//     payloadDoc["max_tokens"] = 100;
//     JsonArray messagesArray = payloadDoc.createNestedArray("messages");
//     JsonObject message = messagesArray.createNestedObject();
//     message["role"] = "user";
//     message["content"] = res;

//     // Serialize JSON payload to a string
//     String payloadString;
//     serializeJson(payloadDoc, payloadString);

//     // Start connection and send HTTP POST request
//     int httpCode = https.POST(payloadString);
//     Serial.println(httpCode);

//     // Check response
//     if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
//         Serial.println("Getting answer...");
//         String response = https.getString();

//         // Deserialize JSON response
//         DynamicJsonDocument responseDoc(1024);
//         deserializeJson(responseDoc, response);

//         // Check if the response contains the expected fields
//         if (responseDoc.containsKey("content")) {
//             // Extract answer from JSON response
//             String answer = responseDoc["content"][0]["text"].as<String>();
//             Serial.print("Answer : ");
//             Serial.println(answer);
//         } else {
//             Serial.println("No answer found in response.");
//             }
//         } else {
//         Serial.printf("[HTTPS] POST failed, error: %s\n", https.errorToString(httpCode).c_str());
//         }

//     // End HTTPS connection
//     https.end();
//     } else {
//         Serial.printf("[HTTPS] Unable to connect\n");
//     }
// ------------------------------------------------------------------------------------------------------------------------------
}
