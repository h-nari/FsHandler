#include <Arduino.h>
#include <ESP8266WebServer.h>
#include "FsHandler.h"

FsHandler::FsHandler(const char *uri, bool uploadable, bool deletable) :
  m_uploadable(uploadable), m_deletable(deletable)
{
  m_uri = uri;
  if(m_uri[m_uri.length()-1] != '/')
    m_uri += "/";
  m_baseUriLength = m_uri.length();
  m_urlStyleSheet = NULL;
  m_urlScript = NULL;
}

bool FsHandler::canHandle(HTTPMethod method, String uri)
{
  const char *rp = uri.c_str();

  //  m_uri:"rom/"  rp:"/rom/..." => ok
  
  if(rp[0] != '/' ||
     strncmp(m_uri.c_str(),rp+1,m_baseUriLength)!=0) return false;
  
  if(method == HTTP_GET)
    return true;
  else if(method == HTTP_POST)
    return  m_uploadable;
  else if(method == HTTP_DELETE)
    return m_deletable;
  return false;
}


bool FsHandler::canUpload(String uri)
{
  return canHandle(HTTP_POST, uri);
}

void FsHandler::addHtmlHeaderStr(String &s,const char *title)
{
  s += F("<html><head>");
  s += F(" <meta charset='utf-8'/>\r\n");
  s += F(" <title>");
  s += title;
  s += F("</title>\r\n");

  if(m_urlStyleSheet){
    s += F(" <link rel='stylesheet' type='text/css' media='screen'");
    s += F(" href='");
    s += m_urlStyleSheet;
    s += F("' />\r\n");
  } else {
    s += F("<style type='text/css'>\n");
    s += F("BODY{background-color: #f0f0ff;}\n");
    s += F(".file_list {");
    s += F("border:solid 1px #000000;");
    s += F("border-collapse:separate;");
    s += F("margin:40px 20px;");
    s += F("background-color:#808080;");
    s += F("}\n");
    s += F(".file_list TH{background-color:#f0f0f0;}\n");
    s += F(".file_list TD{background-color: #ffffff;padding:5px 40px;}\n");
    s += F(".file_list .btn{padding: 2px;}\n");
    s += F(".upload{width: 400px;margin:40px 20px;padding:10px;");
    s += F("border: solid 1px #000000;}\n");
    s += F(".upload INPUT{margin:10px;padding:5px;");
    s += F("background-color:#ffffff;}\n");
    s += F("</style>\n");
  }
  s += F(" <script src='https://code.jquery.com/jquery-3.2.1.min.js'");
  s += F(" integrity='sha256-hwg4gsxgFZhOsEEamdOYGBf13FyQuiTwlAQgxVSNgt4='");
  s += F(" crossorigin='anonymous'></script>\r\n");
  if(m_urlScript){
    s += F(" <script src='");
    s += m_urlScript;
    s += F("'></script>\n");
  } else {
    s += F("  <script>");
    s += F("    function deleteFsFile(path) {\n");
    s += F("      if(confirm('本当に ' + path + ' を削除しますか？')){\n");
    s += F("        $.ajax({\n");
    s += F("          url: location.pathname + path,\n");
    s += F("          type: 'DELETE',\n");
    s += F("          dataType: 'html',\n");
    s += F("        }).done(function(data){\n");
    s += F("          document.write(data);\n");
    s += F("        }).fail(function(jqXHR, status){\n");
    s += F("          alert('ファイルの削除に失敗しました。');\n");
    s += F("        });\n");
    s += F("      }\n");
    s += F("    }\n");

    s += F("  function fsPageInit() {\n");
    s += F("    if(!location.pathname.match(/\\/$/))\n");
    s += F("      location.pathname += '/';\n");
    s += F("  }\n");

    s += F("  $(document).ready(fsPageInit);\n");
    s += F("  </script>");
  }
  s += F("</head>");
  s += F("<body>\r\n");
  s += F("<h1>");
  s += title;
  s += F("</h1>\r\n");
}

void FsHandler::addTableBegin(String &s)
{
  s += F("<table class='file_list'>\r\n");
  s += F("<tr><th>name</th><th>dir/size</th>");
  if(m_deletable)
    s += F(" <th></th>");
  s += F("\r\n");
}

void FsHandler::addTableEntry(String &s, const char *name, const char *link,
			      uint32_t size)
{
  s += F("<tr><td>");
  s += F("<a href='");
  s += link;
  s += F("'>");
  s += name;
  s += F("</a></td><td align='right'>");
  s += size;
  s += F(" bytes");
  s += F("</td>");
  if(m_deletable ){
    s += F("<td class='btn'><button onClick='deleteFsFile(\"");
    s += name;
    s += F("\");'>delete</button>");
    s += F("</td>");
  }
  s += F("</tr>\r\n");
}

void FsHandler::addTableDir(String &s, const char *name, const char *link)
{
  s += F("<tr><td>");
  s += F("<a href='");
  s += link;
  s += F("'>");
  s += name;
  s += F("</a></td><td align='right'>Directory</td>");
  if(m_deletable)
    s += F("<td></td>");
  s += F("</tr>\r\n");
}

void FsHandler::addTableEnd(String &s)
{
  s += F("</table>\r\n");
  if(m_uploadable){
    s += F("<form method='post' enctype='multipart/form-data'");
    s += F("class='upload'>");
    s += F("<input type='file' size='40'  name='file1'>");
    s += F("<input type='submit' value='upload'>");
    s += F("</form>\r\n");
    s += F("</body></html>\r\n");
  }
}

const char *FsHandler::getContentType(const String &path)
{
  const char *ct;
  if(path.endsWith(F(".HTM")) || path.endsWith(F(".htm")))
    ct = "text/html";
  else if(path.endsWith(F(".JPG")) || path.endsWith(F(".jpg")))
    ct = "image/jpeg";
  else if(path.endsWith(F(".BMP"))|| path.endsWith(F(".bmp")))
    ct = "image/bmp";
  else
    ct = "text/plain";
  return ct;
}
