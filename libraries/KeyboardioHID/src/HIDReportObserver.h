/*
Copyright (c) 2015-2019 Keyboard.io, Inc
Copyright (c) 2023, 2024 Dygmalab S.L.

See the readme for credit to other people.

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

#pragma once

#include <stdint.h>

class HIDReportObserver {
 public:

  typedef void(*SendReportHook)(uint8_t id, const void* data,
                                int len, int result);

  static void observeReport(uint8_t id, const void* data,
                            int len, int result) {
    if (send_report_hook_) {
      (*send_report_hook_)(id, data, len, result);
    }
  }

  static SendReportHook currentHook() {
    return send_report_hook_;
  }

  static SendReportHook resetHook(SendReportHook new_hook) {
    auto previous_hook = send_report_hook_;
    send_report_hook_ = new_hook;
    return previous_hook;
  }

 private:

  static SendReportHook send_report_hook_;
};
