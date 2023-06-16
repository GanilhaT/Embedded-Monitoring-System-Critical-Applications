/**
 * @file lcp.h
 * @brief LCP (PPP Link Control Protocol)
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

#ifndef _LCP_H
#define _LCP_H

//Dependencies
#include "core/net.h"
#include "ppp/ppp.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief LCP option types
 **/

typedef enum
{
   LCP_OPTION_MRU              = 1, ///<Maximum-Receive-Unit
   LCP_OPTION_ACCM             = 2, ///<Async-Control-Character-Map
   LCP_OPTION_AUTH_PROTOCOL    = 3, ///<Authentication-Protocol
   LCP_OPTION_QUALITY_PROTOCOL = 4, ///<Quality-Protocol
   LCP_OPTION_MAGIC_NUMBER     = 5, ///<Magic-Number
   LCP_OPTION_PFC              = 7, ///<Protocol-Field-Compression
   LCP_OPTION_ACFC             = 8  ///<Address-and-Control-Field-Compression
} LcpOptionType;


//CodeWarrior or Win32 compiler?
#if defined(__CWCC__) || defined(_WIN32)
   #pragma pack(push, 1)
#endif


/**
 * @brief Maximum-Receive-Unit option
 **/

typedef __start_packed struct
{
   uint8_t type;   //0
   uint8_t length; //1
   uint16_t mru;   //2-3
} __end_packed LcpMruOption;


/**
 * @brief Async-Control-Character-Map option
 **/

typedef __start_packed struct
{
   uint8_t type;   //0
   uint8_t length; //1
   uint32_t accm;  //2-5
} __end_packed LcpAccmOption;


/**
 * @brief Authentication-Protocol option
 **/

typedef __start_packed struct
{
   uint8_t type;      //0
   uint8_t length;    //1
   uint16_t protocol; //2-3
   uint8_t data[];    //4
} __end_packed LcpAuthProtocolOption;


/**
 * @brief Quality-Protocol option
 **/

typedef __start_packed struct
{
   uint8_t type;      //0
   uint8_t length;    //1
   uint16_t protocol; //2-3
   uint8_t data[];    //4
} __end_packed LcpQualityProtocolOption;


/**
 * @brief Magic-Number option
 **/

typedef __start_packed struct
{
   uint8_t type;         //0
   uint8_t length;       //1
   uint32_t magicNumber; //2-5
} __end_packed LcpMagicNumberOption;


/**
 * @brief Protocol-Field-Compression option
 **/

typedef __start_packed struct
{
   uint8_t type;   //0
   uint8_t length; //1
} __end_packed LcpPfcOption;


/**
 * @brief Address-and-Control-Field-Compression option
 **/

typedef __start_packed struct
{
   uint8_t type;   //0
   uint8_t length; //1
} __end_packed LcpAcfcOption;


//CodeWarrior or Win32 compiler?
#if defined(__CWCC__) || defined(_WIN32)
   #pragma pack(pop)
#endif

//LCP FSM events
error_list lcpOpen(PppContext *context);
error_list lcpClose(PppContext *context);

void lcpTick(PppContext *context);

void lcpProcessPacket(PppContext *context, const PppPacket *packet, size_t length);

error_list lcpProcessConfigureReq(PppContext *context,
   const PppConfigurePacket *configureReqPacket);

error_list lcpProcessConfigureAck(PppContext *context,
   const PppConfigurePacket *configureAckPacket);

error_list lcpProcessConfigureNak(PppContext *context,
   const PppConfigurePacket *configureNakPacket);

error_list lcpProcessConfigureReject(PppContext *context,
   const PppConfigurePacket *configureRejPacket);

error_list lcpProcessTerminateReq(PppContext *context,
   const PppTerminatePacket *terminateReqPacket);

error_list lcpProcessTerminateAck(PppContext *context,
   const PppTerminatePacket *terminateAckPacket);

error_list lcpProcessCodeRej(PppContext *context,
   const PppCodeRejPacket *codeRejPacket);

error_list lcpProcessProtocolRej(PppContext *context,
   const PppProtocolRejPacket *protocolRejPacket);

error_list lcpProcessEchoReq(PppContext *context,
   const PppEchoPacket *echoReqPacket);

error_list lcpProcessEchoRep(PppContext *context,
   const PppEchoPacket *echoRepPacket);

error_list lcpProcessDiscardReq(PppContext *context,
   const PppDiscardReqPacket *discardReqPacket);

error_list lcpProcessUnknownCode(PppContext *context,
   const PppPacket *packet);

error_list lcpProcessUnknownProtocol(PppContext *context,
   uint16_t protocol, const uint8_t *information, size_t length);

//LCP FSM callback functions
void lcpThisLayerUp(PppContext *context);
void lcpThisLayerDown(PppContext *context);
void lcpThisLayerStarted(PppContext *context);
void lcpThisLayerFinished(PppContext *context);

void lcpInitRestartCount(PppContext *context, uint_t value);
void lcpZeroRestartCount(PppContext *context);

error_list lcpSendConfigureReq(PppContext *context);

error_list lcpSendConfigureAck(PppContext *context,
   const PppConfigurePacket *configureReqPacket);

error_list lcpSendConfigureNak(PppContext *context,
   const PppConfigurePacket *configureReqPacket);

error_list lcpSendConfigureRej(PppContext *context,
   const PppConfigurePacket *configureReqPacket);

error_list lcpSendTerminateReq(PppContext *context);

error_list lcpSendTerminateAck(PppContext *context,
   const PppTerminatePacket *terminateReqPacket);

error_list lcpSendCodeRej(PppContext *context, const PppPacket *packet);
error_list lcpSendEchoRep(PppContext *context, const PppEchoPacket *echoReqPacket);

//LCP options checking
error_list lcpParseOption(PppContext *context, PppOption *option,
   size_t inPacketLen, PppConfigurePacket *outPacket);

error_list lcpParseMruOption(PppContext *context,
   LcpMruOption *option, PppConfigurePacket *outPacket);

error_list lcpParseAccmOption(PppContext *context,
   LcpAccmOption *option, PppConfigurePacket *outPacket);

error_list lcpParseAuthProtocolOption(PppContext *context,
   LcpAuthProtocolOption *option, PppConfigurePacket *outPacket);

error_list lcpParseMagicNumberOption(PppContext *context,
   LcpMagicNumberOption *option, PppConfigurePacket *outPacket);

error_list lcpParsePfcOption(PppContext *context,
   LcpPfcOption *option, PppConfigurePacket *outPacket);

error_list lcpParseAcfcOption(PppContext *context,
   LcpAcfcOption *option, PppConfigurePacket *outPacket);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
