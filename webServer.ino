#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DHT.h>

ESP8266WebServer server(80);
float temp;
float hum;
DHT dht11(D4, DHT11);
const int ledPin = D6; 
bool isLedOn = false;
String status;

void init_wifi(String ssid, String password) {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup(void) {
  pinMode(ledPin, OUTPUT); 
  digitalWrite(ledPin, LOW); 
  Serial.begin(115200);
  init_wifi("vivo V23 5G", "212224236");
  server.begin();
  Serial.println("HTTP server started");
  server.on("/toggleLED", HTTP_GET, [](){
    isLedOn = !isLedOn;
    if (isLedOn) {
      digitalWrite(ledPin, HIGH);
      status = "ON";
    } else {
      digitalWrite(ledPin, LOW);
      status = "OFF";
    }
    server.send(200, "text/plain", status);
  });
  server.on("/", HTTP_GET,[](){
    ReadDHT11();
    status = isLedOn ? "ON":"OFF";
    String htmlContent = "<!DOCTYPE html>"
                        "<html>"
                        "<head>"
                        "<meta charset=\"utf-8\">"
                        "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
                        "<link href=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/css/bootstrap.min.css\" rel=\"stylesheet\" integrity=\"sha384-1BmE4kWBq78iYhFldvKuhfTAU6auU8tT94WrHftjDbrCEXSU1oBoqyl2QvZ6jIW3\" crossorigin=\"anonymous\">"
                        "<title>Arduino Web Page</title>"
                        "</head>"
                        "<body>"
                        "<div class=\"container d-flex justify-content-center flex-column align-items-center\">"
                        "<h1 class=\"mb-4\">64107899 Panachat Aiamnam</h1>"
                        "<button type=\"button\" class=\"btn btn-secondary mb-3\" id=\"ledButton\" onclick=\"toggleLED()\">Toggle LED</button>"
                        "<span id=\"status\" class=\"mb-3\">Status: "+status+"</span>"
                        "<div class=\"badge bg-danger text-wrap mb-3\" style=\"width: 10rem;\">"
                        "<p class=\"mb-3\">Temperature: " + String(temp) + " &#8451;</p>"
                        "<p class=\"mb-3\">Humidity: " + String(hum) + " &#8451;</p>"
                        "</div>"
                        "</div>"
                        "<script>"
                        "function toggleLED() { fetch('/toggleLED').then((response) => response.text())"
                        ".then(status => {"
                        "  document.getElementById('status').innerText = 'Status: ' + status;"
                        "  var button = document.getElementById('ledButton');"
                        "  if (status === 'ON') {"
                        "    button.classList.remove('btn-primary');"
                        "    button.classList.add('btn-success');"
                        "  } else {"
                        "    button.classList.remove('btn-success');"
                        "    button.classList.add('btn-warning');"
                        "  }"
                        "})"
                        ".then(() => window.location.reload(true)); }"
                        "</script>"
                        "<script src=\"https://cdn.jsdelivr.net/npm/bootstrap@5.1.3/dist/js/bootstrap.bundle.min.js\" integrity=\"sha384-ka7Sk0Gln4gmtz2MlQnikT1wXgYsOg+OMhuP+IlRH9sENBO0LRn5q+8nbTov4+1p\" crossorigin=\"anonymous\"></script>"
                        "</body>"
                        "</html>";
    server.send(200, "text/html", htmlContent);
  });
}

void ReadDHT11() {
  temp = 0;
  hum = 0;
  dht11.begin();
  temp = dht11.readTemperature();
  hum = dht11.readHumidity();
  delay(1000);
}

void loop() {
  server.handleClient();
}
