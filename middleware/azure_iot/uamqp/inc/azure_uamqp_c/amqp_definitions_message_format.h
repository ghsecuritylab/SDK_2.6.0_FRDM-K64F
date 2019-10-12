

// Copyright (c) Microsoft. All rights reserved.
// Licensed under the MIT license. See LICENSE file in the project root for full license information.

// This file is generated. DO NOT EDIT it manually.
// The generator that produces it is located at /uamqp_generator/uamqp_generator.sln

#ifndef AMQP_DEFINITIONS_MESSAGE_FORMAT_H
#define AMQP_DEFINITIONS_MESSAGE_FORMAT_H


#ifdef __cplusplus
#include <cstdint>
extern "C" {
#else
#include <stdint.h>
#include <stdbool.h>
#endif

#include "azure_uamqp_c/amqpvalue.h"
#include "azure_c_shared_utility/umock_c_prod.h"


    typedef uint32_t message_format;

    MOCKABLE_FUNCTION(, AMQP_VALUE, amqpvalue_create_message_format, message_format, value);


    #define amqpvalue_get_message_format amqpvalue_get_uint



#ifdef __cplusplus
}
#endif

#endif /* AMQP_DEFINITIONS_MESSAGE_FORMAT_H */
