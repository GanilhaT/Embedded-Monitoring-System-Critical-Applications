/**
 * @file ssh_request.h
 * @brief Global request and channel request handling
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

#ifndef _SSH_REQUEST_H
#define _SSH_REQUEST_H

//Dependencies
#include "ssh/ssh.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief "tcpip-forward" global request parameters
 **/

typedef struct
{
   SshString addrToBind;
   uint32_t portNumToBind;
} SshTcpIpFwdParams;


/**
 * @brief "cancel-tcpip-forward" global request parameters
 **/

typedef struct
{
   SshString addrToBind;
   uint32_t portNumToBind;
} SshCancelTcpIpFwdParams;


/**
 * @brief "elevation" global request parameters
 **/

typedef struct
{
   bool_t elevationPerformed;
} SshElevationParams;


/**
 * @brief "pty-req" channel request parameters
 **/

typedef struct
{
   SshString termEnvVar;
   uint32_t termWidthChars;
   uint32_t termHeightRows;
   uint32_t termWidthPixels;
   uint32_t termHeightPixels;
   SshBinaryString termModes;
} SshPtyReqParams;


/**
 * @brief "x11-req" channel request parameters
 **/

typedef struct
{
   bool_t singleConnection;
   SshString x11AuthProtocol;
   SshString x11AuthCookie;
   uint32_t x11ScreenNum;
} SshX11ReqParams;


/**
 * @brief "env" channel request parameters
 **/

typedef struct
{
   SshString varName;
   SshString varValue;
} SshEnvParams;


/**
 * @brief "exec" channel request parameters
 **/

typedef struct
{
   SshString command;
} SshExecParams;


/**
 * @brief "subsystem" channel request parameters
 **/

typedef struct
{
   SshString subsystemName;
} SshSubsystemParams;


/**
 * @brief "window-change" channel request parameters
 **/

typedef struct
{
   uint32_t termWidthChars;
   uint32_t termHeightRows;
   uint32_t termWidthPixels;
   uint32_t termHeightPixels;
} SshWindowChangeParams;


/**
 * @brief "xon-xoff" channel request parameters
 **/

typedef struct
{
   bool_t clientCanDo;
} SshXonXoffParams;


/**
 * @brief "signal" channel request parameters
 **/

typedef struct
{
   SshString signalName;
} SshSignalParams;


/**
 * @brief "exit-status" channel request parameters
 **/

typedef struct
{
   uint32_t exitStatus;
} SshExitStatusParams;


/**
 * @brief "exit-signal" channel request parameters
 **/

typedef struct
{
   SshString signalName;
   bool_t coreDumped;
   SshString errorMessage;
   SshString languageTag;
} SshExitSignalParams;


/**
 * @brief "break" channel request parameters
 **/

typedef struct
{
   uint32_t breakLen;
} SshBreakParams;


//SSH related functions
error_list sshSendGlobalRequest(SshConnection *connection,
   const char_t *requestName, const void *requestParams, bool_t wantReply);

error_list sshSendRequestSuccess(SshConnection *connection);
error_list sshSendRequestFailure(SshConnection *connection);

error_list sshSendChannelRequest(SshChannel *channel, const char_t *requestType,
   const void *requestParams, bool_t wantReply);

error_list sshSendChannelSuccess(SshChannel *channel);
error_list sshSendChannelFailure(SshChannel *channel);

error_list sshFormatGlobalRequest(SshConnection *connection,
   const char_t *requestName, const void *requestParams, bool_t wantReply,
   uint8_t *p, size_t *length);

error_list sshFormatTcpIpFwdParams(const SshTcpIpFwdParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatCancelTcpIpFwdParams(const SshCancelTcpIpFwdParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatElevationParams(const SshElevationParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatRequestSuccess(SshConnection *connection, uint8_t *p,
   size_t *length);

error_list sshFormatRequestFailure(SshConnection *connection, uint8_t *p,
   size_t *length);

error_list sshFormatChannelRequest(SshChannel *channel, const char_t *requestType,
   const void *requestParams, bool_t wantReply, uint8_t *p, size_t *length);

error_list sshFormatPtyReqParams(const SshPtyReqParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatExecParams(const SshExecParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatSubsystemParams(const SshSubsystemParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatWindowChangeParams(const SshWindowChangeParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatSignalParams(const SshSignalParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatExitStatusParams(const SshExitStatusParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatBreakParams(const SshBreakParams *params,
   uint8_t *p, size_t *written);

error_list sshFormatChannelSuccess(SshChannel *channel, uint8_t *p,
   size_t *length);

error_list sshFormatChannelFailure(SshChannel *channel, uint8_t *p,
   size_t *length);

error_list sshParseGlobalRequest(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseTcpIpFwdParams(const uint8_t *p, size_t length,
   SshTcpIpFwdParams *params);

error_list sshParseCancelTcpIpFwdParams(const uint8_t *p, size_t length,
   SshCancelTcpIpFwdParams *params);

error_list sshParseElevationParams(const uint8_t *p, size_t length,
   SshElevationParams *params);

error_list sshParseRequestSuccess(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseRequestFailure(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseChannelRequest(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParsePtyReqParams(const uint8_t *p, size_t length,
   SshPtyReqParams *params);

error_list sshParseExecParams(const uint8_t *p, size_t length,
   SshExecParams *params);

bool_t sshGetExecArg(const SshExecParams *params, uint_t index, SshString *arg);

error_list sshParseSubsystemParams(const uint8_t *p, size_t length,
   SshSubsystemParams *params);

error_list sshParseWindowChangeParams(const uint8_t *p, size_t length,
   SshWindowChangeParams *params);

error_list sshParseSignalParams(const uint8_t *p, size_t length,
   SshSignalParams *params);

error_list sshParseExitStatusParams(const uint8_t *p, size_t length,
   SshExitStatusParams *params);

error_list sshParseBreakParams(const uint8_t *p, size_t length,
   SshBreakParams *params);

error_list sshParseChannelSuccess(SshConnection *connection,
   const uint8_t *message, size_t length);

error_list sshParseChannelFailure(SshConnection *connection,
   const uint8_t *message, size_t length);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
