DO $$
DECLARE
    admin_org_id INTEGER;
    auto_building_id INTEGER;
    auto_room_id INTEGER;
BEGIN
    SELECT organization_id
    INTO admin_org_id
    FROM users
    WHERE email = 'admin@dorm.energy'
       OR username = 'admin'
    ORDER BY
        CASE WHEN email = 'admin@dorm.energy' THEN 0 ELSE 1 END
    LIMIT 1;

    IF admin_org_id IS NULL THEN
        RAISE EXCEPTION 'Admin workspace is required before restoring required device room assignment';
    END IF;

    INSERT INTO buildings (organization_id, name, address, description)
    VALUES (
        admin_org_id,
        'MQTT Auto Devices',
        'MQTT telemetry',
        'Automatically created from MQTT telemetry'
    )
    ON CONFLICT DO NOTHING;

    SELECT id
    INTO auto_building_id
    FROM buildings
    WHERE organization_id = admin_org_id
      AND name = 'MQTT Auto Devices'
    LIMIT 1;

    INSERT INTO rooms (building_id, room_name, room_type, floor_number)
    VALUES (auto_building_id, 'Auto-discovered devices', 'MQTT', 0)
    ON CONFLICT DO NOTHING;

    SELECT id
    INTO auto_room_id
    FROM rooms
    WHERE building_id = auto_building_id
      AND room_name = 'Auto-discovered devices'
    LIMIT 1;

    UPDATE devices
    SET room_id = auto_room_id
    WHERE room_id IS NULL;
END $$;

ALTER TABLE devices
DROP CONSTRAINT IF EXISTS devices_room_id_fkey;

ALTER TABLE devices
ALTER COLUMN room_id SET NOT NULL;

ALTER TABLE devices
ADD CONSTRAINT devices_room_id_fkey
FOREIGN KEY (room_id)
REFERENCES rooms(id)
ON DELETE RESTRICT;
