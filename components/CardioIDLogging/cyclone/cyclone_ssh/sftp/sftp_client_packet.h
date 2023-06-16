/**
 * @file sftp_client_packet.h
 * @brief SFTP packet parsing and formatting
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

#ifndef _SFTP_CLIENT_PACKET_H
#define _SFTP_CLIENT_PACKET_H

//Dependencies
#include "sftp/sftp_client.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

//SFTP client related functions
error_list sftpClientFormatFxpInit(SftpClientContext *context,
   uint32_t version);

error_list sftpClientFormatFxpOpen(SftpClientContext *context,
   const char_t *filename, uint32_t pflags);

error_list sftpClientFormatFxpClose(SftpClientContext *context,
   const uint8_t *handle, size_t handleLen);

error_list sftpClientFormatFxpRead(SftpClientContext *context,
   const uint8_t *handle, size_t handleLen, uint64_t offset, uint32_t dataLen);

error_list sftpClientFormatFxpWrite(SftpClientContext *context,
   const uint8_t *handle, size_t handleLen, uint64_t offset, uint32_t dataLen);

error_list sftpClientFormatFxpOpenDir(SftpClientContext *context,
   const char_t *path);

error_list sftpClientFormatFxpReadDir(SftpClientContext *context,
   const uint8_t *handle, size_t handleLen);

error_list sftpClientFormatFxpRemove(SftpClientContext *context,
   const char_t *filename);

error_list sftpClientFormatFxpMkDir(SftpClientContext *context,
   const char_t *path);

error_list sftpClientFormatFxpRmDir(SftpClientContext *context,
   const char_t *path);

error_list sftpClientFormatFxpRealPath(SftpClientContext *context,
   const char_t *path);

error_list sftpClientFormatFxpStat(SftpClientContext *context,
   const char_t *path);

error_list sftpClientFormatFxpRename(SftpClientContext *context,
   const char_t *oldPath, const char_t *newPath);

error_list sftpClientParseFxpVersion(SftpClientContext *context,
   const uint8_t *packet, size_t length);

error_list sftpClientParseFxpStatus(SftpClientContext *context,
   const uint8_t *packet, size_t length);

error_list sftpClientParseFxpHandle(SftpClientContext *context,
   const uint8_t *packet, size_t length);

error_list sftpClientParseFxpData(SftpClientContext *context,
   const uint8_t *packet, size_t fragLen, size_t totalLen);

error_list sftpClientParseFxpName(SftpClientContext *context,
   const uint8_t *packet, size_t fragLen, size_t totalLen);

error_list sftpClientParseFxpAttrs(SftpClientContext *context,
   const uint8_t *packet, size_t length);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
