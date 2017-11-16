#include "conf.h"

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <stdlib.h>
#include <time.h>
#include "ota_util.h"

#define FS_NO_GLOBALS
#include <FS.h>
#include <SD.h>
#include "FsHandler.h"
#include "SpiffsHandler.h"
#include "sdHandler.h"

#define SD_CS	   0		/* CS signal for SDcard: IO0 */ 

ESP8266WebServer server(80);
SpiffsHandler fsHandler("rom",true,true);
SdHandler sdHandler("sd", "/", true, true);

void setup(void){
  Serial.begin(115200);

	WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  Serial.println("\n\nReset:");
	delay(100);
	
  if(SD.begin(SD_CS)) {
    Serial.printf("initialize SD Card done.\n");
		sdHandler.enable();
  } else
    Serial.printf("initialise SD Card failed.\n");

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  server.on("/", []{
      String s;
      s += F("<h1>fs_demo</h1>");
			s += F("<p>");
      s += F("<a href='./rom/'>rom</a><br/>");
      s += F("<a href='./sd/'>sd</a><br/>");
			s += F("</p>");
      server.send(200,"text/html",s);
    });
	
  server.addHandler(&fsHandler);
  server.addHandler(&sdHandler);
  server.begin();
  Serial.println("HTTP server started");

#if USE_OTA
	ota_init();
#endif
}

void loop(void){
#if USE_OTA
  ArduinoOTA.handle();
#endif
  server.handleClient();
}

/*** Local variables: ***/
/*** tab-width:2 ***/
/*** End: ***/
