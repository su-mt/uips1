#!/usr/bin/env python3
"""
Простой калькулятор XOR контрольной суммы
Вводите байты в шестнадцатеричном формате
"""

def calculate_xor_checksum(bytes_list):
    """Вычисление XOR контрольной суммы"""
    checksum = 0
    for byte in bytes_list:
        checksum ^= byte
    return checksum


def main():
    print("=" * 60)
    print("Калькулятор XOR контрольной суммы")
    print("=" * 60)
    print("\nВведите байты в HEX формате через пробел")
    print("Примеры:")
    print("  A0 00 81 F1")
    print("  0xA0 0x00 0x81 0xF1")
    print("  a0 00 81 f1")
    print("\nДля выхода введите 'q' или 'quit'\n")
    
    while True:
        try:
            # Ввод от пользователя
            user_input = input("Байты: ").strip()
            
            # Проверка на выход
            if user_input.lower() in ['q', 'quit', 'exit']:
                print("Выход...")
                break
            
            if not user_input:
                print("⚠️  Введите хотя бы один байт!\n")
                continue
            
            # Разделение на токены
            tokens = user_input.split()
            bytes_list = []
            
            # Парсинг байтов
            for token in tokens:
                # Удаление префикса 0x если есть
                token = token.lower().replace('0x', '')
                
                # Преобразование в число
                byte_value = int(token, 16)
                
                # Проверка диапазона
                if byte_value < 0 or byte_value > 255:
                    print(f"⚠️  Ошибка: {token} вне диапазона 0x00-0xFF")
                    raise ValueError
                
                bytes_list.append(byte_value)
            
            # Вычисление контрольной суммы
            checksum = calculate_xor_checksum(bytes_list)
            
            # Вывод результата
            print("\n" + "-" * 60)
            print(f"Байты:         {' '.join([f'0x{b:02X}' for b in bytes_list])}")
            print(f"Количество:    {len(bytes_list)}")
            print(f"Контрольная сумма (XOR): 0x{checksum:02X} ({checksum})")
            
            # Полный пакет (байты + контрольная сумма)
            full_packet = bytes_list + [checksum]
            print(f"Полный пакет:  {' '.join([f'0x{b:02X}' for b in full_packet])}")
            print("-" * 60 + "\n")
            
        except ValueError:
            print("⚠️  Ошибка! Введите корректные HEX значения (0-9, A-F)\n")
        except KeyboardInterrupt:
            print("\n\nВыход...")
            break
        except Exception as e:
            print(f"⚠️  Ошибка: {e}\n")


if __name__ == "__main__":
    main()
