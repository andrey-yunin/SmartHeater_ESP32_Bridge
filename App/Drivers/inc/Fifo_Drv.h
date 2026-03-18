#ifndef FIFO_DRV_H_
#define FIFO_DRV_H_

#include <stdint.h>
#include <stdbool.h>

/**
 * @brief Универсальный кольцевой буфер (FIFO) в стиле ООП.
 * @tparam SIZE Размер буфера (должен быть степенью двойки: 64, 128, 256...)
 */
template <uint16_t SIZE>

class Fifo_Drv {
    public:
    
    Fifo_Drv() : _head(0), _tail(0) { 
        //это то же самое что и 
        /*
        Fifo_Drv() {
            _head = 0;
            _tail = 0;
        }
           написание конструктора в виде списка инициализации более эффективно, 
           так как позволяет инициализировать члены класса непосредственно при их объявлении, 
           а не сначала создавать их с помощью конструктора по умолчанию, 
           а затем присваивать им значения. 
           Это особенно важно для константных членов и ссылок, 
           которые должны быть инициализированы при объявлении. 
        */
        // Проверка на степень двойки во время компиляции
        static_assert((SIZE != 0) && ((SIZE & (SIZE - 1)) == 0), "FIFO SIZE must be a power of 2!");
        }

    /**
     * @brief Добавить байт в очередь
     */
    bool Push(uint8_t byte) {
        uint16_t next = (_head + 1) & (SIZE - 1);
        if (next != _tail) {
            _buffer[_head] = byte;
            _head = next;
            return true;
        }
        return false; // Буфер полон
    }

    
    /**
     * @brief Извлечь байт из очереди
     */
    bool Pop(uint8_t &byte) {
        if (_head != _tail) {
            byte = _buffer[_tail];
            _tail = (_tail + 1) & (SIZE - 1);
            return true;
        }
        return false; // Буфер пуст
    }

    /**
     * @brief Очистить очередь
     */
    void Flush() {
        _head = _tail = 0;
        }
        
    /**
     * @brief Проверить, пуст ли буфер
     */
    bool isEmpty() const {
        return _head == _tail;
    }

    private:
    uint8_t _buffer[SIZE];
    volatile uint16_t _head;
    volatile uint16_t _tail;
};

#endif // FIFO_DRV_H_
