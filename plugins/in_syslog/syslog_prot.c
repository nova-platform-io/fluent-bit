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

#include <fluent-bit/flb_input_plugin.h>
#include <fluent-bit/flb_parser.h>
#include <fluent-bit/flb_time.h>
#include <fluent-bit/flb_pack.h>

#include "syslog.h"
#include "syslog_conn.h"

#include <string.h>

static inline void consume_bytes(char *buf, int bytes, int length)
{
    memmove(buf, buf + bytes, length - bytes);
}

static inline int pack_line(struct flb_syslog *ctx,
                            struct flb_time *time,
                            char *data, size_t data_size,
                            char *raw_data, size_t raw_data_size)
{
    char              *modified_data_buffer;
    int                modified_data_size;
    msgpack_object_kv *new_map_entries[1];
    msgpack_object_kv  raw_message_entry;
    msgpack_sbuffer    mp_sbuf;
    msgpack_packer     mp_pck;
    int                result;

    modified_data_buffer = NULL;

    if (ctx->message_raw_key != NULL) {
        new_map_entries[0] = &raw_message_entry;

        raw_message_entry.key.type = MSGPACK_OBJECT_STR;
        raw_message_entry.key.via.str.size = strlen(ctx->message_raw_key);
        raw_message_entry.key.via.str.ptr  = ctx->message_raw_key;

        raw_message_entry.val.type = MSGPACK_OBJECT_BIN;
        raw_message_entry.val.via.bin.size = raw_data_size;
        raw_message_entry.val.via.bin.ptr  = raw_data;

        result = flb_msgpack_expand_map(data, data_size,
                                        new_map_entries, 1,
                                        &modified_data_buffer,
                                        &modified_data_size);

        if (result != 0) {
            flb_plg_debug(ctx->ins,
                          "error expanding log record "
                          "with raw message : %d", result);
        }
    }

    /* Initialize local msgpack buffer */
    msgpack_sbuffer_init(&mp_sbuf);
    msgpack_packer_init(&mp_pck, &mp_sbuf, msgpack_sbuffer_write);

    msgpack_pack_array(&mp_pck, 2);
    flb_time_append_to_msgpack(time, &mp_pck, 0);

    if (modified_data_buffer != NULL) {
        msgpack_sbuffer_write(&mp_sbuf, modified_data_buffer, modified_data_size);
    }
    else {
        msgpack_sbuffer_write(&mp_sbuf, data, data_size);
    }

    flb_input_log_append(ctx->ins, NULL, 0, mp_sbuf.data, mp_sbuf.size);
    msgpack_sbuffer_destroy(&mp_sbuf);

    if (modified_data_buffer != NULL) {
        flb_free(modified_data_buffer);
    }

    return 0;
}

int syslog_prot_process(struct syslog_conn *conn)
{
    int len;
    int ret;
    char *p;
    char *eof;
    char *end;
    void *out_buf;
    size_t out_size;
    struct flb_time out_time;
    struct flb_syslog *ctx = conn->ctx;

    eof = conn->buf_data;
    end = conn->buf_data + conn->buf_len;

    /* Always parse while some remaining bytes exists */
    while (eof < end) {
        /* Lookup the ending byte */
        eof = p = conn->buf_data + conn->buf_parsed;
        while (*eof != '\n' && *eof != '\0' && eof < end) {
            eof++;
        }

        /* Incomplete message */
        if (eof == end || (*eof != '\n' && *eof != '\0')) {
            break;
        }

        /* No data ? */
        len = (eof - p);
        if (len == 0) {
            consume_bytes(conn->buf_data, 1, conn->buf_len);
            conn->buf_len--;
            conn->buf_parsed = 0;
            conn->buf_data[conn->buf_len] = '\0';
            end = conn->buf_data + conn->buf_len;

            if (conn->buf_len == 0) {
                break;
            }

            continue;
        }

        /* Process the string */
        ret = flb_parser_do(ctx->parser, p, len,
                            &out_buf, &out_size, &out_time);
        if (ret >= 0) {
            if (flb_time_to_nanosec(&out_time) == 0L) {
                flb_time_get(&out_time);
            }
            pack_line(ctx, &out_time,
                      out_buf, out_size,
                      p, len);
            flb_free(out_buf);
        }
        else {
            flb_plg_warn(ctx->ins, "error parsing log message with parser '%s'",
                         ctx->parser->name);
            flb_plg_debug(ctx->ins, "unparsed log message: %.*s", len, p);
        }

        conn->buf_parsed += len + 1;
        end = conn->buf_data + conn->buf_len;
        eof = conn->buf_data + conn->buf_parsed;
    }

    if (conn->buf_parsed > 0) {
        consume_bytes(conn->buf_data, conn->buf_parsed, conn->buf_len);
        conn->buf_len -= conn->buf_parsed;
        conn->buf_parsed = 0;
        conn->buf_data[conn->buf_len] = '\0';
    }

    return 0;
}

int syslog_prot_process_udp(char *buf, size_t size, struct flb_syslog *ctx)
{
    int ret;
    void *out_buf;
    size_t out_size;
    struct flb_time out_time = {0};

    ret = flb_parser_do(ctx->parser, buf, size,
                        &out_buf, &out_size, &out_time);
    if (ret >= 0) {
        if (flb_time_to_double(&out_time) == 0) {
            flb_time_get(&out_time);
        }
        pack_line(ctx, &out_time,
                  out_buf, out_size,
                  buf, size);
        flb_free(out_buf);
    }
    else {
        flb_plg_warn(ctx->ins, "error parsing log message with parser '%s'",
                     ctx->parser->name);
        flb_plg_debug(ctx->ins, "unparsed log message: %.*s",
                      (int) size, buf);
        return -1;
    }

    return 0;
}
