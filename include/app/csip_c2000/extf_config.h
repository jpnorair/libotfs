/* Replace License */
/**
  * @file       /app/csip_c2000/extf_config.h
  * @author     JP Norair (jpnorair@indigresso.com)
  * @version    V1.0
  * @date       31 July 2012
  * @brief      Extension Function Configuration File
  *
  ******************************************************************************
  */

#ifndef __EXTF_CONFIG_H
#define __EXTF_CONFIG_H

/// Queue Module: on C2000 this is completely different
///@todo could put all C2000 generic patches in a separate include header
#define EXTF_q_init
#define EXTF_q_rebase
#define EXTF_q_copy
#define EXTF_q_intrinsics
#define EXTF_q_length
#define EXTF_q_span
#define EXTF_q_writespace
#define EXTF_q_readspace
#define EXTF_q_blocktime
#define EXTF_q_blockwrite
#define EXTF_q_lock
#define EXTF_q_unlock
#define EXTF_q_empty
#define EXTF_q_rewind
#define EXTF_q_start
#define EXTF_q_markbyte
#define EXTF_q_writebyte
#define EXTF_q_writeshort
#define EXTF_q_writeshort_be
#define EXTF_q_writelong
#define EXTF_q_writelong_be
#define EXTF_q_readbyte
#define EXTF_q_readshort
#define EXTF_q_readshort_be
#define EXTF_q_readlong
#define EXTF_q_readlong_be
#define EXTF_q_writestring
#define EXTF_q_readstring
#define EXTF_q_movedata


/// ALP Module EXTFs



/// Auth Module EXTFs



/// Buffer Module EXTFs



/// CRC16 Module EXTFs


/// Crypto Module EXTFs
#define EXTF_EAXdrv_init
#define EXTF_EAXdrv_clear
#define EXTF_EAXdrv_encrypt
#define EXTF_EAXdrv_decrypt



/// Encode Module EXTFs



/// External Module EXTFs



/// M2 Network Module EXTFs



/// M2QP Module EXTFs


/// MPipe EXTFs


/// NDEF module EXTFs


/// OT Utils EXTFs


/// OTAPI C EXTFs


/// OTAPI EXTFs



/// Radio EXTFs


/// Session EXTFs


/// System EXTFs


/// Veelite Core EXTFs


/// Veelite Module EXTFs




#endif 
