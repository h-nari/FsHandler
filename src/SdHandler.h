#ifndef _sd_hanlder_h_
#define _sd_handler_h_

#include "FsHandler.h"
#include <SD.h>

class SdHandler : public FsHandler {
 protected:
  String   m_path;
  boolean   m_enabled;
  
 public:
  SdHandler(const char *uri,const char *path,
	    bool uplodable=false,bool deletable=false);
  bool handle(ESP8266WebServer& server, HTTPMethod requestMethod,
	      String requestUri) override;
  void upload(ESP8266WebServer &server,String uri,HTTPUpload &upload) override;
  void enable(bool b = true) { m_enabled = b;}
};


#endif /* _sd_handler_h_ */

