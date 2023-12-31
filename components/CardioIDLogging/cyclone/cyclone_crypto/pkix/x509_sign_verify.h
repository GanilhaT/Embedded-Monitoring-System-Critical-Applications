/**
 * @file x509_sign_verify.h
 * @brief RSA/DSA/ECDSA/EdDSA signature verification
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

#ifndef _X509_SIGN_VERIFY_H
#define _X509_SIGN_VERIFY_H

//Dependencies
#include "core/crypto.h"
#include "pkix/x509_common.h"

//C++ guard
#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief Signature verification callback function
 **/

typedef error_list (*X509SignVerifyCallback)(const uint8_t *tbsCert,
   size_t tbsCertLen, const X509SignatureAlgoId *signatureAlgoId,
   const X509SubjectPublicKeyInfo *publicKeyInfo,
   const X509SignatureValue *signatureValue);


//X.509 related functions
error_list x509RegisterSignVerifyCallback(X509SignVerifyCallback callback);

error_list x509VerifySignature(const uint8_t *tbsCert, size_t tbsCertLen,
   const X509SignatureAlgoId *signatureAlgoId,
   const X509SubjectPublicKeyInfo *publicKeyInfo,
   const X509SignatureValue *signatureValue);

error_list x509VerifyRsaSignature(const uint8_t *tbsCert, size_t tbsCertLen,
   const HashAlgo *hashAlgo, const X509SubjectPublicKeyInfo *publicKeyInfo,
   const X509SignatureValue *signatureValue);

error_list x509VerifyRsaPssSignature(const uint8_t *tbsCert, size_t tbsCertLen,
   const HashAlgo *hashAlgo, size_t saltLen,
   const X509SubjectPublicKeyInfo *publicKeyInfo,
   const X509SignatureValue *signatureValue);

error_list x509VerifyDsaSignature(const uint8_t *tbsCert, size_t tbsCertLen,
   const HashAlgo *hashAlgo, const X509SubjectPublicKeyInfo *publicKeyInfo,
   const X509SignatureValue *signatureValue);

error_list x509VerifyEcdsaSignature(const uint8_t *tbsCert, size_t tbsCertLen,
   const HashAlgo *hashAlgo, const X509SubjectPublicKeyInfo *publicKeyInfo,
   const X509SignatureValue *signatureValue);

error_list x509VerifyEd25519Signature(const uint8_t *tbsCert, size_t tbsCertLen,
   const X509SubjectPublicKeyInfo *publicKeyInfo,
   const X509SignatureValue *signatureValue);

error_list x509VerifyEd448Signature(const uint8_t *tbsCert, size_t tbsCertLen,
   const X509SubjectPublicKeyInfo *publicKeyInfo,
   const X509SignatureValue *signatureValue);

//C++ guard
#ifdef __cplusplus
}
#endif

#endif
