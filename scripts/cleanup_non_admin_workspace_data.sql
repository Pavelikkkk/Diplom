DO $$
DECLARE
    admin_org_id INTEGER;
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
        RAISE EXCEPTION 'Admin workspace not found; cleanup aborted';
    END IF;

    DELETE FROM anomalies
    WHERE device_id IN (
        SELECT d.device_id
        FROM devices d
        JOIN rooms r
            ON r.id = d.room_id
        JOIN buildings b
            ON b.id = r.building_id
        WHERE b.organization_id <> admin_org_id
    );

    DELETE FROM sensor_readings
    WHERE device_id IN (
        SELECT d.device_id
        FROM devices d
        JOIN rooms r
            ON r.id = d.room_id
        JOIN buildings b
            ON b.id = r.building_id
        WHERE b.organization_id <> admin_org_id
    );

    DELETE FROM devices
    WHERE room_id IN (
        SELECT r.id
        FROM rooms r
        JOIN buildings b
            ON b.id = r.building_id
        WHERE b.organization_id <> admin_org_id
    );

    DELETE FROM rooms
    WHERE building_id IN (
        SELECT id
        FROM buildings
        WHERE organization_id <> admin_org_id
    );

    DELETE FROM buildings
    WHERE organization_id <> admin_org_id;
END $$;
