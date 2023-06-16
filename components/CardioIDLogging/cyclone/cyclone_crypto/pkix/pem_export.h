/**
 * @file pem_export.h
 * @brief PEM file export functions
 *
 * @section License
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 * Copyright (C) 2010-2023 Oryx Embedded SARL. All rights reserved.
 *
 * This file is part of CycloneCRYPTO Open.
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

#ifndef _PEM_EXPORT_H
#define _PEM_EXPORT_H

//Dependencies
#include "core/crypto.h"
#include "pkix/pem_common.h"
#include "pkc/dh.h"
#include "pkc/rsa.h"
#include "pkc/dsa.h"
#include "ecc/ec.h"
#include "ecc/eddsa.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif

//PEM related functions
error_list pemExportCertificate(const uint8_t *cert, size_t certLen,
   char_t *output, size_t *written);

error_list pemExportCrl(const uint8_t *crl, size_t crlLen,
   char_t *output, size_t *written);

error_list pemExportCsr(const uint8_t *csr, size_t csrLen,
   char_t *output, size_t *written);

error_list pemExportRsaPublicKey(const RsaPublicKey *publicKey,
   char_t *output, size_t *written);

error_list pemExportRsaPrivateKey(const RsaPrivateKey *privateKey,
   char_t *output, size_t *written);

error_list pemExportRsaPssPublicKey(const RsaPublicKey *publicKey,
   char_t *output, size_t *written);

error_list pemExportRsaPssPrivateKey(const RsaPrivateKey *privateKey,
   char_t *output, size_t *written);

error_list pemExportDsaPublicKey(const DsaPublicKey *publicKey,
   char_t *output, size_t *written);

error_list pemExportDsaPrivateKey(const DsaPrivateKey *privateKey,
   char_t *output, size_t *written);

error_list pemExportEcParameters(const EcCurveInfo *curveInfo,
   char_t *output, size_t *written);

error_list pemExportEcPublicKey(const EcCurveInfo *curveInfo,
   const EcPublicKey *publicKey, char_t *output, size_t *written);

error_list pemExportEcPrivateKey(const EcCurveInfo *curveInfo,
   const EcPrivateKey *privateKey, const EcPublicKey *publicKey,
   char_t *output, size_t *written);

error_list pemExportEddsaPublicKey(const EcCurveInfo *curveInfo,
   const EddsaPublicKey *publicKey, char_t *output, size_t *written);

error_list pemExportEddsaPrivateKey(const EcCurveInfo *curveInfo,
   const EddsaPrivateKey *privateKey, char_t *output, size_t *written);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
