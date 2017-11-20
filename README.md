# FsHandler
arduino library to access spiffs/sd via ESP8266WebServer

## 概要

* ESP8266WebServer用ファイルシステム参照・変更Handler
 * SpiffsHandler: SPIFFSファイル・システム用
 * SdHandler SDメモリ・ファイル・システム用
 * ファイル一覧・参照・アップロード・削除機能
* WebServerの開発がより便利に？
* プログラム開発時に特に便利

### 画面サンプル

<div style="text-align: center;">
<img src=" https://github.com/h-nari/FsHandler/blob/master/img/sc171117a4.png?raw=true" width="400" title="サンプル画面"></div>

## 使い方

SpiffsHandler或いはSdHandler型のインスタンスを宣言し、
ESP8266WebServerのaddHandlerメソッドでセットします。

具体的には
<a href="https://github.com/h-nari/FsHandler/tree/master/examples/fs_demo">FsHandler/examples/fs_demo</a>
をご参照下さい。

### ヘッダーファイル

以下のようにヘッダーファイルをインクルードして下さい。

``` c++
#define FS_NO_GLOBALS
#include <FS.h>
#include <SD.h>
#include "FsHandler.h"
#include "SpiffsHandler.h"
#include "sdHandler.h"
```

``` #define FS_NO_GLOBALS ``` はFS.hとSD.hファイルにあるFileクラスの衝突を回避するのに必要です。

### インスタンスの宣言

SpiffsHandlerクラスのコンストラクタは以下のように宣言されています。

uriはWebServer上でSPIFFSが参照されるパス。
uploadableはアップロード機能の有無、
deletableはファイル削除機能の有無を指定します。

``` c++
SpiffsHandler(const char *uri,bool uplodable=false,bool deletable=false);
```

SdHandlerのコンストラクタの宣言を下に示します。

``` c++
SdHandler(const char *uri,const char *path,
         bool uplodable=false,bool deletable=false);
```
uriは同じくWebServer上でSDメモリが参照されるパス。
pathは、Webに公開されるトップとなるSDメモリー上のディレクトリ。uploadableとdeletableはSpiffsHandlerと同じです。

サンプルプログラムでは、以下のように
インスタンスを宣言しています。

``` c++
SpiffsHandler fsHandler("rom",true,true);
SdHandler sdHandler("sd", "/", true, true);
```

### 初期化

SdHandlerの場合、自分でSD.bigin()し、成功したら
sdHandler.enable()を呼び出す必要があります。
Webからアクセスされた際、指定されたファイルが
存在しないのか、SDカードが、そもそも読めないのかを
識別するためです。

最後に、addHandler()し、server.begin()でWebServerを
起動します。

``` c++
server.addHandler(&fsHandler);
server.addHandler(&sdHandler);
server.begin();
```

### カスタマイズ

setStyleSheet(), setScript()で、
組み込まれるスタイルシート、
スクリプトファイルを指定できます。

``` c++
void setStyleSheet(const char *url) { m_urlStyleSheet = url;}
void setScript(const char *url) { m_urlScript = url;}
```

## 今後の課題

ディレクトリのファイル一覧を出力する際
出力するHTMLを一旦Stringに蓄えて出力するため、
ファイル数が多いディレクトリー等の場合
大量のメモリを消費することになります。
この問題は、将来、解決したいと思います。
