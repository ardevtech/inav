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

#include <stdbool.h>
#include <string.h>
#include "platform.h"

#include "ltmparser.h"

// Local Functions
void parseFrame(LtmParserState_t *state)
{

    if (state != NULL)
    {
        switch (state->payloadType)
        {
        case 'M':
            memcpy(&state->wingmanData, state->payload, sizeof(MFrame_t));
            break;
        }
    }
}

// Accessible Functions
void ltmParserInit(LtmParserState_t *state)
{
    if (state != NULL)
    {
        state->payloadType = 0;
        state->state = WAITING_FOR_START;
        state->payloadLen = 0;
        memset(state->payload,0,MAX_LTM_PAYLOAD);
        state->payloadCount = 0;
        state->checksum = 0;
        state->checksumAccum = 0;
        memset(&state->wingmanData,0,sizeof(MFrame_t));
    }
}

// Append to buffer and execute state machine
bool ltmParserAppend(LtmParserState_t *state, char ch)
{
    bool finishedFrame = false;

	switch (state->state)
	{
	case WAITING_FOR_START:
		if (ch == '$')
		{
			state->state = WAITING_FOR_TAG;
		}
		break;
	case WAITING_FOR_TAG:
		if (ch == 'T')
		{
			state->state = WAITING_FOR_TYPE;
		}
		break;
	case WAITING_FOR_TYPE:
		switch (ch)
		{
		case 'M':
			state->payloadLen = PAYLOAD_M_SIZE;
			break;
		
		}
		
        state->payloadType = ch;
		state->state = READING_PAYLOAD;
		state->payloadCount = 0;
		state->checksum = 0;
		state->checksumAccum = 0;
		memset(state->payload, 0, MAX_LTM_PAYLOAD);

		break;
	case READING_PAYLOAD:
		state->payload[state->payloadCount] = ch;
		state->checksumAccum ^= ch;

		state->payloadCount++;
		if (state->payloadCount >= state->payloadLen)
		{
			state->state = READING_CHECKSUM;
		}
		break;
	case READING_CHECKSUM:
		state->checksum = ch;
		state->state = WAITING_FOR_START;
		finishedFrame = true;
		parseFrame(state);
		break;
	default:
		state->state = WAITING_FOR_START;
	}

	return finishedFrame;    
}

bool ltmParserValid(LtmParserState_t *state)
{
	if (state != NULL)
	{
		return state->checksum == state->checksumAccum;
	}else
	{
		return false;
	}
	
}
