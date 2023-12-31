/**
 * @file ssh_auth.h
 * @brief SSH user authentication protocol
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

#ifndef _SSH_AUTH_H
#define _SSH_AUTH_H

//Dependencies
#include "ssh/ssh.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Authentication methods
 **/

typedef enum
{
   SSH_AUTH_METHOD_NONE       = 0,
   SSH_AUTH_METHOD_PASSWORD   = 1,
   SSH_AUTH_METHOD_PUBLIC_KEY = 2,
   SSH_AUTH_METHOD_HOST_BASED = 3
} SshAuthMethod;


//SSH related functions
error_list sshSendUserAuthBanner(SshConnection *connection,
   const char_t *banner);

error_list sshSendUserAuthRequest(SshConnection *connection);
error_list sshSendUserAuthSuccess(SshConnection *connection);
error_list sshSendUserAuthFailure(SshConnection *connection);

error_list sshAcceptAuthRequest(SshConnection *connection);
error_list sshRejectAuthRequest(SshConnection *connection);

error_list sshFormatUserAuthBanner(SshConnection *connection,
   const char_t *banner, uint8_t *p, size_t *length);

error_list sshFormatUserAuthRequest(SshConnection *connection, uint8_t *message,
   size_t *length);

error_list sshFormatNoneAuthParams(SshConnection *connection, uint8_t *p,
   size_t *written);

error_list sshFormatUserAuthSuccess(SshConnection *connection, uint8_t *p,
   size_t *length);

error_list sshFormatUserAuthFailure(SshConnection *connection, uint8_t *p,
   size_t *length);

error_list sshFormatUserAuthMethods(SshConnection *connection, uint8_t *p,
   size_t *written);

error_list sshParseUserAuthBanner(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseUserAuthRequest(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseNoneAuthParams(SshConnection *connection,
   const SshString *userName, const uint8_t *p, size_t length);

error_list sshParseUserAuthSuccess(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseUserAuthFailure(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseUserAuthMessage(SshConnection *connection, uint8_t type,
   const uint8_t *message, size_t length);

SshAuthMethod sshGetAuthMethod(SshConnection *connection);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
