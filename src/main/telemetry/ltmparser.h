/*
 * LTM Parser
 * This file is part of INAV.
 *
 * INAV is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * INAV is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with INAV.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Parses LTM inbound packet
 *  Created on: 28 11 2019
 *      Author: Anton Rothwell
 */

#pragma once

#define MAX_LTM_PAYLOAD 20
#define PAYLOAD_M_SIZE 15

typedef enum LtmState { WAITING_FOR_START, WAITING_FOR_TAG, WAITING_FOR_TYPE, READING_PAYLOAD, READING_CHECKSUM } LtmState_e;

typedef struct  MFrame_s
{
	uint8_t id;
	int32_t relativeBearing;
	uint16_t gs;
	int32_t alt;
	int32_t distance;
} __attribute__((packed)) MFrame_t;


typedef struct LtmParserState_s
{
    char payloadType ;
    uint8_t payloadLen;
	LtmState_e state ;
	uint8_t payloadCount;
	uint8_t checksum ;
	uint8_t checksumAccum ;
    char payload[MAX_LTM_PAYLOAD];
    MFrame_t wingmanData;
} LtmParserState_t;

void ltmParserInit(LtmParserState_t *state);
bool ltmParserAppend(LtmParserState_t *state, char ch);
bool ltmParserValid(LtmParserState_t *state);