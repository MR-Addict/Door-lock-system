void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("An error has occurred while mounting SPIFFS");
    ESP.restart();
    return;
  }
}

bool updateSPIFFS() {
  // Read file
  File file = SPIFFS.open("/profile.json", "r");
  if (!file || file.isDirectory()) {
    Serial.println("Failed to open file for reading!");
    return false;
  }
  String message;
  while (file.available()) {
    message += (char)file.read();
  }
  file.close();

  // Read JSON
  DynamicJsonDocument doc(200);
  DeserializationError error = deserializeJson(doc, message);
  if (error) {
    Serial.print(F("deserializeJson() failed: "));
    Serial.println(error.f_str());
    return false;
  }
  if (doc.containsKey("ssid"))wifi.ssid = doc["ssid"].as<String>();
  else return false;
  if (doc.containsKey("pwd")) wifi.pwd = doc["pwd"].as<String>();
  else return false;
  if (doc.containsKey("hostname")) wifi.hostname = doc["hostname"].as<String>();
  else return false;
  if (doc.containsKey("login_user")) wifi.login_user = doc["login_user"].as<String>();
  else return false;
  if (doc.containsKey("login_pwd")) wifi.login_pwd = doc["login_pwd"].as<String>();
  else return false;
  return true;
}

void writeSPIFFS(String wmssid, String wmpwd, String wmhostname, String wmlogin_user, String wmlogin_pwd) {
  SPIFFS.remove("/profile.json");
  File file = SPIFFS.open("/profile.json", "w");
  if (!file || file.isDirectory()) {
    Serial.println("Failed to creat file for writing!");
    return;
  }

  // Set the values in the document
  StaticJsonDocument<200> doc;
  doc["ssid"] = wmssid;
  doc["pwd"] = wmpwd;
  doc["hostname"] = wmhostname;
  doc["login_user"] = wmlogin_user;
  doc["login_pwd"] = wmlogin_pwd;

  // Serialize JSON to file
  if (serializeJson(doc, file) == 0) {
    Serial.println(F("Failed to write to file"));
  }

  // Close the file
  file.close();
}
