DO $$
DECLARE
    admin_org_id INTEGER;
    test_building_id INTEGER;
    room_01_id INTEGER;
    room_02_id INTEGER;
    room_03_id INTEGER;
    room_04_id INTEGER;
    room_05_id INTEGER;
BEGIN
    SELECT organization_id
    INTO admin_org_id
    FROM users
    WHERE email = 'admin@dorm.energy'
       OR username = 'admin'
    ORDER BY
        CASE
            WHEN email = 'admin@dorm.energy' THEN 0
            ELSE 1
        END
    LIMIT 1;

    IF admin_org_id IS NULL THEN
        RAISE NOTICE 'Admin user is missing, skipping ESP32 test devices seed';
        RETURN;
    END IF;

    INSERT INTO buildings
    (
        organization_id,
        name,
        address,
        description
    )
    VALUES
    (
        admin_org_id,
        'ESP32 Test Building',
        'MQTT emulator',
        'Rooms and devices used for manual detector testing'
    )
    ON CONFLICT DO NOTHING;

    SELECT id
    INTO test_building_id
    FROM buildings
    WHERE organization_id = admin_org_id
      AND name = 'ESP32 Test Building'
    LIMIT 1;

    INSERT INTO rooms
    (
        building_id,
        room_name,
        room_type,
        floor_number
    )
    VALUES
        (test_building_id, 'ESP32 Room 01', 'Test', 1),
        (test_building_id, 'ESP32 Room 02', 'Test', 1),
        (test_building_id, 'ESP32 Room 03', 'Test', 1),
        (test_building_id, 'ESP32 Room 04', 'Test', 1),
        (test_building_id, 'ESP32 Room 05', 'Test', 1)
    ON CONFLICT DO NOTHING;

    SELECT id INTO room_01_id FROM rooms WHERE building_id = test_building_id AND room_name = 'ESP32 Room 01' LIMIT 1;
    SELECT id INTO room_02_id FROM rooms WHERE building_id = test_building_id AND room_name = 'ESP32 Room 02' LIMIT 1;
    SELECT id INTO room_03_id FROM rooms WHERE building_id = test_building_id AND room_name = 'ESP32 Room 03' LIMIT 1;
    SELECT id INTO room_04_id FROM rooms WHERE building_id = test_building_id AND room_name = 'ESP32 Room 04' LIMIT 1;
    SELECT id INTO room_05_id FROM rooms WHERE building_id = test_building_id AND room_name = 'ESP32 Room 05' LIMIT 1;

    INSERT INTO devices
    (
        device_id,
        device_name,
        device_model,
        firmware_version,
        room_id,
        is_online,
        last_seen_at
    )
    VALUES
        ('esp32-01', 'ESP32 01', 'ESP32', 'emulator', room_01_id, FALSE, NULL),
        ('esp32-02', 'ESP32 02', 'ESP32', 'emulator', room_02_id, FALSE, NULL),
        ('esp32-03', 'ESP32 03', 'ESP32', 'emulator', room_03_id, FALSE, NULL),
        ('esp32-04', 'ESP32 04', 'ESP32', 'emulator', room_04_id, FALSE, NULL),
        ('esp32-05', 'ESP32 05', 'ESP32', 'emulator', room_05_id, FALSE, NULL)
    ON CONFLICT (device_id)
    DO UPDATE SET
        device_name = EXCLUDED.device_name,
        device_model = EXCLUDED.device_model,
        firmware_version = EXCLUDED.firmware_version,
        room_id = EXCLUDED.room_id;
END $$;
