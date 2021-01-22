/* Copyright (c) Microsoft Corporation. */

#include "vt_database.h"

uint32_t temp[1012];

uint vt_database_initialize(VT_DATABASE* database_ptr,uint32_t flash_address, uint32_t fallcurve_component_id)
{
    database_ptr->vt_flash_address = flash_address;
    database_ptr->vt_fallcurve_component_id = fallcurve_component_id;

    _vt_database_initialize_fingerpintdb(database_ptr);
    _vt_database_initialize_falltimedb(database_ptr);
    _vt_database_initialize_pearsoncoefficientdb(database_ptr);

    return VT_SUCCESS;
}

void _vt_database_initialize_fingerpintdb(VT_DATABASE* database_ptr)
{

    database_ptr->_vt_total_fingerprints = 0;
    memset(database_ptr->_vt_fingerprintdb, 0, sizeof(database_ptr->_vt_fingerprintdb));
    uint32_t total_fingerprints;
    uint32_t total_fingerprints_of_this_component = 0;

    if (database_ptr->vt_flash_address != 0x00)
    {
        _vt_dsc_flash_read(database_ptr->vt_flash_address, temp, 2);
        
        if (temp[0] == FLASH_DB_START_VALUE)
        {
            total_fingerprints = temp[1];
            _vt_dsc_flash_read(database_ptr->vt_flash_address, temp, 2 + total_fingerprints * 103);

            uint32_t row, column, index;

            for (row = 0, index = 2; index < (total_fingerprints * 103 + 2); row++)
            {
                while(temp[index] != database_ptr->vt_fallcurve_component_id && index < (total_fingerprints * 103 + 2))
                {
                    index += 103;
                }
                if (index >= (total_fingerprints * 103 + 2))
                {
                    break;
                }
                
                index++;
                total_fingerprints_of_this_component++;
                for (column = 0; column < 102; column++, index++)
                {
                    database_ptr->_vt_fingerprintdb[row][column] = (uint32_t)temp[index];
                }
            }
            database_ptr->_vt_total_fingerprints = total_fingerprints_of_this_component;    
            printf("\nTotal Fingerprints %d\n", (int)database_ptr->_vt_total_fingerprints);
        }
        else
        {
            temp[0] = FLASH_DB_START_VALUE;
            temp[1] =  database_ptr->_vt_total_fingerprints;
            _vt_dsc_flash_write(database_ptr->vt_flash_address, temp, 2);
        }
        
             
    }
    
}

void _vt_database_initialize_falltimedb(VT_DATABASE* database_ptr)
{
    database_ptr->_vt_total_falltime = 0;
    memset(database_ptr->_vt_falltimedb, 0, sizeof(database_ptr->_vt_falltimedb));
    
    if ((database_ptr->vt_flash_address != 0x00) && (database_ptr->_vt_total_fingerprints > 0))
    {
        int falltime;
        double pearson_coefficient;

        for (int i = 0; i < database_ptr->_vt_total_fingerprints; i++)
        {
            if (_vt_fingerprint_calculate_falltime_pearsoncoefficient(&(database_ptr->_vt_fingerprintdb[i][2]),
                    100,
                    (database_ptr->_vt_fingerprintdb[i][1]),
                    &falltime,
                    &pearson_coefficient) == VT_SUCCESS)
            {
                _vt_database_store_falltime(database_ptr, falltime, (database_ptr->_vt_fingerprintdb[i][0]));
            }
        }
        
    }

}

void _vt_database_initialize_pearsoncoefficientdb(VT_DATABASE* database_ptr)
{
    database_ptr->_vt_total_pearson_coefficient = 0;
    memset(database_ptr->_vt_pearson_coefficientdb, 0, sizeof(database_ptr->_vt_pearson_coefficientdb));
    
    if ((database_ptr->vt_flash_address != 0x00) && (database_ptr->_vt_total_fingerprints > 0))
    {
        int falltime;
        double pearson_coefficient;

        for (int i = 0; i < database_ptr->_vt_total_fingerprints; i++)
        {
            if (_vt_fingerprint_calculate_falltime_pearsoncoefficient(&(database_ptr->_vt_fingerprintdb[i][2]),
                    100,
                    (database_ptr->_vt_fingerprintdb[i][1]),
                    &falltime,
                    &pearson_coefficient) == VT_SUCCESS)
            {
                _vt_database_store_pearsoncoefficient(
                    database_ptr, pearson_coefficient, (database_ptr->_vt_fingerprintdb[i][0]));
            }
                
        }
    }

}
