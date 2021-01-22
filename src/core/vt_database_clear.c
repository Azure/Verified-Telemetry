/* Copyright (c) Microsoft Corporation. */

#include "vt_database.h"
uint32_t temp[1012];
uint vt_database_clear(VT_DATABASE* database_ptr)
{
    uint32_t total_fingerprints;
    uint32_t total_fingerprints_of_this_component = 0;
	database_ptr->_vt_total_fingerprints = 0;
    memset(database_ptr->_vt_fingerprintdb, 0, sizeof(database_ptr->_vt_fingerprintdb));

    if (database_ptr->vt_flash_address != 0x00)
    {
        _vt_dsc_flash_read(database_ptr->vt_flash_address, temp, 2);
        
        if (temp[0] == FLASH_DB_START_VALUE)
        {
            total_fingerprints = temp[1];
            _vt_dsc_flash_read(database_ptr->vt_flash_address, temp, 2 + total_fingerprints * 103);
            for (uint index = 2; index < (2 + total_fingerprints * 103); index+=103)
            {
                while(temp[index] == database_ptr->vt_fallcurve_component_id && index < (total_fingerprints * 103 + 2))
                {
                    index += 103;
                    total_fingerprints_of_this_component++;
                }
                if (index >= (total_fingerprints * 103 + 2))
                {
                    break;
                }
                for (int iter = 0; iter < 103; iter++)
                {
                    temp[(index - 103*total_fingerprints_of_this_component) + iter] = temp[index + iter];
                }
            }
            total_fingerprints -= total_fingerprints_of_this_component;
            temp[1] = total_fingerprints;
            _vt_dsc_flash_write(database_ptr->vt_flash_address, temp, (total_fingerprints * 103 + 2));
        }
    }

    database_ptr->_vt_total_falltime = 0;
    memset(database_ptr->_vt_falltimedb, 0, sizeof(database_ptr->_vt_falltimedb));

    database_ptr->_vt_total_pearson_coefficient = 0;
    memset(database_ptr->_vt_pearson_coefficientdb, 0, sizeof(database_ptr->_vt_pearson_coefficientdb));

	return VT_SUCCESS;
}