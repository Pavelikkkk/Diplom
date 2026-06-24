ALTER TABLE devices
DROP CONSTRAINT IF EXISTS devices_room_id_fkey;

ALTER TABLE sensor_readings
DROP CONSTRAINT IF EXISTS fk_sensor_readings_device;

ALTER TABLE anomalies
DROP CONSTRAINT IF EXISTS fk_anomalies_device;

ALTER TABLE devices
ALTER COLUMN room_id DROP NOT NULL;

ALTER TABLE devices
ADD CONSTRAINT devices_room_id_fkey
FOREIGN KEY (room_id)
REFERENCES rooms(id)
ON DELETE SET NULL;

ALTER TABLE sensor_readings
ADD CONSTRAINT fk_sensor_readings_device
FOREIGN KEY (device_id)
REFERENCES devices(device_id)
ON DELETE CASCADE;

ALTER TABLE anomalies
ADD CONSTRAINT fk_anomalies_device
FOREIGN KEY (device_id)
REFERENCES devices(device_id)
ON DELETE CASCADE;
