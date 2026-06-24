#include <WiFi.h>
#include <PubSubClient.h>
#include <WiFiClientSecure.h>

#define PIR_PIN 26

#define SECONDS(s) ((s) * 1000UL)
#define MINUTES(m) SECONDS((m) * 60UL)
#define HOURS(h) MINUTES((h) * 60UL)

const unsigned long LIGHT_INTERVAL = MINUTES(5);
const unsigned long POWER_INTERVAL = SECONDS(15);

const char *ssid = "jof";
const char *password = "336123061";
const char *mqtt_server = "z611af1f.ala.eu-central-1.emqxsl.com";

const char *mqtt_user = "SoSM";
const char *mqtt_password = "1234";

// Создаёт защищённый TCP-клиент для подключения через TLS
WiFiClientSecure espClient;

// Создаёт MQTT-клиент, который использует espClient как транспортный уровень.
// Через этот объект выполняется подключение к брокеру, публикация сообщений
// и подписка на MQTT-топики.
PubSubClient client(espClient);

bool lastMotionState = false;
String deviceId;
String roomName = "myRealRoom" 

unsigned long lastLightSend = 0;
unsigned long lastPowerSend = 0;

const unsigned long LIGHT_INTERVAL = MINUTES(5);
const unsigned long POWER_INTERVAL = SECONDS(15);

const float VOLTAGE = 220.0;

String buildDeviceId()
{
    String mac = WiFi.macAddress();
    mac.replace(":", "");
    mac.toLowerCase();
    return "esp32-" + mac;
}

String sensorTopic(const char *sensorType)
{
    return "devices/" + deviceId + "-" + String(roomName) + "/" + String(sensorType);
}

String sensorPayload(const char *sensorType, const String &value, const char *unit)
{
    String payload = "{\"deviceId\":\"" + deviceId + "\","
                                                     "\"sensorType\":\"" +
                     String(sensorType) + "\","
                                          "\"value\":" +
                     value;

    if (unit != nullptr && strlen(unit) > 0)
    {
        payload += ",\"unit\":\"" + String(unit) + "\"";
    }

    payload += "}";
    return payload;
}

void connectWiFi()
{
    if (WiFi.status() == WL_CONNECTED)
        return;

    Serial.println("Connecting WiFi...");

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }

    Serial.println();
    Serial.println("WiFi CONNECTED");

    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

void reconnectMQTT()
{
    while (!client.connected())
    {
        Serial.println("Connecting MQTT...");

        if (client.connect(
                deviceId.c_str(),
                mqtt_user,
                mqtt_password))
        {
            Serial.println("MQTT CONNECTED");
        }
        else
        {
            Serial.print("MQTT FAILED rc=");
            Serial.println(client.state());

            delay(3000);
        }
    }
}

void sendMotion(bool motion)
{
    String payload = sensorPayload("motion", String(motion ? 1 : 0), "");
    String topic = sensorTopic("motion");

    bool ok =
        client.publish(
            topic.c_str(),
            payload.c_str());

    // Serial.print("Motion ");
    // Serial.print(motion ? "TRUE" : "FALSE");
    // Serial.print(" -> ");
    // Serial.println(ok ? "MQTT OK" : "MQTT FAILED");
}

void sendLight()
{
    String payload = sensorPayload("light", "700", "lux");
    String topic = sensorTopic("light");

    bool ok =
        client.publish(
            topic.c_str(),
            payload.c_str());

    // Serial.print("Light 700 lux -> ");
    // Serial.println(ok ? "MQTT OK" : "MQTT FAILED");
}

void sendPower()
{
    float currentAmps = random(100, 2000) / 1000.0;  // 0.1 - 2.0 А
    float powerKw = (VOLTAGE * currentAmps) / 1000.0;
    
    String payload = sensorPayload("power", String(powerKw, 2), "kW");
    String topic = sensorTopic("power");

    bool ok = client.publish(topic.c_str(), payload.c_str());

    // Serial.print("Power ");
    // Serial.print(powerKw, 2);
    // Serial.print(" kW (");
    // Serial.print(currentAmps, 2);
    // Serial.print(" A @ ");
    // Serial.print(VOLTAGE);
    // Serial.print("V) -> ");
    // Serial.println(ok ? "MQTT OK" : "MQTT FAILED");
}

void setup()
{
    Serial.begin(115200);

    pinMode(PIR_PIN, INPUT);

    connectWiFi();
    deviceId = buildDeviceId();

    Serial.print("Device ID: ");
    Serial.println(deviceId);

    espClient.setInsecure(); // только для тестов
    client.setServer(mqtt_server, 8883);

    Serial.println("PIR calibration...");
    delay(30000);

    reconnectMQTT();

    lastMotionState = digitalRead(PIR_PIN);

    sendMotion(lastMotionState);

    sendLight();
    sendPower();

    lastLightSend = millis();
    lastPowerSend = millis();

    Serial.println("System started");
}

void loop()
{
    if (WiFi.status() != WL_CONNECTED)
    {
        connectWiFi();
    }

    if (!client.connected())
    {
        reconnectMQTT();
    }

    client.loop();

    bool motion = digitalRead(PIR_PIN);

    if (motion != lastMotionState)
    {
        sendMotion(motion);
        lastMotionState = motion;
    }

    unsigned long now = millis();

    if (now - lastLightSend >= LIGHT_INTERVAL)
    {
        sendLight();
        lastLightSend = now;
    }

    if (now - lastPowerSend >= POWER_INTERVAL)
    {
        sendPower();
        lastPowerSend = now;
    }

    delay(100);
}
