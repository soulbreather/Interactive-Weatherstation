// Main file for Interactive Weatherstation
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>

// Set these to your desired credentials.
const char *ssid = "vejr";          // Enter your WIFI ssid
const char *password = "123456789"; // Enter your WIFI password

// API
const String host = "api.openweathermap.org";
const String url = "/data/2.5/weather?q=Moscow&units=metric&appid=da4b275dfea6fdb17c656dc5ccacff61";

const size_t capacity = JSON_ARRAY_SIZE(1) + 2 * JSON_OBJECT_SIZE(1) + 2 * JSON_OBJECT_SIZE(2) + JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(5) + JSON_OBJECT_SIZE(6) + JSON_OBJECT_SIZE(14);
DynamicJsonDocument doc(capacity);

int water_pump = 5;
int fan = 6;
int led = 7;

void setup()
{
    Serial.begin(115200);
    delay(10);
    pinMode(water_pump, OUTPUT);
    pinMode(fan, OUTPUT);
    pinMode(led, OUTPUT);
}

void loop()
{

    // We start by connecting to a WiFi network
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());

    Serial.print("Connecting to ");
    Serial.println(host);

    // Use WiFiClient class to create TCP connections
    WiFiClientSecure client;
    client.setInsecure(); // Eliminate the need for at certificate fingerprint... but allows an attacker to impersonate the webserver...
    const int httpsPort = 80;
    if (!client.connect(host, httpsPort))
    {
        Serial.println("Connection failed");
        return;
    }

    Serial.print("Requesting URL: ");
    Serial.println(url);

    // This will send the request to the server
    client.println("GET " + url + " HTTP/1.1");
    client.println("Host: " + host);
    client.println("user-agent: curl/7.68.0");
    client.println("accept: */*");
    client.println(); //end of header

    delay(1000); // Wait a while to let the server respond (increase on shitty connections)

    // Read all the lines of the reply from server
    String line;
    while (client.available())
    {

        yield();
        String line = client.readStringUntil('\n'); // Newline return as delimiter
        // Serial.println(line);

        if (line.startsWith("{"))
        {
            deserializeJson(doc, line); // If the current line is the json string we want, parse it to make a json object.
            Serial.println("Found json!");
            Serial.println("\n");
            char *city = doc[0]["name"];
            int temp = doc[0]["main"]["temp"];
            float wind = doc[0]["wind"]["speed"];
            float rain = doc[0]["rain"]["1h"];
            int clouds = doc[0]["clouds"]["all"];

            Serial.println("City: " + String(city));
            Serial.println("Temperature: " + String(temp)) + " celsius";
            Serial.println("Wind speed: " + String(wind)) + " m/s";
            Serial.println("Rain: " + String(rain)) + " mm";
            Serial.println("Clouds: " + String(clouds)) + " %";
        }

        float fan_speed;
    }
    digitalWrite(led, HIGH);
    delay(600000);
}