#!/usr/bin/env python3
"""
Скрипт для тестирования команд управления STM32
Протокол: 5 байт [Адрес, 0x00, 0x81, Команда, КС]
"""

import serial
import time
import sys
import glob


def calculate_checksum(data):
    """Вычисление контрольной суммы (XOR первых 4 байт)"""
    checksum = 0
    for byte in data[:4]:
        checksum ^= byte
    return checksum


def create_command(cmd_code):
    """
    Создание команды по протоколу
    Формат: [0xA0, 0x00, 0x81, cmd_code, checksum]
    """
    packet = [0x80, 0x00, 0x81, cmd_code]
    checksum = calculate_checksum(packet)
    packet.append(checksum)
    return bytes(packet)


def find_stm32_port():
    """Поиск порта STM32"""
    # Для macOS
    ports = glob.glob('/dev/tty.usbmodem*')
    if not ports:
        # Для Linux
        ports = glob.glob('/dev/ttyACM*')
    if not ports:
        # Для Windows
        ports = glob.glob('COM*')
    
    if ports:
        return ports[0]
    return None


def send_command(ser, cmd_code, cmd_name):
    """Отправка команды и ожидание ответа"""
    packet = create_command(cmd_code)
    
    print(f"\n{'='*60}")
    print(f"Отправка команды: {cmd_name}")
    print(f"Пакет: {' '.join([f'0x{b:02X}' for b in packet])}")
    
    ser.write(packet)
    time.sleep(0.2)  # Ждем обработки
    
    # Чтение ответа
    if ser.in_waiting > 0:
        response = ser.read(ser.in_waiting)
        print(f"Ответ: {response.decode('utf-8', errors='ignore')}")
    else:
        print("Ответ: (нет данных)")
    print(f"{'='*60}")


def send_invalid_command(ser):
    """Отправка неподходящей команды"""
    # Неправильный адрес
    invalid_packet = bytes([0xFF, 0x00, 0x81, 0xF1, 0x00])
    
    print(f"\n{'='*60}")
    print(f"Отправка НЕПРАВИЛЬНОЙ команды (неверный адрес)")
    print(f"Пакет: {' '.join([f'0x{b:02X}' for b in invalid_packet])}")
    
    ser.write(invalid_packet)
    time.sleep(0.2)
    
    if ser.in_waiting > 0:
        response = ser.read(ser.in_waiting)
        print(f"Ответ: {response.decode('utf-8', errors='ignore')}")
    else:
        print("Ответ: (нет данных - команда отклонена)")
    print(f"{'='*60}")


def main():
    # Поиск порта
    port = find_stm32_port()
    
    if not port:
        print("❌ Устройство STM32 не найдено!")
        print("Укажите порт вручную:")
        port = input("Введите имя порта (например, /dev/tty.usbmodem14203): ")
    
    print(f"📡 Подключение к порту: {port}")
    
    try:
        # Открытие Serial порта
        ser = serial.Serial(
            port=port,
            baudrate=115200,
            bytesize=serial.EIGHTBITS,
            parity=serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=1
        )
        
        print(f"✅ Порт {port} открыт успешно")
        print(f"Скорость: {ser.baudrate} бод")
        
        time.sleep(2)  # Ждем инициализацию
        
        # Очистка буфера
        ser.reset_input_buffer()
        ser.reset_output_buffer()
        
        # Отправка команд
        commands = [
            (0xF1, "0xF1 - Старт"),
            (0xF2, "0xF2 - Стоп"),
            (0xF3, "0xF3 - Выдать ток"),
            (0xF4, "0xF4 - Выдать сопротивление изоляции"),
            (0xF5, "0xF5 - Выдать константы"),
        ]
        
        for cmd_code, cmd_name in commands:
            send_command(ser, cmd_code, cmd_name)
            time.sleep(0.5)
        
        # Неправильная команда
        send_invalid_command(ser)
        
        print(f"\n✅ Все команды отправлены!")
        
        # Закрытие порта
        ser.close()
        print(f"📴 Порт {port} закрыт")
        
    except serial.SerialException as e:
        print(f"❌ Ошибка работы с портом: {e}")
        sys.exit(1)
    except KeyboardInterrupt:
        print("\n⚠️  Прервано пользователем")
        if 'ser' in locals() and ser.is_open:
            ser.close()
        sys.exit(0)


if __name__ == "__main__":
    main()
