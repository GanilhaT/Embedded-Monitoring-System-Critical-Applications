/**
 * @file ppp_misc.h
 * @brief PPP miscellaneous functions
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2010-2023 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneTCP Open.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 * @author Oryx Embedded SARL (www.oryx-embedded.com)
 * @version 2.2.4
 **/

#ifndef _PPP_MISC_H
#define _PPP_MISC_H

//Dependencies
#include "core/net.h"
#include "ppp/ppp.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

//PPP related functions
error_list pppSendConfigureAckNak(PppContext *context,
   const PppConfigurePacket *configureReqPacket, PppProtocol protocol, PppCode code);

error_list pppSendTerminateReq(PppContext *context,
   uint8_t identifier, PppProtocol protocol);

error_list pppSendTerminateAck(PppContext *context,
   uint8_t identifier, PppProtocol protocol);

error_list pppSendCodeRej(PppContext *context, const PppPacket *packet,
   uint8_t identifier, PppProtocol protocol);

error_list pppSendProtocolRej(PppContext *context, uint8_t identifier,
   uint16_t protocol, const uint8_t *information, size_t length);

error_list pppSendEchoRep(PppContext *context,
   const PppEchoPacket *echoReqPacket, PppProtocol protocol);

error_list pppAddOption(PppConfigurePacket *packet, uint8_t optionType,
   const void *optionValue, uint8_t optionLen);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
