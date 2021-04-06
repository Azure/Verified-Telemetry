/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <string.h>

#include "vt_database.h"
#include "vt_dsc.h"

uint32_t vt_database_clear(VT_DATABASE* database_ptr)
{
    uint32_t total_fingerprints;
    uint32_t total_fingerprints_of_this_component = 0;
    database_ptr->_vt_total_fingerprints          = 0;
    memset(database_ptr->_vt_fingerprintdb, 0, sizeof(database_ptr->_vt_fingerprintdb));

    if (database_ptr->vt_flash_address != 0x00)
    {
        uint32_t temp[1012];

        _vt_dsc_flash_read(database_ptr->vt_flash_address, temp, 2);

        if (temp[0] == FLASH_DB_START_VALUE)
        {
            total_fingerprints = temp[1];
            _vt_dsc_flash_read(database_ptr->vt_flash_address, temp, 2 + total_fingerprints * VT_FLASH_FINGERPRINT_LENGTH);
            for (uint32_t index = 2; index < (2 + total_fingerprints * VT_FLASH_FINGERPRINT_LENGTH); index += VT_FLASH_FINGERPRINT_LENGTH)
            {
                while (temp[index] == database_ptr->vt_fallcurve_component_id && index < (total_fingerprints * VT_FLASH_FINGERPRINT_LENGTH + 2))
                {
                    index += VT_FLASH_FINGERPRINT_LENGTH;
                    total_fingerprints_of_this_component++;
                }
                if (index >= (total_fingerprints * VT_FLASH_FINGERPRINT_LENGTH + 2))
                {
                    break;
                }
                for (int iter = 0; iter < VT_FLASH_FINGERPRINT_LENGTH; iter++)
                {
                    temp[(index - VT_FLASH_FINGERPRINT_LENGTH * total_fingerprints_of_this_component) + iter] = temp[index + iter];
                }
            }
            total_fingerprints -= total_fingerprints_of_this_component;
            temp[1] = total_fingerprints;
            _vt_dsc_flash_write(database_ptr->vt_flash_address, temp, (total_fingerprints * VT_FLASH_FINGERPRINT_LENGTH + 2));
        }
    }

    database_ptr->_vt_total_falltime = 0;
    memset(database_ptr->_vt_falltimedb, 0, sizeof(database_ptr->_vt_falltimedb));

    database_ptr->_vt_total_pearson_coefficient = 0;
    memset(database_ptr->_vt_pearson_coefficientdb, 0, sizeof(database_ptr->_vt_pearson_coefficientdb));

    return VT_SUCCESS;
}
