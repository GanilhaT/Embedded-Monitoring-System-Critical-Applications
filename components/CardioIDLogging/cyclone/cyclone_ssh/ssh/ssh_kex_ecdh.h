/**
 * @file ssh_kex_ecdh.h
 * @brief ECDH key exchange
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

#ifndef _SSH_KEX_ECDH_H
#define _SSH_KEX_ECDH_H

//Dependencies
#include "ssh/ssh.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

//SSH related functions
error_list sshSendKexEcdhInit(SshConnection *connection);
error_list sshSendKexEcdhReply(SshConnection *connection);

error_list sshFormatKexEcdhReply(SshConnection *connection, uint8_t *p,
   size_t *length);

error_list sshFormatKexEcdhInit(SshConnection *connection, uint8_t *p,
   size_t *length);

error_list sshParseKexEcdhInit(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseKexEcdhReply(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseKexEcdhMessage(SshConnection *connection, uint8_t type,
   const uint8_t *message, size_t length);

error_list sshLoadKexEcdhParams(EcDomainParameters *params, const char_t *kexAlgo);
error_list sshGenerateEcdhKeyPair(SshConnection *connection);
error_list sshComputeEcdhSharedSecret(SshConnection *connection);
error_list sshDigestClientEcdhPublicKey(SshConnection *connection);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
