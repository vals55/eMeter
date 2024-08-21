// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#include "buffer.h"
#include "utils.h"
#include "config.h"
#include "rlog.h"

template<class T>
EEPROMBuff<T>::EEPROMBuff(const uint16_t _blocks, const uint16_t _start_addr) : blocks(_blocks), start_addr(_start_addr), activeBlock(0) {
    uint16_t crc = 0;
    elementSize = sizeof(T);
    crc_addr = start_addr + elementSize * blocks + 1;

    T tmp;
    EEPROM.begin(4096);
    for (int i = 0; i < blocks; i++) {
        crc = EEPROM.get(crc_addr, crc);
        if (crc > 0) {
            EEPROM.end();
            activeBlock = i;
            if (get(tmp))
                return;
        }
    }

    erase();
}

template<class T>
void EEPROMBuff<T>::erase() {
    
    EEPROM.begin(4096);
    activeBlock = 0;
    for (uint16_t i = start_addr; i < crc_addr + blocks * 2; i++) {
        EEPROM.write(i, 0);
    }
    EEPROM.commit();
    EEPROM.end();
}

template<class T>
void EEPROMBuff<T>::add(const T &element) {
    uint16_t prev = activeBlock;
    activeBlock = (activeBlock < blocks - 1) ? activeBlock + 1 : 0;

    uint16_t crc = getCRC(element);
    EEPROM.begin(4096);
    EEPROM.put(start_addr + activeBlock * elementSize, element);
    EEPROM.put(crc_addr + prev * sizeof(crc), 0);
    EEPROM.put(crc_addr + activeBlock * sizeof(crc), crc);
    EEPROM.commit();
    EEPROM.end();
}

template<class T>
bool EEPROMBuff<T>::get(T &element) {
    return get_block(activeBlock, element);
}

template<class T>
bool EEPROMBuff<T>::get_block(const uint16_t block, T &element) {
    T tmp;
    uint16_t crc = 0;

    EEPROM.begin(4096);
    EEPROM.get(start_addr + block * elementSize, tmp);
    EEPROM.get(crc_addr + block * sizeof(crc), crc);
    uint16_t calc = getCRC(tmp);
    if (calc == crc) {
        element = tmp;
        return true;
    }
    return false;
}

template<class T>
uint16_t EEPROMBuff<T>::size() {
    return blocks;
}

template class EEPROMBuff<BoardConfig>;
