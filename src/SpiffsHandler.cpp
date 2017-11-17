#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "SpiffsHandler.h"

SpiffsHandler::SpiffsHandler(const char *uri, bool uploadable, bool deletable)
  : FsHandler(uri, uploadable, deletable)  
{
  m_mounted = SPIFFS.begin();
}

bool SpiffsHandler::handle(ESP8266WebServer& server, HTTPMethod method,
		       String requestUri)
{
  if(!canHandle(method, requestUri)) return false;

  if(!m_mounted){
    server.send(404, "text/plain", "SPIFFS not formated");
    return true;
  }
  
  String path = requestUri.substring(m_baseUriLength+1);

  if(method == HTTP_DELETE){
    Serial.printf("%s:%d delete:%s\n",__FUNCTION__,__LINE__,path.c_str());
    if(SPIFFS.remove(path)){
      server.send(200, "text/plain", "delete succeeded");
      return true;
    } else {
      server.send(404, "text/plain", "delete failed");
      return true;
    }
  }
  
  if(path == "" || path == "/" || method == HTTP_DELETE){
    fs::Dir dir = SPIFFS.openDir("");
    String s;
    s.reserve(2048);

    addHtmlHeaderStr(s, "SPIFFS File List");
    s += F("<a href='../'>parent directory</a><br/>");
    addTableBegin(s);
    addTableDir(s, "..","..");
    String link;
    while(dir.next()){
      fs::File f = dir.openFile("r");
      const char *name = f.name();
      link  = "/";
      link += m_uri;
      link += name;
      addTableEntry(s, name, link.c_str(), f.size());
      f.close();
    }
    addTableEnd(s);

    server.send(200, "text/html", s);
    Serial.printf("%s:%d length:%d\n",__FUNCTION__,__LINE__,s.length());
  }
  else {
    fs::File f = SPIFFS.open(path,"r");
    if(f){
      String contentType = getContentType(path);
      server.streamFile(f, contentType);
      f.close();
    } else {
      Serial.printf("Spiffs:%s not found.\n",path.c_str());
      server.send(404, "text/plain", "file not found");
    }
  }
  return true;
}

void SpiffsHandler::upload(ESP8266WebServer &server, String uri,
			   HTTPUpload &upload)
{
  static fs::File f;
  
  if(upload.status == UPLOAD_FILE_START){
    String path = upload.filename;
    Serial.printf("upload: start:%s\n",path.c_str());
    if(upload.filename == "")
      return;
    
    f = SPIFFS.open(path, "w");
    if(!f) Serial.printf("open %s failed.\n", path.c_str());
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(f) f.write(upload.buf, upload.currentSize);
    Serial.printf("upload: write %u\n",upload.currentSize);
  } else if(upload.status == UPLOAD_FILE_END){
    if(f) f.close();
    Serial.printf("Upload: END, Size: %u\n",upload.totalSize);
  }
}
