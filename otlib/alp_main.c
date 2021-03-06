/*  Copyright 2017, JP Norair
  *
  * Licensed under the OpenTag License, Version 1.0 (the "License");
  * you may not use this file except in compliance with the License.
  * You may obtain a copy of the License at
  *
  * http://www.indigresso.com/wiki/doku.php?id=opentag:license_1_0
  *
  * Unless required by applicable law or agreed to in writing, software
  * distributed under the License is distributed on an "AS IS" BASIS,
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  */
/**
  * @file       /otlib/alp_main.c
  * @author     JP Norair
  * @version    R103
  * @date       31 Oct 2017
  * @brief      Application Layer Protocol Main Processor
  * @ingroup    ALP
  *
  * ALP is a library, not a task!
  *
  * The functions implemented in this file are for "main ALP."  Individual
  * protocol processing routines are implemented in other files named alp...c,
  * which should be in the same directory as this alp_main.c.
  *
  ******************************************************************************
  */

#include <otstd.h>
#include <otplatform.h>

#include <otlib/alp.h>
#include <otlib/ndef.h>
#include <otlib/memcpy.h>

#include <app/config.h>


#define ALP_FUNCTIONS       5

#if ALP(FILE_MGR)
#   define ALP_FILESYSTEM   1
#else
#   error "Filesystem must be supported"
#endif

#if (OT_FEATURE(SENSORS) == ENABLED)
#   define ALP_SENSORS      1
#else
#   define ALP_SENSORS      0
#endif

#define ALP_SECURITY        0
#if ALP(SECURE_MGR)
#   if ((OT_FEATURE(NL_SECURITY) == ENABLED) || (OT_FEATURE(DLL_SECURITY) == ENABLED))
#       undef ALP_SECURITY
#       define ALP_SECURITY 1
#   endif
#endif

#if ALP(LOGGER)
#   undef ALP_LOGGER
#   if (OT_FEATURE(LOGGER) == ENABLED)
#       define ALP_LOGGER   1
#   else
#       define ALP_LOGGER   0
#   endif
#endif

#if ALP(DASHFORTH)
#   undef ALP_DASHFORTH
#   if (OT_FEATURE(DASHFORTH) == ENABLED)
#       define ALP_DASHFORTH 1
#   else
#       define ALP_DASHFORTH 0
#   endif
#endif

#if (OT_FEATURE(ALPEXT) == ENABLED)
#   define ALP_EXT      1
#else
#   define ALP_EXT      0
#endif 





// Subroutines
ot_u8   sub_get_headerlen(ot_u8 tnf);
void    sub_insert_header(alp_tmpl* alp, ot_qcur hdr_position, ot_u8 hdr_len);




ot_u8 sub_get_headerlen(ot_u8 tnf) {
#if (OT_FEATURE(NDEF) == ENABLED)
    ot_u8 hdr_len;
    hdr_len     = 4;                        // Pure ALP
    hdr_len    += ((tnf == 5) << 1);        // Add Type len & ID len Fields
    hdr_len    -= (tnf == 6);               // Add Type len, Remove ID len & 2 byte ID
    return hdr_len;
#else
    return 4;
#endif
}



void sub_insert_header(alp_tmpl* alp, ot_qcur hdr_position, ot_u8 hdr_len) {
/// <LI> Add hdr_len to the queue length (cursors are already in place). </LI>
/// <LI> If using NDEF (hdr_len != 4), output header processing is ugly. </LI>
/// <LI> Pure ALP (hdr_len == 4) output header processing is universal. </LI>
/// <LI> Finally, always clear MB because now the first record is done. </LI>
    ot_qcur savedput = alp->outq->putcursor + hdr_len;
    
    ///@todo make sure this works, might need to have qcur be signed
    if (hdr_position != NULL) {
        alp->outq->putcursor = hdr_position;
    }

#   if (OT_FEATURE(NDEF) == ENABLED)
    if (hdr_len != 4) {
        q_writebyte(alp->outq, alp->OUTREC(FLAGS));     //Flags byte (always)
        q_writebyte(alp->outq, 0);                      //Type Len (NDEF only)
        q_writebyte(alp->outq, alp->OUTREC(PLEN));      //Payload len
        
        if (alp->OUTREC(FLAGS) & ALP_FLAG_MB) {
            alp->OUTREC(FLAGS)  &= ~(NDEF_IL | 7);
            alp->OUTREC(FLAGS)  |= TNF_Unchanged;       // Make next record "Unchanged"
            q_writebyte(alp->outq, 2);                  //NDEF ID Len
            q_writebyte(alp->outq, alp->OUTREC(ID));    //ALP-ID    (ID 1)
            q_writebyte(alp->outq, alp->OUTREC(CMD));   //ALP-CMD   (ID 2)
        }
    }
    else
#   else
    {
        q_writelong_be(alp->outq, alp->OUTREC(FLAGS));
        //ot_memcpy(hdr_position, &alp->OUTREC(FLAGS), 4);
    }
#   endif

    alp->outq->putcursor = savedput;
    alp->OUTREC(FLAGS)  &= ~ALP_FLAG_MB;
}







/** Protocol Processors <BR>
 * ========================================================================<BR>
 * The Null Processor is implemented here.  The rest of the processors are
 * implemented in separate C files, named alp_...c
 */
OT_WEAK ot_bool alp_proc_null(alp_tmpl* a0, const id_tmpl* a1) {
    return True;   // Atomic, with no payload data
}

#if (OT_FEATURE(ALPEXT) != ENABLED)
OT_WEAK ot_bool alp_ext_proc(alp_tmpl* alp, const id_tmpl* user_id) { return True; }
#endif





/** Internal Module Routines <BR>
 * ========================================================================<BR>
 */

typedef struct {
    alp_fn      callback;
    ot_queue*   appq;
} alp_elem_t;

static const alp_elem_t null_elem = { &alp_proc_null, NULL };

#if (ALP_EXT)
static const alp_elem_t ext_elem = { &alp_ext_proc, NULL };
#endif

///@todo If this gets much bigger, it's better to have a binary tree ID search
///      in order to conserve ROM/RAM space.
static const alp_elem_t alp_table[ALP_FUNCTIONS] = {
#if ALP_FILESYSTEM
    { &alp_proc_filedata, NULL },
#endif
#if ALP_SENSORS
    { &alp_proc_sensor, NULL },
#else
    { &alp_proc_null, NULL },
#endif
#if ALP_SECURITY
    { &alp_proc_security, NULL },
#else
    { &alp_proc_null, NULL },
#endif
#if ALP_LOGGER
    { &alp_proc_logger, NULL },
#else
    { &alp_proc_null, NULL },
#endif
#if ALP_DASHFORTH
    { &alp_proc_dashforth, NULL },
#else
    { &alp_proc_null, NULL },
#endif
};

const alp_elem_t* sub_get_elem(ot_u8 alp_id) {
    const alp_elem_t* element;
    
    if (alp_id == 0) {
        element = &null_elem;
    }
    else {
        alp_id--;

#       if (ALP_API)
        if (alp_id >= 0x80) {
            alp_id -= (0x80 - (ALP_FUNCTIONS-ALP_API));
        }
#       endif
        if (alp_id >= ALP_FUNCTIONS) {
#           if (ALP_EXT)
            element = &ext_elem;
#           else
            element = &null_elem;
#           endif
        }
        else {
            element = &(alp_table[alp_id]);
        }
    }
    
    return (const alp_elem_t*)element;
}


ot_bool alp_proc(alp_tmpl* alp, const id_tmpl* user_id) {
    const alp_elem_t*   proc_elem;
    ot_bool             output_code;
    
    // Always flush payload length of output before any data is written
    alp->OUTREC(PLEN) = 0;

    /// Get the processing element based on the input record ID
    proc_elem = sub_get_elem(INREC(alp, ID)); //sub_get_elem(alp->INREC(ID));

    /// If an application queue exists:
    /// <LI>Empty it on Message-Begin set</LI>
    /// <LI>Store payload to it in any case</LI>
    if (proc_elem->appq != NULL) {
        if (INREC(alp, FLAGS) & ALP_FLAG_MB) {
        //if (alp->inq->getcursor[0] & ALP_FLAG_MB) {
            q_empty(proc_elem->appq);
        }
        
        q_movedata(proc_elem->appq, alp->inq, INREC(alp, PLEN));
        //q_writestring(proc_elem->appq, alp->inq->getcursor, alp->INREC(PLEN));        // attempt 1
        //q_writestring(proc_elem->appq, alp->inq->getcursor, alp->inq->getcursor[1]);  // attempt 0
    }
    
    /// If the Message-End flag is set, then run the processor callback
    if (INREC(alp, FLAGS) & ALP_FLAG_ME) {
    //if (alp->inq->getcursor[0] & ALP_FLAG_ME) {
        output_code = proc_elem->callback(alp, user_id);
    }
    else {
        output_code = True;
    }

    ///@todo Presently responses are immediate.  This could be fixed later.
    alp->OUTREC(FLAGS) |= ALP_FLAG_ME;

    // Return 0 length (False) or non-zero length (True)
    return output_code;
}





/** Externally Callable Routines <BR>
 * ========================================================================<BR>
 */

void alp_init(alp_tmpl* alp, ot_queue* inq, ot_queue* outq) {
    alp->purge_id       = 1;
	alp->OUTREC(FLAGS)  = (ALP_FLAG_MB | ALP_FLAG_ME | ALP_FLAG_SR);   ///@todo this will need to be removed soon
	alp->inq            = inq;
	alp->outq           = outq;
    
    
}



void alp_add_app(alp_tmpl* alp, ot_u8 alp_id, alp_fn callback, ot_queue appq) {
///@todo To be completed when transformation of ALP is complete
}



ALP_status alp_parse_message(alp_tmpl* alp, const id_tmpl* user_id) {
    ALP_status  exit_code;
    ot_qcur     input_position;
    ot_qcur     hdr_position;
    //ot_u8       hdr_len;
    ot_int      proc_output;
    //ot_int      bytes;

    /// Lock the ot_queues while ALP is parsing/processing
    //alp->inq->options.ubyte[0]  = 1;
    //alp->outq->options.ubyte[0] = 1;
    q_lock(alp->inq);
    q_lock(alp->outq);

    /// Safety check: make sure both queues have room remaining for the
    /// most minimal type of message, an empty message
    if ((q_readspace(alp->inq) < 4) || (q_writespace(alp->outq) < 4)) {
        exit_code = MSG_Null;
        goto alp_parse_message_END;
    }

    /// Load a new input record only when the last output record has the
    /// "Message End" flag set.  Therefore, it was the last record of a
    /// previous message.  If new input record header does not match
    /// OpenTag requirement, bypass it and go to the next.  Else, copy
    /// the input record to the output record.  alp_proc() will adjust
    /// the output payload length and flags, as necessary.
    if (alp->OUTREC(FLAGS) & ALP_FLAG_ME) {
        alp->OUTREC(FLAGS)  = q_getcursor_val(alp->inq, 0);
        alp->OUTREC(PLEN)   = 0;
        alp->OUTREC(ID)     = q_getcursor_val(alp->inq, 2);
        alp->OUTREC(CMD)    = q_getcursor_val(alp->inq, 3);
    }
    input_position          = alp->inq->getcursor;
    alp->inq->getcursor    += 4;

    ///@todo transform output creation part to a separate function call the
    ///      application should use when building response messages.  That
    ///      function should handle chunking.

    /// Reserve space in alp->outq for header data.  It is updated later.
    /// The flags and payload length are determined by processing, so this
    /// method is necessary.
    hdr_position            = alp->outq->putcursor;
    alp->outq->putcursor   += 4;

    /// ALP Proc must write appropriate data to alp->outrec, and it must
    /// make sure not to overrun the output queue.  It must write:
    /// <LI> NDEF_CF if the output record is chunking </LI>
    /// <LI> NDEF_ME if the output record is the last in the message </LI>
    /// <LI> The output record payload length </LI>
    proc_output = alp_proc(alp, user_id);
    if (alp->OUTREC(PLEN) == 0) {
        // Remove header and any output data if no data written
        // Also, remove output chunking flag
        alp->outq->putcursor   = hdr_position;
        alp->OUTREC(FLAGS)    &= ~NDEF_CF;
    }
    else {
        ot_qcur savedput        = alp->outq->putcursor;
        alp->outq->putcursor    = hdr_position;
        q_writebyte(alp->outq, alp->OUTREC(FLAGS));
        q_writebyte(alp->outq, alp->OUTREC(PLEN));
        q_writebyte(alp->outq, alp->OUTREC(ID));
        q_writebyte(alp->outq, alp->OUTREC(CMD));
        alp->outq->putcursor    = savedput;
        alp->OUTREC(FLAGS)     &= ~ALP_FLAG_MB;
    }

    /// The input record, now treated, shall be rewound
    alp->inq->putcursor = input_position;
    alp->inq->getcursor = input_position;
    
    /// Unlock the ot_queues after ALP is parsing/processing
    //alp->inq->options.ubyte[0]  = 0;
    //alp->outq->options.ubyte[0] = 0;
    q_unlock(alp->inq);
    q_unlock(alp->outq);
    
    exit_code = MSG_End;

    alp_parse_message_END:
    return exit_code;
}





/** Functions Under Review <BR>
  * ========================================================================<BR>
  * These are legacy functions.  They might get bundled into different
  * functions, changed, or removed.
  */

/// @note This function is used by the logger (logger.c), but nowhere
/// else.  The ability to create a new output record/message is required, but
/// the method of doing it may likely get re-architected.
OT_WEAK void alp_new_record(alp_tmpl* alp, ot_u8 flags, ot_u8 payload_limit, ot_int payload_remaining) {
    // Clear control flags (begin, end, chunk)
	// Chunk and End will be intelligently set in this function, but Begin must
	// be set by the caller, AFTER this function.
	alp->OUTREC(FLAGS) |= flags;
	alp->OUTREC(FLAGS) |= NDEF_SR;
#   if (OT_FEATURE(NDEF))
	alp->OUTREC(FLAGS) &= ~(ALP_FLAG_ME | ALP_FLAG_CF | NDEF_IL);
#   else
    alp->OUTREC(FLAGS) &= (ALP_FLAG_MB | NDEF_SR);
#   endif

    // NDEF TNF needs to be 5 (with ID) on MB=1 and 6 (no ID) or MB=0
	// Pure ALP should always have IL=0 and TNF=0
#   if (OT_FEATURE(NDEF))
    if (alp->OUTREC(FLAGS) & 7) {
        alp->OUTREC(FLAGS) &= ~7;
        alp->OUTREC(FLAGS) |= (alp->OUTREC(FLAGS) & ALP_FLAG_MB) ? (NDEF_IL+5) : 6;
    }
#   endif

	// Automatically set Chunk or End.
	// "payload_remaining" is re-purposed to contain the number of bytes loaded
	// Chunk Flag is ignored by pure-ALP
	if (payload_remaining > payload_limit) {
		payload_remaining   = payload_limit;
#       if (OT_FEATURE(NDEF))
		alp->OUTREC(FLAGS)  |= ALP_FLAG_CF;
#       endif
	}
	else {
		alp->OUTREC(FLAGS)  |= ALP_FLAG_ME;
	}

	alp->OUTREC(PLEN) = (ot_u8)payload_remaining;
	sub_insert_header(alp, NULL, sub_get_headerlen(alp->OUTREC(FLAGS)&7));
}



