#ifndef _fs_handler_h_
#define _fs_handler_h_

#include <detail/RequestHandler.h>

class FsHandler : public RequestHandler {
 protected:
  String m_uri;
  size_t m_baseUriLength;
  bool   m_uploadable;
  bool   m_deletable;
  const char *m_urlStyleSheet;
  const char *m_urlScript;
  bool   m_bAuth;
  String m_sAuthUser;
  String m_sAuthPasswd;
  
 public:
  FsHandler(const char *uri,bool uplodable=false,bool deletable=false);
  bool canHandle(HTTPMethod method, String uri) override;
  bool canUpload(String requestUri);
  void setUploadable(bool t) { m_uploadable = t;}
  void setDeletable(bool t)  { m_deletable  = t;}
  void setStyleSheet(const char *url) { m_urlStyleSheet = url;}
  void setScript(const char *url) { m_urlScript = url;}
  void setAuth(const char *user, const char *pwd);
  
 protected:
  const char *getContentType(const String &path);
  void addHtmlHeaderStr(String &s,const char *title);
  void addTableBegin(String &s);
  void addTableEntry(String &s,const char *name,const char *link,uint32_t size);
  void addTableDir(String &s, const char *name, const char *link);
  void addTableEnd(String &s);
  virtual bool authCheck(ESP8266WebServer& server);
};

#endif /* _fs_handler_h_ */

