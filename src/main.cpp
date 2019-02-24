#include <QApplication>
#include <windows.h>
#include "window.h"

NiohPracticeTools::Window* window;

int main(int argc, char** argv) {
  QApplication app(argc, argv);

  window = new NiohPracticeTools::Window();
  window->show();

  auto hook = SetWindowsHookEx(WH_KEYBOARD_LL, [](int nCode, WPARAM wParam, LPARAM lParam)->LRESULT {
    if (nCode == HC_ACTION) {
      switch (wParam) {
        case WM_KEYDOWN:
          // Don't really care, should just slim this down to
          // an "if ncode == HC_ACTION && wParam == WM_KEYUP"
          break;
        case WM_KEYUP:
          window->keyup(PKBDLLHOOKSTRUCT(lParam)->vkCode);
          break;
      }
    }
    return CallNextHookEx(NULL, nCode, wParam, lParam);
  }, 0, 0);


  return app.exec();
}