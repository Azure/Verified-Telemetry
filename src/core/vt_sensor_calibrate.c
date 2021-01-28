/* Copyright (c) Microsoft Corporation.
   Licensed under the MIT License. */

#include "vt_database.h"
#include "vt_dsc.h"
#include "vt_sensor.h"

VT_FALL_STATE _vt_index37_state(int index_37);


void vt_sensor_calibrate(VT_SENSOR* sensor_ptr)
{
    printf("\nCalibrating Sensor Fingerprint\n");

    VT_STATE_BLOCK states;
    int status;

    states.previous_sampling_frequency = -1;
    states.previous_shape              = -1;
    states.current_sampling_frequency  = VT_STARTING_FREQUENCY;
    states.current_shape               = -1;
    states.noise_state                 =  0;

    status                                       = _vt_sensor_calibrate(sensor_ptr, &states);
    sensor_ptr->vt_sampling_frequency            = states.current_sampling_frequency;


    printf("Best Possible Sampling Frequency = %d\n", states.current_sampling_frequency);
    if (status == VT_SUCCESS)
        printf("Sensor has a good fall Curve\n");

    else if (status == VT_NOISY_FUNCTION_ERROR)
        printf("The Sensor doesnt give a fall curve in the specified frequency range.\n");

    else
        printf("The Sensor gives a consistent curve, but the fall is imperfect. Error Code = (0x%02x)\n", status);
}

uint _vt_sensor_calibrate(VT_SENSOR* sensor_ptr, VT_STATE_BLOCK* states)
{
    int index_max;
    int index_37;

    _vt_sensor_read_fingerprint(sensor_ptr, states->current_fingerprint, states->current_sampling_frequency);
    states->current_shape = _vt_fingerprint_calculate_shape(states->current_fingerprint, VT_FINGERPRINT_LENGTH);

    switch (states->current_shape)
    {
        case VT_SHAPE_FALL:
            index_max = _vt_fingerprint_calculate_maximum_index(states->current_fingerprint, VT_FINGERPRINT_LENGTH);
            index_37  = _vt_fingerprint_calculate_37index(states->current_fingerprint, VT_FINGERPRINT_LENGTH);
            VT_FALL_STATE index37_state = _vt_index37_state(index_37);

            switch (index37_state)
            {
                case VT_FALL_STATE_UNDERSHOOT:
                    states->current_sampling_frequency =
                        5 * (states->current_sampling_frequency /
                                (log((states->current_fingerprint[index_max] /
                                      states->current_fingerprint[VT_FINGERPRINT_LENGTH - 1]))));
                    break;
                case VT_FALL_STATE_OVERSHOOT:
                    states->current_sampling_frequency =
                        (index_37 * states->current_sampling_frequency) / (100 - VT_PRECISION_THRESHOLD / 2);
                    break;
                case VT_FALL_STATE_TARGET:
                    break;
            }
            break;

        case VT_SHAPE_RISE:
            if (states->previous_shape == VT_SHAPE_STEP)
                states->current_sampling_frequency =
                    (states->current_sampling_frequency + states->previous_sampling_frequency) / 2;
            else
                states->current_sampling_frequency = VT_MAXIMUM_FREQUENCY;
            break;

        case VT_SHAPE_STEP:
            if (states->previous_shape == VT_SHAPE_RISE)
                states->current_sampling_frequency =
                    (states->current_sampling_frequency + states->previous_sampling_frequency) / 2;
            else
                states->current_sampling_frequency = VT_MINIMUM_FREQUENCY;
            break;

        case VT_SHAPE_NOISE:
            states->noise_state++;

            switch (states->noise_state)
            {
                case VT_NOISE_STATE_MAX:
                    states->current_sampling_frequency = VT_MAXIMUM_FREQUENCY;
                    break;

                case VT_NOISE_STATE_MID:
                    states->current_sampling_frequency = (VT_MINIMUM_FREQUENCY + VT_MAXIMUM_FREQUENCY) / 2;
                    break;

                case VT_NOISE_STATE_MIN:
                    states->current_sampling_frequency = VT_MINIMUM_FREQUENCY;
                    break;

                case VT_NOISE_STATE_EXIT:
                    break;
            }
            break;
    }

    if (states->current_sampling_frequency < VT_MINIMUM_FREQUENCY)
        states->current_sampling_frequency = VT_MINIMUM_FREQUENCY;

    else if (states->current_sampling_frequency > VT_MAXIMUM_FREQUENCY)
        states->current_sampling_frequency = VT_MAXIMUM_FREQUENCY;

    if (states->current_sampling_frequency == states->previous_sampling_frequency)
    {
        if (_vt_fingerprint_evaluate_nrmse(states->previous_fingerprint,
                states->current_fingerprint,
                VT_FINGERPRINT_LENGTH) < VT_REPEATABILITY_THRESHOLD ||
            states->current_shape == VT_SHAPE_NOISE)
        {
            switch (states->current_shape)
            {
                case VT_SHAPE_FALL:
                    return VT_SUCCESS;
                case VT_SHAPE_RISE:
                    return VT_RISING_FUNCTION_ERROR;
                case VT_SHAPE_STEP:
                    return VT_STEP_FUNCTION_ERROR;
                case VT_SHAPE_NOISE:
                    return VT_NOISY_FUNCTION_ERROR;
            }
        }

        return VT_REPEATABILITY_ERROR;
    }

    states->previous_sampling_frequency = states->current_sampling_frequency;
    states->previous_shape              = states->current_shape;
    memcpy(states->previous_fingerprint, states->current_fingerprint, (100 * sizeof(uint32_t)));

    return _vt_sensor_calibrate(sensor_ptr, states);
}

VT_FALL_STATE _vt_index37_state(int index_37)
{
    if (index_37 == -1)
        return VT_FALL_STATE_UNDERSHOOT;

    else if (100 - index_37 >= VT_PRECISION_THRESHOLD)
        return VT_FALL_STATE_OVERSHOOT;

    return VT_FALL_STATE_TARGET;
}