/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2022 The Fluent Bit Authors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef FLB_LOG_EVENT_ENCODER_PRIMITIVES_H
#define FLB_LOG_EVENT_ENCODER_PRIMITIVES_H

int flb_log_event_encoder_append_values(
        struct flb_log_event_encoder *context,
        int field,
        ssize_t value_count,
        va_list arguments);

int flb_log_event_encoder_append_binary_length(
        struct flb_log_event_encoder *context,
        int target_field,
        size_t length);

int flb_log_event_encoder_append_binary_body(
        struct flb_log_event_encoder *context,
        int target_field,
        char *value,
        size_t length);

int flb_log_event_encoder_append_ext_length(
        struct flb_log_event_encoder *context,
        int target_field,
        int8_t type,
        size_t length);

int flb_log_event_encoder_append_ext_body(
        struct flb_log_event_encoder *context,
        int target_field,
        char *value,
        size_t length);

int flb_log_event_encoder_append_string_length(
        struct flb_log_event_encoder *context,
        int target_field,
        size_t length);

int flb_log_event_encoder_append_string_body(
        struct flb_log_event_encoder *context,
        int target_field,
        char *value,
        size_t length);

int flb_log_event_encoder_append_int8(
        struct flb_log_event_encoder *context,
        int target_field,
        int8_t value);

int flb_log_event_encoder_append_int16(
        struct flb_log_event_encoder *context,
        int target_field,
        int16_t value);

int flb_log_event_encoder_append_int32(
        struct flb_log_event_encoder *context,
        int target_field,
        int32_t value);

int flb_log_event_encoder_append_int64(
        struct flb_log_event_encoder *context,
        int target_field,
        int64_t value);

int flb_log_event_encoder_append_uint8(
        struct flb_log_event_encoder *context,
        int target_field,
        uint8_t value);

int flb_log_event_encoder_append_uint16(
        struct flb_log_event_encoder *context,
        int target_field,
        uint16_t value);

int flb_log_event_encoder_append_uint32(
        struct flb_log_event_encoder *context,
        int target_field,
        uint32_t value);

int flb_log_event_encoder_append_uint64(
        struct flb_log_event_encoder *context,
        int target_field,
        uint64_t value);

int flb_log_event_encoder_append_double(
    struct flb_log_event_encoder *context,
    int target_field,
    double value);

int flb_log_event_encoder_append_boolean(
    struct flb_log_event_encoder *context,
    int target_field,
    int value);

int flb_log_event_encoder_append_character(
        struct flb_log_event_encoder *context,
        int target_field,
        char value);

int flb_log_event_encoder_append_binary(
        struct flb_log_event_encoder *context,
        int target_field,
        char *value,
        size_t length);

int flb_log_event_encoder_append_string(
        struct flb_log_event_encoder *context,
        int target_field,
        char *value,
        size_t length);

int flb_log_event_encoder_append_ext(
        struct flb_log_event_encoder *context,
        int target_field,
        int8_t type,
        char *value,
        size_t length);

int flb_log_event_encoder_append_cstring(
        struct flb_log_event_encoder *context,
        int target_field,
        char *value);

int flb_log_event_encoder_append_msgpack_object(
    struct flb_log_event_encoder *context,
    int target_field,
    msgpack_object *value);

int flb_log_event_encoder_append_raw_msgpack(
    struct flb_log_event_encoder *context,
    int target_field,
    char *value_buffer,
    size_t value_size);

int flb_log_event_encoder_append_timestamp(
    struct flb_log_event_encoder *context,
    int target_field,
    struct flb_time *value);

int flb_log_event_encoder_append_legacy_timestamp(
    struct flb_log_event_encoder *context,
    int target_field,
    struct flb_time *value);

int flb_log_event_encoder_append_forward_v1_timestamp(
    struct flb_log_event_encoder *context,
    int target_field,
    struct flb_time *timestamp);

int flb_log_event_encoder_append_fluent_bit_v1_timestamp(
    struct flb_log_event_encoder *context,
    int target_field,
    struct flb_time *value);

int flb_log_event_encoder_append_fluent_bit_v2_timestamp(
    struct flb_log_event_encoder *context,
    int target_field,
    struct flb_time *value);

#endif
