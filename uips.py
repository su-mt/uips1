#!/usr/bin/env python3
import serial
import threading
import sys
import time

# Настройки порта
PORT = '/dev/cu.usbmodem1403'
BAUDRATE = 115200

# Флаг для остановки потока чтения
stop_reading = threading.Event()

def read_serial(ser):
    """Поток для чтения данных из serial порта"""
    while not stop_reading.is_set():
        if ser.in_waiting > 0:
            try:
                data = ser.read(ser.in_waiting)
                # Вывод в hex формате
                hex_str = ' '.join(f'{b:02X}' for b in data)
                print(f"\n<< HEX: {hex_str}")
                
                # Попытка вывести как текст
                try:
                    text = data.decode('utf-8', errors='ignore')
                    if text.strip():
                        print(f"<< TEXT: {text}")
                except:
                    pass
                
                print(">> ", end='', flush=True)
            except Exception as e:
                print(f"\nError reading: {e}")
        time.sleep(0.01)

def main():
    try:
        # Открыть serial порт
        ser = serial.Serial(PORT, BAUDRATE, timeout=1)
        print(f"Connected to {PORT} at {BAUDRATE} baud")
        print("Enter hex bytes (e.g., 'FA 00 81 00' or 'FA008100')")
        print("Or enter text directly. Press Ctrl+C to exit.\n")
        
        # Запустить поток чтения
        reader_thread = threading.Thread(target=read_serial, args=(ser,), daemon=True)
        reader_thread.start()
        
        while True:
            try:
                # Получить ввод от пользователя
                user_input = input(">> ").strip()
                
                if not user_input:
                    continue
                
                # Временно остановить чтение для чистого вывода
                stop_reading.set()
                time.sleep(0.05)
                stop_reading.clear()
                reader_thread = threading.Thread(target=read_serial, args=(ser,), daemon=True)
                reader_thread.start()
                
                # Попробовать распарсить как hex
                try:
                    # Убрать пробелы и разделители
                    hex_str = user_input.replace(' ', '').replace('0x', '').replace('\\x', '')
                    
                    # Проверить, является ли hex строкой
                    if all(c in '0123456789ABCDEFabcdef' for c in hex_str):
                        # Парсить как hex
                        data = bytes.fromhex(hex_str)
                        ser.write(data)
                        print(f">> Sent HEX: {' '.join(f'{b:02X}' for b in data)}")
                    else:
                        # Отправить как текст
                        data = user_input.encode('utf-8')
                        ser.write(data)
                        print(f">> Sent TEXT: {user_input}")
                
                except ValueError:
                    # Если не hex, отправить как текст
                    data = user_input.encode('utf-8')
                    ser.write(data)
                    print(f">> Sent TEXT: {user_input}")
                
            except KeyboardInterrupt:
                print("\n\nExiting...")
                break
            except Exception as e:
                print(f"\nError: {e}")
        
        # Закрыть соединение
        stop_reading.set()
        ser.close()
        print("Connection closed.")
        
    except serial.SerialException as e:
        print(f"Error opening serial port: {e}")
        print("\nTips:")
        print("1. Check if the device is connected")
        print("2. Close any other programs using the port")
        print("3. Try: lsof | grep /dev/cu.usbmodem21403")
        sys.exit(1)

if __name__ == "__main__":
    main()