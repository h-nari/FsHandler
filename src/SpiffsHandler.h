#ifndef _spiffs_handler_h_
#define _spiffs_handler_h_

#include "FsHandler.h"
#define FS_NO_GLOBALS
#include <FS.h>

class SpiffsHandler : public FsHandler {
 protected:
  boolean m_mounted;
 public:
  SpiffsHandler(const char *uri,bool uplodable=false,bool deletable=false);
  bool handle(ESP8266WebServer& server, HTTPMethod requestMethod,
	      String requestUri) override;
  void upload(ESP8266WebServer &s, String uri, HTTPUpload &upload) override;
};


#endif /* _spiffs_handler_h_ */

