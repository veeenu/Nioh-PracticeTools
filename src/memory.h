#pragma once

#include <windows.h>
#include <Psapi.h>
#include <algorithm>
#include <vector>
#include <tuple>
#include <string>
#include <cstdint>
#include <tinyformat.h>

namespace NiohPracticeTools {

  class Process;
  typedef void (Process::*ProcessFlagSetter) (bool);

  std::tuple<DWORD, HMODULE, MODULEINFO> find_process (const std::string& name);

  class memory_exception : public std::domain_error {
    using std::domain_error::domain_error;
  };

  template<typename T> T read (HANDLE proc, uint64_t addr) {
    T dest;
    uint64_t bytes_read;
    auto ret = ReadProcessMemory(proc, (void*)addr, &dest, sizeof(T), &bytes_read);
    if (ret == 0) {
      throw memory_exception(tfm::format("ReadProcessMemory: %x", GetLastError()));
    }
    return dest;
  }

  template<typename T> void write (HANDLE proc, uint64_t addr, T data) {
    uint64_t bytes_written;
    auto ret = WriteProcessMemory (proc, (void*)addr, &data, sizeof(T), &bytes_written);
    if (ret == 0) {
      throw memory_exception(tfm::format("ReadProcessMemory: %x", GetLastError()));
    }
  }

  uint64_t eval_pointer_chain (HANDLE proc, std::vector<uint64_t> chain);

  struct Position {
    float x;
    float y;
    float z;
  };

  class Process {
    private:
      DWORD pid;
      uint64_t base;
      MODULEINFO info;
      HANDLE ph;

      std::vector<uint64_t> position;
    public:
      Process () = default;
      void attach (DWORD, HMODULE, MODULEINFO);
      void assert_attached () const;
      bool is_attached () const;

      Position read_position() const;
      void write_position(Position p) const;
  };
};