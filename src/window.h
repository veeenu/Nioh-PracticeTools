#pragma once

#include <QPushButton>
#include <QMessageBox>
#include <QTextEdit>
#include <QLayout>

#include <QListView>
#include <QStringListModel>

#include <windows.h>

#include <string>
#include <thread>
#include <unordered_map>

#include <json.hpp>

#include "memory.h"

using namespace nlohmann;

namespace NiohPracticeTools {
  class Window : public QWidget {
    public:
      explicit Window(QWidget* parent = nullptr);
      void keyup(DWORD vk_code);
    private:
      QListView* positions_list_view;
      QTextEdit* pos_x;
      QTextEdit* pos_y;
      QTextEdit* pos_z;
      QPushButton* attach;
      QPushButton* load_pos;
      QStringList positions_list_view_qsl;
      QStringListModel positions_list_model;

      Process process;

      bool lock;
      std::thread position_updater;
      std::unordered_map<std::string, Position> positions;
  };
};