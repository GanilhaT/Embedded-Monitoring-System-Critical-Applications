/**
 * @file ssh_connection.h
 * @brief SSH connection protocol
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2019-2023 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneSSH Open.
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

#ifndef _SSH_CONNECTION_H
#define _SSH_CONNECTION_H

//Dependencies
#include "ssh/ssh.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief "x11" channel specific parameters
 **/

typedef struct
{
   SshString originatorAddr;
   uint32_t originatorPort;
} SshX11ChannelParams;


/**
 * @brief "forwarded-tcpip" channel specific parameters
 **/

typedef struct
{
   SshString addrConnected;
   uint32_t portConnected;
   SshString originIpAddr;
   uint32_t originPort;
} SshForwardedTcpIpParams;


/**
 * @brief "direct-tcpip" channel specific parameters
 **/

typedef struct
{
   SshString hostToConnect;
   uint32_t portToConnect;
   SshString originIpAddr;
   uint32_t originPort;
} SshDirectTcpIpParams;


//SSH related functions
error_list sshSendChannelOpen(SshChannel *channel, const char_t *channelType,
   const void *channelParams);

error_list sshSendChannelOpenConfirmation(SshChannel *channel);

error_list sshSendChannelOpenFailure(SshConnection *connection,
   uint32_t recipientChannel, uint32_t reasonCode, const char_t *description);

error_list sshSendChannelWindowAdjust(SshChannel *channel, size_t windowSizeInc);
error_list sshSendChannelData(SshChannel *channel, size_t dataLen);
error_list sshSendChannelEof(SshChannel *channel);
error_list sshSendChannelClose(SshChannel *channel);

error_list sshFormatChannelOpen(SshChannel *channel, const char_t *channelType,
   const void *channelParams, uint8_t *p, size_t *length);

error_list sshFormatForwardedTcpIpParams(const SshForwardedTcpIpParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatDirectTcpIpParams(const SshDirectTcpIpParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatChannelOpenConfirmation(SshChannel *channel, uint8_t *p,
   size_t *length);

error_list sshFormatChannelOpenFailure(SshConnection *connection,
   uint32_t recipientChannel, uint32_t reasonCode, const char_t *description,
   uint8_t *p, size_t *length);

error_list sshFormatChannelWindowAdjust(SshChannel *channel, size_t windowSizeInc,
   uint8_t *p, size_t *length);

error_list sshFormatChannelData(SshChannel *channel, size_t dataLen,
   uint8_t *p, size_t *length);

error_list sshFormatChannelEof(SshChannel *channel, uint8_t *p, size_t *length);
error_list sshFormatChannelClose(SshChannel *channel, uint8_t *p, size_t *length);

error_list sshParseChannelOpen(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseForwardedTcpIpParams(const uint8_t *p, size_t length,
   SshForwardedTcpIpParams *params);

error_list sshParseDirectTcpIpParams(const uint8_t *p, size_t length,
   SshDirectTcpIpParams *params);

error_list sshParseChannelOpenConfirmation(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseChannelOpenFailure(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseChannelWindowAdjust(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseChannelData(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseChannelExtendedData(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseChannelEof(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseChannelClose(SshConnection *connection,
   const uint8_t *message, size_t length);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
