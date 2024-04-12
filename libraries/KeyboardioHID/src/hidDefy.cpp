/*
Copyright (c) 2023, 2024 Dygmalab S.L.

Copyright (c) 2015, Arduino LLC
Original code (pre-library): Copyright (c) 2011, Peter Barrett

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/


#include "hidDefy.h"
#include "Ble_composite_dev.h"
#include "DescriptorPrimitives.h"
#include "HIDAliases.h"
#include "HIDReportObserver.h"
#include "MultiReport/Keyboard.h"
#include "ble_hid_service.h"

HID_ &HID()
{
    static HID_ obj;
    return obj;
}

void HID_::AppendDescriptor(HIDSubDescriptor *node)
{
    //  descriptor.resize(descriptor.size() + node->length);
    //  for (int i = 0; i < node->length; ++i) {
    //	descriptor.push_back(node->data[i]);
    //  }
}

int HID_::SendReport(uint8_t id, const void *data, int len)
{
    auto result = SendReport_(id, data, len);
    HIDReportObserver::observeReport(id, data, len, result);
    return result;
}


tu_fifo_t tx_ff_hid;

uint8_t tx_ff_buf_hid[8000];

struct NextReport
{
    uint8_t id;
    uint16_t len;
};

int HID_::SendReport_(uint8_t id, const void *data, int len)
{
    /* On SAMD, we need to send the whole report in one batch; sending the id, and
     * the report itself separately does not work, the report never arrives. Due
     * to this, we merge the two into a single buffer, and send that.
     *
     * While the same would work for other architectures, AVR included, doing so
     * costs RAM, which is something scarce on AVR. So on that platform, we opt to
     * send the id and the report separately instead. */
#if defined(ARDUINO_ARCH_AVR)
    uint8_t p[64];
    p[0] = id;
    memcpy(&p[1], data, len);
    return USB_Send(pluggedEndpoint, p, len + 1);
#elseif AVR
    auto ret = USB_Send(pluggedEndpoint, &id, 1);
    if (ret < 0) return ret;
    auto ret2 = USB_Send(pluggedEndpoint | TRANSFER_RELEASE, data, len);
    if (ret2 < 0) return ret2;
    return ret + ret2;
#elif defined(ARDUINO_RASPBERRY_PI_PICO)

    while (!usb_hid.ready())
    {
        tight_loop_contents();
    }
    bool b = usb_hid.sendReport(id, data, len);
    return b;
#elif defined(ARDUINO_NRF52_ADAFRUIT)

    if (TinyUSBDevice.suspended())
    {
        TinyUSBDevice.remoteWakeup();
    }
    else if (TinyUSBDevice.mounted() || ble_connected())
    {
        NextReport nextReport{id, static_cast<uint16_t>(len)};
        tu_fifo_write_n(&tx_ff_hid, &nextReport, (uint16_t)(sizeof(nextReport)));
        tu_fifo_write_n(&tx_ff_hid, data, (uint16_t)len);
    }

    return 1;
#else

#error "Unsupported architecture"

#endif
}

bool HID_::SendLastReport()
{
    bool success = true;
    if (tu_fifo_count(&tx_ff_hid) != 0)
    {
        struct
        {
            NextReport nextReport;
            uint8_t dataReport[256];
        } nextReportWithData;
        tu_fifo_peek_n(&tx_ff_hid, &nextReportWithData.nextReport, (uint16_t)(sizeof(nextReportWithData.nextReport)));
        tu_fifo_peek_n(&tx_ff_hid, &nextReportWithData, (uint16_t)(sizeof(nextReportWithData.nextReport)) + nextReportWithData.nextReport.len);

        if (ble_connected())
            success = ble_send_report(nextReportWithData.nextReport.id, (uint8_t *const)nextReportWithData.dataReport, nextReportWithData.nextReport.len);
        else
            success = usb_hid.sendReport(nextReportWithData.nextReport.id, nextReportWithData.dataReport, nextReportWithData.nextReport.len);

        if (success || (ble_innited() && !ble_connected()) || (!ble_innited() && TinyUSBDevice.suspended()))
        {
            tu_fifo_advance_read_pointer(&tx_ff_hid, (uint16_t)(sizeof(nextReportWithData.nextReport)) + nextReportWithData.nextReport.len);
        }
    }
    return success;
}


enum
{
    REPORT_ID_KEYBOARD = 1,
    REPORT_ID_MOUSE,
    REPORT_ID_CONSUMER_CONTROL,
    REPORT_ID_SYSTEM_CONTROL,
    REPORT_ID_RAW
};

#define INPUT_REPORT_LEN_RAW 200  /**< Maximum length of the Input Report characteristic. */
#define OUTPUT_REPORT_LEN_RAW 200 /**< Maximum length of Output Report. */


// clang-format off
// Consumer Control Report Descriptor Template
#define TUD_HID_REPORT_DESC_CONSUMER_DYGMA(...) \
HID_USAGE_PAGE ( HID_USAGE_PAGE_CONSUMER    )              ,\
HID_USAGE      ( HID_USAGE_CONSUMER_CONTROL )              ,\
HID_COLLECTION ( HID_COLLECTION_APPLICATION )              , /* Report ID if any */\
 __VA_ARGS__ \
 HID_LOGICAL_MIN  ( 0x00                                ) ,\
 HID_LOGICAL_MAX_N( 0x03FF, 2                           ) ,\
 HID_USAGE_MIN    ( 0x00                                ) ,\
 HID_USAGE_MAX_N  ( 0x03FF, 2                           ) ,\
 HID_REPORT_COUNT ( 4                                   ) ,\
 HID_REPORT_SIZE  ( 16                                  ) ,\
 HID_INPUT        ( HID_DATA | HID_ARRAY | HID_ABSOLUTE ) ,\
HID_COLLECTION_END                            \
/*Enable back clang*/
// clang-format on

uint8_t const hid_report_descriptor[] = {
    D_USAGE_PAGE, D_PAGE_GENERIC_DESKTOP, D_USAGE, D_USAGE_KEYBOARD, D_COLLECTION, D_APPLICATION, D_REPORT_ID, HID_REPORTID_NKRO_KEYBOARD, D_USAGE_PAGE,
    D_PAGE_KEYBOARD,

    /* Key modifier byte */
    D_USAGE_MINIMUM, HID_KEYBOARD_FIRST_MODIFIER, D_USAGE_MAXIMUM, HID_KEYBOARD_LAST_MODIFIER, D_LOGICAL_MINIMUM, 0x00, D_LOGICAL_MAXIMUM, 0x01, D_REPORT_SIZE, 0x01,
    D_REPORT_COUNT, 0x08, D_INPUT, (D_DATA | D_VARIABLE | D_ABSOLUTE),

    /* 5 LEDs for num lock etc, 3 left for advanced, custom usage */
    D_USAGE_PAGE, D_PAGE_LEDS, D_USAGE_MINIMUM, 0x01, D_USAGE_MAXIMUM, 0x08, D_REPORT_COUNT, 0x08, D_REPORT_SIZE, 0x01, D_OUTPUT, (D_DATA | D_VARIABLE | D_ABSOLUTE),

    /* NKRO Keyboard */
    D_USAGE_PAGE, D_PAGE_KEYBOARD,

    // Padding 4 bits, to skip NO_EVENT & 3 error states.
    D_REPORT_SIZE, 0x04, D_REPORT_COUNT, 0x01, D_INPUT, (D_CONSTANT),

    D_USAGE_MINIMUM, HID_KEYBOARD_A_AND_A, D_USAGE_MAXIMUM, HID_LAST_KEY, D_LOGICAL_MINIMUM, 0x00, D_LOGICAL_MAXIMUM, 0x01, D_REPORT_SIZE, 0x01, D_REPORT_COUNT,
    (KEY_BITS - 4), D_INPUT, (D_DATA | D_VARIABLE | D_ABSOLUTE),

#if (KEY_BITS % 8)
    // Padding to round up the report to byte boundary.
    D_REPORT_SIZE, (8 - (KEY_BITS % 8)), D_REPORT_COUNT, 0x01, D_INPUT, (D_CONSTANT),
#endif

    D_END_COLLECTION, TUD_HID_REPORT_DESC_MOUSE(HID_REPORT_ID(REPORT_ID_MOUSE)), TUD_HID_REPORT_DESC_CONSUMER_DYGMA(HID_REPORT_ID(REPORT_ID_CONSUMER_CONTROL)),
    TUD_HID_REPORT_DESC_SYSTEM_CONTROL(HID_REPORT_ID(REPORT_ID_SYSTEM_CONTROL)),
    TUD_HID_REPORT_DESC_GENERIC_INOUT(OUTPUT_REPORT_LEN_RAW, HID_REPORT_ID(REPORT_ID_RAW))};


HID_::HID_() : protocol(HID_REPORT_PROTOCOL), idle(0)
{
    setReportData.reportId = 0;
    setReportData.leds = 0;
}

int HID_::begin()
{
    const uint8_t sizeRawHID[] = {TUD_HID_REPORT_DESC_GENERIC_INOUT(OUTPUT_REPORT_LEN_RAW, HID_REPORT_ID(REPORT_ID_RAW))};
    usb_hid.setPollInterval(1);
    usb_hid.setReportDescriptor(hid_report_descriptor, sizeof(hid_report_descriptor)-sizeof(sizeRawHID));
    ble_set_report_descriptor(hid_report_descriptor, sizeof(hid_report_descriptor));
    usb_hid.setBootProtocol(0);
    usb_hid.begin();
    tu_fifo_config(&tx_ff_hid, tx_ff_buf_hid, TU_ARRAY_SIZE(tx_ff_buf_hid), 1, true);
    return 0;
}
