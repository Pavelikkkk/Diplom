DO $$
DECLARE
    admin_user_id INTEGER;
    admin_org_id INTEGER;
    north_building_id INTEGER;
    south_building_id INTEGER;
    room_lobby_id INTEGER;
    room_lab_id INTEGER;
    room_kitchen_id INTEGER;
    room_server_id INTEGER;
BEGIN
    SELECT
        id,
        organization_id
    INTO
        admin_user_id,
        admin_org_id
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
        INSERT INTO organizations
        (
            name,
            type
        )
        VALUES
        (
            'Dorm Energy Admin Workspace',
            'BUSINESS'
        )
        RETURNING id INTO admin_org_id;
    END IF;

    IF admin_user_id IS NULL THEN
        INSERT INTO users
        (
            username,
            email,
            password_hash,
            role,
            organization_id,
            account_type,
            telegram_chat_id
        )
        VALUES
        (
            'admin',
            'admin@dorm.energy',
            '$2b$10$D21nxYuXmLPONxmbYD1uCeu9jKqQBVtkPVuBHywwQO6F/uePShACe',
            'ADMIN',
            admin_org_id,
            'BUSINESS',
            '1179398623'
        )
        RETURNING id INTO admin_user_id;
    ELSE
        UPDATE users
        SET
            username = 'admin',
            email = 'admin@dorm.energy',
            password_hash = '$2b$10$D21nxYuXmLPONxmbYD1uCeu9jKqQBVtkPVuBHywwQO6F/uePShACe',
            role = 'ADMIN',
            organization_id = admin_org_id,
            account_type = 'BUSINESS',
            telegram_chat_id = COALESCE(telegram_chat_id, '1179398623')
        WHERE id = admin_user_id;
    END IF;

    INSERT INTO subscriptions
    (
        organization_id,
        user_id,
        plan,
        status,
        max_buildings,
        max_rooms,
        max_devices
    )
    SELECT
        admin_org_id,
        admin_user_id,
        'ENTERPRISE',
        'ACTIVE',
        50,
        500,
        2500
    WHERE NOT EXISTS (
        SELECT 1
        FROM subscriptions
        WHERE user_id = admin_user_id
    );

    INSERT INTO buildings
    (
        name,
        address,
        description,
        organization_id
    )
    SELECT
        'North Dormitory',
        '12 Campus Avenue',
        'Main residential building with shared study and kitchen areas.',
        admin_org_id
    WHERE NOT EXISTS (
        SELECT 1
        FROM buildings
        WHERE organization_id = admin_org_id
          AND name = 'North Dormitory'
    );

    INSERT INTO buildings
    (
        name,
        address,
        description,
        organization_id
    )
    SELECT
        'South Dormitory',
        '24 Campus Avenue',
        'Residential block with server room and engineering spaces.',
        admin_org_id
    WHERE NOT EXISTS (
        SELECT 1
        FROM buildings
        WHERE organization_id = admin_org_id
          AND name = 'South Dormitory'
    );

    SELECT id INTO north_building_id
    FROM buildings
    WHERE organization_id = admin_org_id
      AND name = 'North Dormitory';

    SELECT id INTO south_building_id
    FROM buildings
    WHERE organization_id = admin_org_id
      AND name = 'South Dormitory';

    INSERT INTO rooms
    (
        room_name,
        room_type,
        floor_number,
        building_id
    )
    SELECT 'Lobby', 'COMMON', 1, north_building_id
    WHERE NOT EXISTS (
        SELECT 1 FROM rooms
        WHERE building_id = north_building_id
          AND room_name = 'Lobby'
    );

    INSERT INTO rooms
    (
        room_name,
        room_type,
        floor_number,
        building_id
    )
    SELECT 'Study Lab', 'STUDY', 2, north_building_id
    WHERE NOT EXISTS (
        SELECT 1 FROM rooms
        WHERE building_id = north_building_id
          AND room_name = 'Study Lab'
    );

    INSERT INTO rooms
    (
        room_name,
        room_type,
        floor_number,
        building_id
    )
    SELECT 'Shared Kitchen', 'KITCHEN', 1, south_building_id
    WHERE NOT EXISTS (
        SELECT 1 FROM rooms
        WHERE building_id = south_building_id
          AND room_name = 'Shared Kitchen'
    );

    INSERT INTO rooms
    (
        room_name,
        room_type,
        floor_number,
        building_id
    )
    SELECT 'Server Room', 'UTILITY', 0, south_building_id
    WHERE NOT EXISTS (
        SELECT 1 FROM rooms
        WHERE building_id = south_building_id
          AND room_name = 'Server Room'
    );

    SELECT id INTO room_lobby_id
    FROM rooms
    WHERE building_id = north_building_id
      AND room_name = 'Lobby';

    SELECT id INTO room_lab_id
    FROM rooms
    WHERE building_id = north_building_id
      AND room_name = 'Study Lab';

    SELECT id INTO room_kitchen_id
    FROM rooms
    WHERE building_id = south_building_id
      AND room_name = 'Shared Kitchen';

    SELECT id INTO room_server_id
    FROM rooms
    WHERE building_id = south_building_id
      AND room_name = 'Server Room';

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
        ('admin-lobby-power-01', 'Lobby Power Meter', 'ESP32-PWR-v2', '2.1.0', room_lobby_id, TRUE, NOW() - INTERVAL '2 minutes'),
        ('admin-lab-motion-01', 'Study Lab Motion Sensor', 'ESP32-MOT-v1', '1.8.4', room_lab_id, TRUE, NOW() - INTERVAL '5 minutes'),
        ('admin-kitchen-env-01', 'Kitchen Environment Sensor', 'ESP32-ENV-v3', '3.0.2', room_kitchen_id, TRUE, NOW() - INTERVAL '1 minute'),
        ('admin-server-power-01', 'Server Room Power Meter', 'ESP32-PWR-v2', '2.1.0', room_server_id, TRUE, NOW() - INTERVAL '30 seconds')
    ON CONFLICT (device_id)
    DO UPDATE SET
        device_name = EXCLUDED.device_name,
        device_model = EXCLUDED.device_model,
        firmware_version = EXCLUDED.firmware_version,
        room_id = EXCLUDED.room_id,
        is_online = EXCLUDED.is_online,
        last_seen_at = EXCLUDED.last_seen_at;

    DELETE FROM anomalies
    WHERE device_id IN (
        'admin-lobby-power-01',
        'admin-lab-motion-01',
        'admin-kitchen-env-01',
        'admin-server-power-01'
    );

    INSERT INTO sensor_readings
    (
        recorded_at,
        device_id,
        sensor_type,
        numeric_value,
        bool_value,
        unit
    )
    SELECT
        NOW() - (point_index || ' hours')::INTERVAL,
        'admin-lobby-power-01',
        'power',
        95 + point_index * 4,
        NULL,
        'W'
    FROM generate_series(0, 23) AS point_index
    ON CONFLICT (recorded_at, device_id, sensor_type)
    DO NOTHING;

    INSERT INTO sensor_readings
    (
        recorded_at,
        device_id,
        sensor_type,
        numeric_value,
        bool_value,
        unit
    )
    SELECT
        NOW() - (point_index || ' hours')::INTERVAL,
        'admin-server-power-01',
        'power',
        220 + point_index * 3,
        NULL,
        'W'
    FROM generate_series(0, 23) AS point_index
    ON CONFLICT (recorded_at, device_id, sensor_type)
    DO NOTHING;

    INSERT INTO sensor_readings
    (
        recorded_at,
        device_id,
        sensor_type,
        numeric_value,
        bool_value,
        unit
    )
    VALUES
        (NOW() - INTERVAL '15 minutes', 'admin-lab-motion-01', 'motion', NULL, TRUE, NULL),
        (NOW() - INTERVAL '10 minutes', 'admin-kitchen-env-01', 'temperature', 27.4, NULL, 'C'),
        (NOW() - INTERVAL '10 minutes', 'admin-kitchen-env-01', 'humidity', 64.0, NULL, '%')
    ON CONFLICT (recorded_at, device_id, sensor_type)
    DO NOTHING;

    INSERT INTO anomalies
    (
        recorded_at,
        device_id,
        sensor_type,
        numeric_value,
        bool_value,
        unit,
        anomaly_type,
        severity,
        score,
        description,
        detected_at
    )
    VALUES
        (
            NOW() - INTERVAL '45 minutes',
            'admin-server-power-01',
            'power',
            410.0,
            NULL,
            'W',
            'Power Spike',
            'CRITICAL',
            0.92,
            'Server room power consumption exceeded the expected range.',
            NOW() - INTERVAL '44 minutes'
        ),
        (
            NOW() - INTERVAL '25 minutes',
            'admin-lobby-power-01',
            'power',
            180.0,
            NULL,
            'W',
            'Power Without Motion',
            'WARNING',
            0.71,
            'Lobby power stayed high while no motion was detected.',
            NOW() - INTERVAL '24 minutes'
        ),
        (
            NOW() - INTERVAL '8 minutes',
            'admin-kitchen-env-01',
            'temperature',
            31.2,
            NULL,
            'C',
            'Temperature Drift',
            'INFO',
            0.58,
            'Kitchen temperature is above the normal evening profile.',
            NOW() - INTERVAL '7 minutes'
        );
END $$;
