/*
 * Copyright (C) 2018-2022 Philippe Proulx <eepp.ca>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license. See the LICENSE file for details.
 */

#include <limits>
#include <cstring>
#include <sstream>
#include <iostream>

#include <yactfr/yactfr.hpp>

#include <mem-data-src-factory.hpp>
#include <elem-printer.hpp>

static const auto metadata =
    "/* CTF 1.8 */\n"
    "typealias integer { size = 8; } := u8;"
    "typealias integer { size = 16; } := u16;"
    "typealias integer { size = 32; } := u32;"
    "typealias integer { size = 64; } := u64;"
    "trace {"
    "  major = 1;"
    "  minor = 8;"
    "  byte_order = be;"
    "  uuid = \"64df608e-8db9-4fed-9c50-0eb972392cf7\";"
    "  packet.header := struct {"
    "    u32 magic;"
    "    u8 uuid[16];"
    "    u8 stream_id;"
    "  };"
    "};"
    "stream {"
    "  id = 0x23;"
    "  packet.context := struct {"
    "    u16 packet_size;"
    "    u16 content_size;"
    "  };"
    "};"
    "event {"
    "  stream_id = 0x23;"
    "  fields := struct {"
    "    string a;"
    "    u32 b;"
    "  };"
    "};"
    "stream {"
    "  id = 0xdd;"
    "  packet.context := struct {"
    "    u16 packet_size;"
    "    u16 content_size;"
    "    u16 custom;"
    "  };"
    "};"
    "event {"
    "  stream_id = 0xdd;"
    "  fields := struct {"
    "    struct {"
    "      u32 a;"
    "      u16 b;"
    "      u8 c;"
    "    } s;"
    "  };"
    "};";

static const std::uint8_t stream[] = {
    // packet header
    0xc1, 0xfc, 0x1f, 0xc1,
    0x64, 0xdf, 0x60, 0x8e, 0x8d, 0xb9, 0x4f, 0xed,
    0x9c, 0x50, 0x0e, 0xb9, 0x72, 0x39, 0x2c, 0xf7,
    0xdd,

    // packet context
    0x01, 0xa0, 0x01, 0x80, 0x11, 0xd2,

    // event record
    0xaa, 0xbb, 0xcc, 0xdd, 0xde, 0xad, 0xff,

    // event record
    0x12, 0x12, 0x23, 0x23, 0x44, 0x55, 0x66,

    // event record
    0x18, 0x19, 0x11, 0x0e, 0xf2, 0x43, 0x51,

    // padding
    0xff, 0x00, 0xf0, 0x0f,

    // packet header
    0xc1, 0xfc, 0x1f, 0xc1,
    0x64, 0xdf, 0x60, 0x8e, 0x8d, 0xb9, 0x4f, 0xed,
    0x9c, 0x50, 0x0e, 0xb9, 0x72, 0x39, 0x2c, 0xf7,
    0x23,

    // packet context
    0x01, 0x60, 0x01, 0x60,

    // event record
    's', 'a', 'l', 'u', 't', 0, 0x44, 0x55, 0x66, 0x77,

    // event record
    'C', 'o', 'l', 'a', 0, 0x44, 0x55, 0x66, 0x77,

    // packet header
    0xc1, 0xfc, 0x1f, 0xc1,
    0x64, 0xdf, 0x60, 0x8e, 0x8d, 0xb9, 0x4f, 0xed,
    0x9c, 0x50, 0x0e, 0xb9, 0x72, 0x39, 0x2c, 0xf7,
    0xdd,

    // packet context
    0x01, 0x20, 0x01, 0x10, 0xfe, 0xdc,

    // event record
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07,

    // padding
    0x11, 0x22,
};

static const auto expected =
    "P {\n"
    "PC {\n"
    "SC:0 {\n"
    "ST {\n"
    "UI:magic:3254525889\n"
    "PMN:3254525889\n"
    "SA:uuid {\n"
    "UI:100\n"
    "UI:223\n"
    "UI:96\n"
    "UI:142\n"
    "UI:141\n"
    "UI:185\n"
    "UI:79\n"
    "UI:237\n"
    "UI:156\n"
    "UI:80\n"
    "UI:14\n"
    "UI:185\n"
    "UI:114\n"
    "UI:57\n"
    "UI:44\n"
    "UI:247\n"
    "P {\n"
    "PC {\n"
    "SC:0 {\n"
    "ST {\n"
    "UI:magic:3254525889\n"
    "PMN:3254525889\n"
    "SA:uuid {\n"
    "UI:100\n"
    "UI:223\n"
    "UI:96\n"
    "UI:142\n"
    "UI:141\n"
    "UI:185\n"
    "UI:79\n"
    "UI:237\n"
    "UI:156\n"
    "UI:80\n"
    "UI:14\n"
    "UI:185\n"
    "UI:114\n"
    "UI:57\n"
    "UI:44\n"
    "UI:247\n"
    "TTU:64df608e-8db9-4fed-9c50-0eb972392cf7\n"
    "}\n"
    "UI:stream_id:221\n"
    "}\n"
    "}\n"
    "DST:221\n"
    "SC:1 {\n"
    "ST {\n"
    "UI:packet_size:288\n"
    "EPTL:288\n"
    "UI:content_size:272\n"
    "EPCL:272\n"
    "UI:custom:65244\n"
    "}\n"
    "}\n"
    "ER {\n"
    "ERT:0\n"
    "SC:5 {\n"
    "ST {\n"
    "ST:s {\n"
    "UI:a:16909060\n"
    "P {\n"
    "PC {\n"
    "SC:0 {\n"
    "ST {\n"
    "UI:magic:3254525889\n"
    "PMN:3254525889\n"
    "SA:uuid {\n"
    "UI:100\n"
    "UI:223\n"
    "UI:96\n"
    "UI:142\n"
    "UI:141\n"
    "UI:185\n"
    "UI:79\n"
    "UI:237\n"
    "UI:156\n"
    "UI:80\n"
    "UI:14\n"
    "UI:185\n"
    "UI:114\n"
    "UI:57\n"
    "UI:44\n"
    "UI:247\n"
    "TTU:64df608e-8db9-4fed-9c50-0eb972392cf7\n"
    "}\n"
    "UI:stream_id:35\n"
    "}\n"
    "}\n"
    "DST:35\n"
    "SC:1 {\n"
    "ST {\n"
    "UI:packet_size:352\n"
    "EPTL:352\n"
    "UI:content_size:352\n"
    "EPCL:352\n"
    "}\n"
    "}\n"
    "ER {\n"
    "ERT:0\n"
    "SC:5 {\n"
    "ST {\n"
    "STR:a {\n"
    "SS:1:s\n"
    "SS:3:alu\n"
    "SS:2:t\n";

int main()
{
    const auto trace = yactfr::traceFromMetadataText(metadata, metadata + std::strlen(metadata));
    MemDataSrcFactory factory {stream, sizeof stream, 3};
    yactfr::ElementSequence seq {trace->type(), factory};
    std::ostringstream ss;
    ElemPrinter printer {ss, 0};
    auto it = seq.begin();

    while (true) {
        if (it.offset() == 160) {
            // jump to third packet
            it.seekPacket(768 / 8);
        } else if (it.offset() == 1016) {
            // jump to second packet
            it.seekPacket(416 / 8);
        } else if (it.offset() == 664) {
            break;
        } else {
            it->accept(printer);
            ++it;
            continue;
        }
    }

    if (ss.str() == expected) {
        return 0;
    }

    std::cerr << "Expected:\n\n" << expected << "\n" <<
                 "Got:\n\n" << ss.str();
    return 1;
}
