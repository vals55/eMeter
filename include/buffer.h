// Copyright 2023 vals. All rights reserved.
// SPDX-License-Identifier: MIT
#ifndef __BUFFER_H
#define __BUFFER_H

#include <Arduino.h>
#include <EEPROM.h>

template<class T>
class EEPROMBuff {
	// Кольцевой буфер: 
	// blocks - кол-во копий Т (размер кольцевого буфера), затем
    // crc16 - столько же копий crc16 каждого блока

public: 
    explicit EEPROMBuff(const uint16_t _blocks, const uint16_t _start_addr = 0);
    void add(const T &element);
    void erase();
	bool get(T &element);
	bool get_block(const uint16_t block, T &element);
	
	uint16_t size();	
private:

	uint16_t start_addr;
	uint16_t activeBlock;
	uint16_t blocks;

	uint16_t elementSize;
	uint16_t crc_addr;
};

#endif
