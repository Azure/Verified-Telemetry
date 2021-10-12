/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */
#include "vt_cs_database.h"

VT_VOID cs_reset_db(VT_CURRENTSENSE_OBJECT* cs_object)
{
    cs_object->fingerprintdb.template_type                                    = VT_CS_NON_REPEATING_SIGNATURE;
    cs_object->fingerprintdb.template.repeating_signatures.num_signatures     = 0;
    cs_object->fingerprintdb.template.repeating_signatures.lowest_sample_freq = VT_DATA_NOT_AVAILABLE;
    cs_object->fingerprintdb.template.repeating_signatures.offset_current     = VT_DATA_NOT_AVAILABLE;
    cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_on     = VT_DATA_NOT_AVAILABLE;
    cs_object->fingerprintdb.template.non_repeating_signature.avg_curr_off    = VT_DATA_NOT_AVAILABLE;
}