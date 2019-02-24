#include "memory.h"

namespace NiohPracticeTools {

  bool lower_equals(const std::string& a, const std::string& b) {
    auto it_a = a.begin();
    auto it_b = b.begin();
    while (it_a != a.end() && it_b != b.end()) {
      if (tolower(*it_a) != tolower(*it_b)) return false;
      ++it_a; ++it_b;
    }
    return true;
  }

  std::tuple<DWORD, HMODULE, MODULEINFO> find_process(const std::string& name) {
    DWORD ret_pid = -1;
    HMODULE ret_base = 0;
    MODULEINFO ret_info;

    DWORD lpidProcess[256];
    unsigned long cbNeeded, count;
    HMODULE hModule[64];
    char modname[30];

    EnumProcesses(lpidProcess, sizeof(lpidProcess), &cbNeeded);
    int nReturned = cbNeeded / sizeof(cbNeeded);

    for (int i = 0; i < nReturned; i++) {
      auto pid = lpidProcess[i];
      auto hProc = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, false, pid);
      if(!hProc) continue;

      EnumProcessModules(hProc, hModule, sizeof(hModule), &count);
      GetModuleBaseNameA(hProc, hModule[0], modname, sizeof(modname));

      std::string procname(modname);
      if (lower_equals(procname, name)) {
        GetModuleInformation(hProc, hModule[0], &ret_info, sizeof(ret_info));
        ret_pid = pid;
        ret_base = hModule[0];
      }

      for (int j = 0; j < 30; j++) modname[j] = 0;
      CloseHandle(hProc);
    }

    return std::make_tuple(ret_pid, ret_base, ret_info);
  }

  uint64_t eval_pointer_chain (HANDLE proc, std::vector<uint64_t> chain) {
    auto addr = chain[0];
    try {
      for (auto it = ++chain.begin(); it != chain.end(); ++it) {
        addr = read<uint64_t>(proc, addr);
        addr = addr + *it;
      }
    } catch (memory_exception e) {
      throw memory_exception("Address can't be used right now");
    }
    return addr;
  }

  ////////////////////////////////////////////////////////////////////////////

  void Process::attach (DWORD pid_, HMODULE base_, MODULEINFO info_) {
    if (ph != nullptr) {
      CloseHandle(ph);
    }
    pid = pid_;
    base = (uint64_t)base_;
    info = info_;
    ph = OpenProcess(PROCESS_ALL_ACCESS, false, pid);

    if (!ph) {
      throw memory_exception("OpenProcess");
    }

    position = { base + 0x02403170, 0x0, 0x28, 0xf0 };

  }

  void Process::assert_attached () const {
    if (!is_attached()) {
      throw memory_exception("Process is not attached!");
    }
  }

  bool Process::is_attached () const {
    return ph != NULL;
  }

  Position Process::read_position () const {
    auto addr = eval_pointer_chain(ph, position);
    return read<Position>(ph, addr);
  }

  void Process::write_position (Position p) const {
    auto addr = eval_pointer_chain(ph, position);
    write<Position>(ph, addr, p);
  }

};