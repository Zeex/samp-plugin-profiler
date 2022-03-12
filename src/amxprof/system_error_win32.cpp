// Copyright (c) 2013-2016 Zeex
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.

#define WIN32_LEAN_AND_MEAN
#include <cstddef>
#include <cstring>
#include <windows.h>
#include "system_error.h"

static char *StripNewLine(char *s) {
  std::size_t length = std::strlen(s);

  for (std::size_t i = length; i > 0; i--) {
    if (s[i - 1] != '\n' && s[i - 1] != '\r') {
      break;
    }
    s[i - 1] = '\0';
  }

  return s;
}

static std::string GetErrorMessage(DWORD error) {
  std::string message;

  LPVOID buffer = NULL;
  DWORD flags = FORMAT_MESSAGE_ALLOCATE_BUFFER |
                FORMAT_MESSAGE_FROM_SYSTEM |
                FORMAT_MESSAGE_IGNORE_INSERTS;
  DWORD lang_id = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);

  DWORD result = FormatMessageA(flags, NULL, error, lang_id,
                                reinterpret_cast<LPSTR>(&buffer),
                                0, NULL);
  if (result > 0) {
    message.assign(StripNewLine(reinterpret_cast<char*>(buffer)));
  }

  LocalFree(buffer);

  return message;
}

namespace amxprof {

SystemError::SystemError(const char *prefix)
 : Exception(std::string(prefix) + std::string(": ") + GetErrorMessage(GetLastError())),
   code_(GetLastError())
{
}

SystemError::SystemError(const char *prefix, int code)
 : Exception(std::string(prefix) + std::string(": ") + GetErrorMessage(GetLastError())),
   code_(code)
{
}

} // namespace amxprof
