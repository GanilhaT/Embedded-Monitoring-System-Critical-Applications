/**
 * @file ssh_kex_hbr.h
 * @brief Post-quantum hybrid key exchange
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

#ifndef _SSH_KEX_HBR_H
#define _SSH_KEX_HBR_H

//Dependencies
#include "ssh/ssh.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

//SSH related functions
error_list sshSendHbrInit(SshConnection *connection);
error_list sshSendHbrReply(SshConnection *connection);

error_list sshFormatHbrReply(SshConnection *connection, uint8_t *p,
   size_t *length);

error_list sshFormatHbrInit(SshConnection *connection, uint8_t *p,
   size_t *length);

error_list sshParseHbrInit(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseHbrReply(SshConnection *connection, const uint8_t *message,
   size_t length);

error_list sshParseHbrMessage(SshConnection *connection, uint8_t type,
   const uint8_t *message, size_t length);

error_list sshSelectKemAlgo(SshConnection *connection);

error_list sshLoadKexClassicalEcdhParams(const char_t *kexAlgo, EcDomainParameters *params);
error_list sshGenerateClassicalEcdhKeyPair(SshConnection *connection);
error_list sshComputeClassicalEcdhSharedSecret(SshConnection *connection);

error_list sshDigestClientInit(SshConnection *connection);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
