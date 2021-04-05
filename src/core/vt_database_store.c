/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include <string.h>

#include "vt_fingerprint.h"
#include "vt_database.h"
#include "vt_dsc.h"

static uint32_t _vt_database_store_fingerprint(
    VT_DATABASE* database_ptr, uint32_t* fallcurvearray, int sampling_frequency, int sensor_id);
static void _vt_database_store_flash(VT_DATABASE* database_ptr, uint32_t* fallcurvearray, int sampling_frequency, int sensor_id);

static uint32_t _vt_database_store_fingerprint(
    VT_DATABASE* database_ptr, uint32_t* fallcurvearray, int sampling_frequency, int sensor_id)
{
    if (database_ptr == NULL)
    {
        return (VT_PTR_ERROR);
    }

    *(*(database_ptr->_vt_fingerprintdb + database_ptr->_vt_total_fingerprints) + 0) = sensor_id;
    *(*(database_ptr->_vt_fingerprintdb + database_ptr->_vt_total_fingerprints) + 1) = sampling_frequency;
    memcpy((*(database_ptr->_vt_fingerprintdb + database_ptr->_vt_total_fingerprints) + 2),
        fallcurvearray,
        (100 * sizeof(fallcurvearray)));
    database_ptr->_vt_total_fingerprints++;

    return VT_SUCCESS;
}

static void _vt_database_store_flash(
    VT_DATABASE* database_ptr, uint32_t* fallcurvearray, int sampling_frequency, int sensor_id)
{
    uint32_t total_fingerprints;
    uint32_t temp[1012];

    _vt_dsc_flash_read(database_ptr->vt_flash_address, temp, 2);

    if (temp[0] == FLASH_DB_START_VALUE)
    {

        total_fingerprints = temp[1];
        _vt_dsc_flash_read(database_ptr->vt_flash_address, temp, (2 + total_fingerprints * 103));

        temp[(103 * total_fingerprints + 2)] = database_ptr->vt_fallcurve_component_id;
        temp[(103 * total_fingerprints + 3)] = sensor_id;
        temp[(103 * total_fingerprints + 4)] = sampling_frequency;
        for (uint32_t database_index = 103 * total_fingerprints + 5, fallcurve_index = 0;
             database_index < 103 * total_fingerprints + 105;
             database_index++, fallcurve_index++)
            temp[database_index] = fallcurvearray[fallcurve_index];

        total_fingerprints++;
        temp[1] = total_fingerprints;

        _vt_dsc_flash_write(database_ptr->vt_flash_address, temp, (total_fingerprints * 103 + 2));
    }
}

uint32_t vt_database_store(VT_DATABASE* database_ptr, uint32_t* fallcurvearray, int sampling_frequency, int sensor_id)
{
    int falltime;
    float pearson_coefficient;

    if (_vt_fingerprint_calculate_falltime_pearsoncoefficient(
            fallcurvearray, 100, sampling_frequency, &falltime, &pearson_coefficient) == VT_SUCCESS)
    {
        _vt_database_store_fingerprint(database_ptr, fallcurvearray, sampling_frequency, sensor_id);
        _vt_database_store_falltime(database_ptr, falltime, sensor_id);
        _vt_database_store_pearsoncoefficient(database_ptr, pearson_coefficient, sensor_id);

        if (database_ptr->vt_flash_address != 0x00)
        {
            _vt_database_store_flash(database_ptr, fallcurvearray, sampling_frequency, sensor_id);
        }

        return VT_SUCCESS;
    }

    return VT_ERROR;
}

uint32_t _vt_database_store_falltime(VT_DATABASE* database_ptr, uint32_t fall_time, int sensor_id)
{
    uint8_t i;

    // Find index of the member nearest(smaller) to the new entry
    for (i = 0; ((i < database_ptr->_vt_total_falltime) && (database_ptr->_vt_falltimedb[i][1] < fall_time)); i++)
        ;
    database_ptr->_vt_total_falltime++;

    // Right Shift the members after the nearest index
    for (uint32_t j = database_ptr->_vt_total_falltime; j > i; j--)
    {
        database_ptr->_vt_falltimedb[j][0] = database_ptr->_vt_falltimedb[j - 1][0];
        database_ptr->_vt_falltimedb[j][1] = database_ptr->_vt_falltimedb[j - 1][1];
    }

    // Add new entry
    database_ptr->_vt_falltimedb[i][0] = sensor_id;
    database_ptr->_vt_falltimedb[i][1] = fall_time;
    printf("\tStored FallTime = %d\r\n", (int)database_ptr->_vt_falltimedb[i][1]);

    return VT_SUCCESS;
}

uint32_t _vt_database_store_pearsoncoefficient(VT_DATABASE* database_ptr, float pearson_coefficient, int sensor_id)
{
    uint8_t i;
    int pearsonCoeffInt1;
    float pearsonCoeffFrac;
    int pearsonCoeffInt2;

    // Scan through DB
    for (i = 0; (i < database_ptr->_vt_total_pearson_coefficient) &&
                (database_ptr->_vt_pearson_coefficientdb[i][0] != sensor_id);
         i++)
        ;

    // Check if the new label already exist
    if (i < database_ptr->_vt_total_pearson_coefficient)
    {
        database_ptr->_vt_pearson_coefficientdb[i][0] = sensor_id;
        database_ptr->_vt_pearson_coefficientdb[i][1] =
            (database_ptr->_vt_pearson_coefficientdb[i][1] + pearson_coefficient) / 2;
        pearsonCoeffInt1   = database_ptr->_vt_pearson_coefficientdb[i][1];
        pearsonCoeffFrac   = pearson_coefficient - pearsonCoeffInt1;
        pearsonCoeffInt2   = pearsonCoeffFrac * 10000;
        printf("\tStored Pearson Coefficient = %d.%04d\r\n", pearsonCoeffInt1, pearsonCoeffInt2);
    }

    // Add new entry if the label is not already present
    else
    {
        database_ptr->_vt_pearson_coefficientdb[database_ptr->_vt_total_pearson_coefficient][0] = sensor_id;
        database_ptr->_vt_pearson_coefficientdb[database_ptr->_vt_total_pearson_coefficient][1] = pearson_coefficient;
        pearsonCoeffInt1   = database_ptr->_vt_pearson_coefficientdb[database_ptr->_vt_total_pearson_coefficient][1];
        pearsonCoeffFrac   = pearson_coefficient - pearsonCoeffInt1;
        pearsonCoeffInt2   = pearsonCoeffFrac * 10000;
        printf("\tStored Pearson Coefficient = %d.%04d\r\n", pearsonCoeffInt1, pearsonCoeffInt2);
        database_ptr->_vt_total_pearson_coefficient++;
    }

    return VT_SUCCESS;
}
