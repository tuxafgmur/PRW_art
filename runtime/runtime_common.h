/*
 * Copyright (C) 2017 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef ART_RUNTIME_RUNTIME_COMMON_H_
#define ART_RUNTIME_RUNTIME_COMMON_H_

// Code shared by runtime/runtime_android.cc and runtime/runtime_linux.cc.

#include <sys/utsname.h>
#include <ucontext.h>

#include <iomanip>

#include "base/dumpable.h"
#include "native_stack_dump.h"
#include "utils.h"

namespace art {

struct Backtrace {
 public:
  explicit Backtrace(void* raw_context) : raw_context_(raw_context) {}
  void Dump(std::ostream& os) const {
    DumpNativeStack(os, GetTid(), nullptr, "\t", nullptr, raw_context_);
  }
 private:
  // Stores the context of the signal that was unexpected and will terminate the runtime. The
  // DumpNativeStack code will take care of casting it to the expected type. This is required
  // as our signal handler runs on an alternate stack.
  void* raw_context_;
};

struct OsInfo {
  void Dump(std::ostream& os) const {
    utsname info;
    uname(&info);
    // Linux 2.6.38.8-gg784 (x86_64)
    // Darwin 11.4.0 (x86_64)
    os << info.sysname << " " << info.release << " (" << info.machine << ")";
  }
};

const char* GetSignalName(int signal_number);
const char* GetSignalCodeName(int signal_number, int signal_code);

struct UContext {
  explicit UContext(void* raw_context)
      : context(reinterpret_cast<ucontext_t*>(raw_context)->uc_mcontext) {}

  void Dump(std::ostream& os) const;

  void DumpRegister32(std::ostream& os, const char* name, uint32_t value) const;
  void DumpRegister64(std::ostream& os, const char* name, uint64_t value) const;

  void DumpX86Flags(std::ostream& os, uint32_t flags) const;

  mcontext_t& context;
};

// Return the signal number we recognize as timeout. -1 means not active/supported.
int GetTimeoutSignal();

void HandleUnexpectedSignalCommon(int signal_number,
                                  siginfo_t* info,
                                  void* raw_context,
                                  bool running_on_linux);

void InitPlatformSignalHandlersCommon(void (*newact)(int, siginfo_t*, void*),
                                      struct sigaction* oldact,
                                      bool handle_timeout_signal);

}  // namespace art

#endif  // ART_RUNTIME_RUNTIME_COMMON_H_
