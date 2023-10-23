void notFound(AsyncWebServerRequest* request) {
  request->send(404, "text/plain", "Page Not found");
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info) {
  // Reconnect
  Serial.print("WiFi lost connection. Reason: ");
  Serial.println(info.wifi_sta_disconnected.reason);
  Serial.println("Trying to Reconnect");
  WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
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
  WiFi.onEvent(WiFiStationDisconnected, WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  // Set station mode
  WiFi.begin(WIFI_SSID.c_str(), WIFI_PASSWORD.c_str());
  Serial.print("Connecting to ");
  Serial.print(WIFI_SSID);

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
    if (user == LOGIN_USER && pwd == LOGIN_PASSWORD) {
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
    isOpenDoor = true;
    request->send_P(200, "text/plain", "Ok");
    Serial.println("Ok");
  });

  server.onNotFound(notFound);
  server.begin();
}
