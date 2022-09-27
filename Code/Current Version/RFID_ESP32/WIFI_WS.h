void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Page Not found");
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  // Reconnect
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(ssid.c_str(), password.c_str());
}

void WIFI_Init() {
  // Init SPIFFS
  if (!SPIFFS.begin()) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // change hostname to unlockdoor-507
  String hostname = "unlockdoor-507";
  WiFi.mode(WIFI_STA);
  WiFi.config(INADDR_NONE, INADDR_NONE, INADDR_NONE, INADDR_NONE);
  WiFi.setHostname(hostname.c_str());

  // Set WIFI disconnect event
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);

  // Set station mode
  WiFi.begin(ssid.c_str(), password.c_str());
  Serial.print("Connecting to ");
  Serial.print(ssid.c_str());

  // Connect to WIFI
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print('.');
  }
  Serial.println();
  Serial.println(WiFi.localIP());
}

void Server_Init() {
  // Home page
  server.on("/", [](AsyncWebServerRequest * request) {
    // Get POST data
    String user = "", pwd = "";
    uint8_t params = request->params();
    for (uint8_t i = 0; i < params; i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (p->isPost()) {
        if (strcmp(p->name().c_str(), "username") == 0) {
          user = p->value().c_str();
        } else if (strcmp(p->name().c_str(), "password") == 0) {
          pwd = p->value().c_str();
        }
      }
    }

    // check user and pwd
    if (user == login_user && pwd == login_pwd) {
      request->send(SPIFFS, "/index.html", "text/html");
    } else {
      request->redirect("/login");
    }
  });

  // Login page
  server.on("/login", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/login.html", "text/html");
  });

  server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest * request) {
    request->send(SPIFFS, "/favicon.png", "image/png");
  });

  server.on("/unlockdoor", HTTP_POST, [](AsyncWebServerRequest * request) {
    for (uint8_t i = 0; i < request->params(); i++) {
      AsyncWebParameter* p = request->getParam(i);
      if (strcmp(p->name().c_str(), "Door") == 0) {
        if (strcmp(p->value().c_str(), "Open") == 0) {
          isOpenDoor = true;
          request->send_P(200, "text/plain", "Ok");
          Serial.println("Ok");
          return;
        }
      }
    }
    Serial.println("Bad request");
    request->send_P(400, "text/plain", "Bad Request");
  });

  server.onNotFound(notFound);
  server.begin();
}
