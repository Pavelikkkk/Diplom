# Dorm Energy Simulator

**Консольное приложение на C++20** для мониторинга и имитации энергопотребления в студенческом общежитии.

**Цель проекта:**
Создать **надёжную, детерминированную и ресурсоэффективную** backend-систему, способную:
- Получать данные от IoT-датчиков 
- Обнаруживать аномалии (пороговые правила + ML-модель)
- Сохранять данные в TimescaleDB
- Отправлять мультиканальные оповещения

## Архитектурные решения (ADR)

Все ключевые решения зафиксированы в формате ADR:

- [ADR-001](./docs/adr/0001-cxx20-and-language-choice.md) — Выбор C++20
- [ADR-002](./docs/adr/0002-cli11-library.md) — Интерфейс командной строки (CLI11)
- [ADR-003](./docs/adr/0003-data-storage.md) — Хранение данных (PostgreSQL + TimescaleDB)
- [ADR-004](./docs/adr/0004-hybrid-anomaly-detection.md) — Гибридная детекция аномалий
- [ADR-005](./docs/adr/0005-synthetic-data-generator.md) — Генератор синтетических данных
- [ADR-006](./docs/adr/0006-mqtt-for-esp32-communication.md) — Протокол взаимодействия с ESP32
- [ADR-007](./docs/adr/0007-telegram-bot-for-notifications.md) — Система оповещения
- [ADR-008](./docs/adr/0008-onnx-runtime-for-ml-inference.md) — Инференс ML-моделей


