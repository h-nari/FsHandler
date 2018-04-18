#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "SdHandler.h"

SdHandler::SdHandler(const char *uri,const char *path,
		     bool uploadable, bool deletable)
  : FsHandler(uri,uploadable, deletable) , m_path(path)
{
  m_enabled = false;
}

bool SdHandler::handle(ESP8266WebServer& server, HTTPMethod requestMethod,
		      String requestUri)
{
  if(!canHandle(requestMethod, requestUri)) return false;

  if(!authCheck(server)) return true;
  
  if(!m_enabled){
    server.send(404,"text/plain", "SD card not found");
    return true;
  }

  String path(m_path);
  String spath = requestUri.substring(m_baseUriLength+1);

  while(spath.startsWith("/"))
    spath = spath.substring(1);
  if(!path.endsWith("/"))
    path += "/";
  path += spath;

  if(requestMethod == HTTP_DELETE){
    if(SD.remove((char *)path.c_str())){
      server.send(200, "text/plain", "delete succeeded");
      return true;
    } else {
      server.send(500,"text/plain", "delete failed");
      return true;
    }
    int i = path.lastIndexOf('/');
    if(i < 0)
      path = "";
    else
      path.remove(i);
  }

  File f = SD.open(path);
  if(!f) {
    server.send(404,"text/plain","not found");
    return true;
  }
  if(!f.isDirectory()) {
    String contentType = getContentType(path);
    server.streamFile(f, contentType);
  } else { /* directory */
    String title;
    String s;
    s.reserve(1000);

    title += F("SD dir '");
    title += path;
    title += F("'");
    addHtmlHeaderStr(s, title.c_str());

    s += F("<a href='../'>parent directory</a><br/>");

    f.rewindDirectory();

    addTableBegin(s);
    addTableDir(s, "..", "..");
    for(File entry = f.openNextFile(); entry; entry = f.openNextFile()){
      if(entry.isDirectory())
	addTableDir(s, entry.name(), entry.name());
      else
	addTableEntry(s, entry.name(), entry.name(), entry.size()); 
      entry.close();
    }
    addTableEnd(s);

    server.send(200,"text/html", s);
  }
  f.close();
  return true;
}

void SdHandler::upload(ESP8266WebServer &server, String uri,
		       HTTPUpload &upload)
{
  static File f;
  static unsigned long tStart,tUpdated;
  unsigned long now = micros();
  
  if(upload.status == UPLOAD_FILE_START){
    if(upload.filename == "")
      return;
    String path = m_path;
    path += uri.substring(m_baseUriLength);
    if(!path.endsWith("/"))
      path += "/";
    path += upload.filename;
    Serial.printf("upload: start uri:%s m_uri:%s uriLen:%u m_path:%s path:%s\n",
		  uri.c_str(),m_uri.c_str(), m_baseUriLength,
		  m_path.c_str(), path.c_str());
    
    f = SD.open(path, FILE_WRITE | O_TRUNC);
    if(!f) Serial.printf("open %s failed.\n", path.c_str());
    tStart = tUpdated = now;
  } else if(upload.status == UPLOAD_FILE_WRITE){
    if(f) f.write(upload.buf, upload.currentSize);
    if(now - tUpdated > 1000){
      tUpdated = now;
      uint32_t t = upload.totalSize;
      Serial.printf("upload: write %dbyte %lumsec\n",t,(now - tStart)/1000);
    }
  } else if(upload.status == UPLOAD_FILE_END){
    if(f) f.close();
    uint32_t t = upload.totalSize;
    Serial.printf("Upload: END, Size: %ubyte %lumsec\n",t,(now - tStart)/1000);
  }
}
