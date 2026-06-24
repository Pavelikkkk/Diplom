import argparse
import json
import random
import ssl
import time

import paho.mqtt.client as mqtt

# ДЛЯ ТЕСТИРОВАНИЯ 

BROKER_HOST = "z611af1f.ala.eu-central-1.emqxsl.com"
BROKER_PORT = 8883
USERNAME = "SoSM"
PASSWORD = "1234"

DEVICES = ["esp32-01", "esp32-02", "esp32-03", "esp32-04", "esp32-05"]


def make_payload(device_id, sensor_type, value, unit):
    payload = {
        "deviceId": device_id,
        "sensorType": sensor_type,
        "value": value,
        "unit": unit,
    }

    if sensor_type == "motion":
        payload["boolValue"] = bool(value)

    return payload


def publish(client, device_id, sensor_type, value, unit):
    topic = f"devices/{device_id}/{sensor_type}"
    payload = make_payload(device_id, sensor_type, value, unit)
    encoded = json.dumps(payload, separators=(",", ":"))
    result = client.publish(topic, encoded, qos=0)
    result.wait_for_publish()
    print(f"OK: {topic} {encoded}")


def send_normal_frame(client):
    for device_id in DEVICES:
        publish(client, device_id, "motion", random.randint(0, 1), "bool")
        publish(client, device_id, "light", random.randint(250, 750), "lux")
        publish(client, device_id, "power", round(random.uniform(0.35, 0.95), 2), "kW")



def unattended_power(client, long_window_seconds):

    # Логика сценария: 
    # система видит, что в комнате долго нет движения,
    # но энергопотребление остаётся высоким. Это может означать,
    # что мощный прибор оставили включённым после ухода человека.

    print("\nScenario: rule_unattended_power_usage")

    publish(client, "esp32-01", "motion", 0, "bool")
    publish(client, "esp32-01", "power", 2.8, "kW")

    time.sleep(long_window_seconds)

    publish(client, "esp32-01", "motion", 0, "bool")
    publish(client, "esp32-01", "power", 2.9, "kW")


def sustained_high_power(client, long_window_seconds):

    # Логика сценария: 
    # система видит, что мощность долго остаётся высокой
    # без заметного снижения. Такая ситуация может быть подозрительной,
    # если обычно мощные приборы работают короткими циклами,
    # а не держат нагрузку постоянно.

    print("\nScenario: rule_sustained_high_power")

    publish(client, "esp32-02", "power", 3.0, "kW")

    time.sleep(long_window_seconds)

    publish(client, "esp32-02", "power", 3.1, "kW")


def repeated_spikes(client):

    # Логика сценария: 
    # система получает серию частых скачков мощности
    # за короткий промежуток времени. Отдельный скачок может быть нормальным,
    # но повторяющиеся резкие подъёмы и падения могут указывать
    # на нестабильную нагрузку или неисправное устройство.

    print("\nScenario: rule_repeated_power_spikes")

    for value in [0.2, 2.7, 0.3, 2.9, 0.2, 3.1, 0.4, 3.0]:
        publish(client, "esp32-03", "power", value, "kW")
        time.sleep(1)


def baseline_spike(client):
    # Логика сценария: 
    # система сначала наблюдает обычное стабильное
    # потребление устройства, а затем получает значение, которое намного выше
    # его нормального уровня. Аномалия определяется не абсолютной мощностью,
    # а отклонением от baseline конкретного устройства.

    print("\nScenario: rule_baseline_power_spike")

    for _ in range(20):
        publish(client, "esp32-04", "power", 0.4, "kW")
        time.sleep(1)

    publish(client, "esp32-04", "power", 2.4, "kW")


def sudden_spike(client):

    # Логика сценария: 
    # система видит резкий скачок мощности за одну секунду.
    # Важен не сам высокий уровень, а скорость изменения:
    # нагрузка почти мгновенно выросла с низкой до очень высокой.

    print("\nScenario: rule_sudden_power_spike")

    publish(client, "esp32-05", "power", 0.3, "kW")
    time.sleep(1)
    publish(client, "esp32-05", "power", 4.2, "kW")


def ml_candidate(client):

    # Логика сценария: система получает комбинацию признаков,
    # которая сама по себе не обязана нарушать простое правило,
    # но выглядит нетипично относительно обычного поведения.
    # Например: долго нет движения, света почти нет,
    # но мощность остаётся средней или высокой.
    
    print("\nScenario: ml_autoencoder_candidate")

    publish(client, "esp32-06", "motion", 0, "bool")
    publish(client, "esp32-06", "light", 2, "lux")
    publish(client, "esp32-06", "power", 1.9, "kW")

def create_client():
    client = mqtt.Client(
        mqtt.CallbackAPIVersion.VERSION2,
        client_id=f"dorm-energy-python-simulator-{random.randint(1000, 9999)}",
        protocol=mqtt.MQTTv5,
    )
    client.username_pw_set(USERNAME, PASSWORD)
    client.tls_set(cert_reqs=ssl.CERT_NONE)
    client.tls_insecure_set(True)
    return client


def main():

    ONCE = False # если True: отправить один normal-фрейм и выйти
    NORMAL_ONLY = False # если True: бесконечно отправлять только normal-фреймы
    NORMAL_INTERVAL = 5 # задержка между normal-фреймами 
    CYCLE_COOLDOWN = 60 # пауза после каждого сценария аномалии 
    LONG_WINDOW = 70 # длительность окна для долгих сценариев 

    client = create_client()
    print(f"Connecting to {BROKER_HOST}:{BROKER_PORT}...")
    client.connect(BROKER_HOST, BROKER_PORT, keepalive=60)
    client.loop_start()

    try:
        cycle = 0

        while True:
            cycle += 1
            print(f"\n=== MQTT stability cycle #{cycle} ===")
            send_normal_frame(client)

            if ONCE:
                break

            if NORMAL_ONLY:
                time.sleep(NORMAL_INTERVAL)
                continue

            time.sleep(NORMAL_INTERVAL)

            scenario = cycle % 6 # количество сценариев 
            if scenario == 1:
                unattended_power(client, LONG_WINDOW)
            elif scenario == 2:
                sustained_high_power(client, LONG_WINDOW)
            elif scenario == 3:
                repeated_spikes(client)
            elif scenario == 4:
                baseline_spike(client)
            elif scenario == 5:
                sudden_spike(client)
            else:
                ml_candidate(client)

            print(f"Cooling down for {CYCLE_COOLDOWN} seconds...")
            time.sleep(CYCLE_COOLDOWN)

    except KeyboardInterrupt:
        print("\nStopping simulator...")
    finally:
        client.loop_stop()
        client.disconnect()


if __name__ == "__main__":
    main()