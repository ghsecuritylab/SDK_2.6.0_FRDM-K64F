/* Copyright 2019 NXP
 * 
 * This software is owned or controlled by NXP and may only be used
 * strictly in accordance with the applicable license terms.  By expressly
 * accepting such terms or by downloading, installing, activating and/or
 * otherwise using the software, you are agreeing that you have read, and
 * that you agree to comply with and are bound by, such license terms.  If
 * you do not agree to be bound by the applicable license terms, then you
 * may not retain, install, activate or otherwise use the software.
 * 
 */

#ifndef PLUGANDTRUST_HOSTLIB_VERSION_INFO_H_INCLUDED
#define PLUGANDTRUST_HOSTLIB_VERSION_INFO_H_INCLUDED


/* clang-format off */
#define PLUGANDTRUST_HOSTLIB_PROD_NAME          "PlugAndTrust_HostLib"
#define PLUGANDTRUST_HOSTLIB_VER_STRING_NUM     "v02.08.00_20190427"
#define PLUGANDTRUST_HOSTLIB_PROD_NAME_VER_FULL "PlugAndTrust_HostLib_v02.08.00_20190427"
#define PLUGANDTRUST_HOSTLIB_VER_MAJOR          (2u)
#define PLUGANDTRUST_HOSTLIB_VER_MINOR          (8u)
#define PLUGANDTRUST_HOSTLIB_VER_DEV            (0u)

/* v02.08 = 20008u */
#define PLUGANDTRUST_HOSTLIB_VER_MAJOR_MINOR ( 0 \
    | (PLUGANDTRUST_HOSTLIB_VER_MAJOR * 10000u)    \
    | (PLUGANDTRUST_HOSTLIB_VER_MINOR))

/* v02.08.00 = 200080000ULL */
#define PLUGANDTRUST_HOSTLIB_VER_MAJOR_MINOR_DEV ( 0 \
    | (PLUGANDTRUST_HOSTLIB_VER_MAJOR * 10000*10000u)    \
    | (PLUGANDTRUST_HOSTLIB_VER_MINOR * 10000u)    \
    | (PLUGANDTRUST_HOSTLIB_VER_DEV))

/* clang-format on */


/* Version Information:
 * Generated by:
 *     scripts\version_info.py (v2019.01.17_00)
 * 
 * Do not edit this file. Update:
 *     hostlib/version_info.txt instead.
 * 
 * 
 * prod_name = "PlugAndTrust_HostLib"
 * 
 * prod_desc = "Host Library"
 * 
 * lang_c_prefix = prod_name.upper()
 * 
 * lang_namespace = ""
 * 
 * v_major  = "02"
 * 
 * v_minor  = "08"
 * 
 * v_dev    = "00"
 * 
 * v_meta   = ""
 * 
 * maturity = "B"
 * 
 */

#endif /* PLUGANDTRUST_HOSTLIB_VERSION_INFO_H_INCLUDED */
