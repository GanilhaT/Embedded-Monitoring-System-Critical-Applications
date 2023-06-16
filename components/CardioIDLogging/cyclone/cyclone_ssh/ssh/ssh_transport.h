/**
 * @file ssh_transport.h
 * @brief SSH transport layer protocol
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

#ifndef _SSH_TRANSPORT_H
#define _SSH_TRANSPORT_H

//Dependencies
#include "ssh/ssh.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

//SSH related functions
error_list sshSendIdString(SshConnection *connection);
error_list sshSendServiceRequest(SshConnection *connection);

error_list sshSendServiceAccept(SshConnection *connection,
   const char_t *serviceName);

error_list sshSendDisconnect(SshConnection *connection,
   uint32_t reasonCode, const char_t *description);

error_list sshSendUnimplemented(SshConnection *connection,
   const uint8_t *packetSeqNum);

error_list sshFormatServiceRequest(SshConnection *connection, uint8_t *p,
   size_t *length);

error_list sshFormatServiceAccept(SshConnection *connection,
   const char_t *serviceName, uint8_t *p, size_t *length);

error_list sshFormatDisconnect(SshConnection *connection, uint32_t reasonCode,
   const char_t *description, uint8_t *p, size_t *length);

error_list sshFormatUnimplemented(SshConnection *connection,
   const uint8_t *packetSeqNum, uint8_t *p, size_t *length);

error_list sshParseIdString(SshConnection *connection, const uint8_t *id,
   size_t length);

error_list sshParseServiceRequest(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseServiceAccept(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseIgnore(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseDebug(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseDisconnect(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseUnimplemented(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseUnrecognized(SshConnection *connection, const uint8_t *message,
   size_t length);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
