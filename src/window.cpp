#include "window.h"

#include <iostream>
#include <fstream>

namespace NiohPracticeTools {
  
  std::unordered_map<std::string, Position> load_from_json (std::string filename) {
    std::ifstream ifs(filename);
    json j;
    ifs >> j;

    std::unordered_map<std::string, Position> p;
    for(json::iterator it = j.begin(); it != j.end(); ++it) {
      auto v = it.value();
      Position pos = { v[0], v[1], v[2] };
      p.emplace(it.key(), pos);
    }
    return p;
  }

  Window::Window (QWidget* parent) 
    : QWidget(parent), positions_list_model(this), lock(false) {

    setFixedSize(400, 440);

    try {
      positions = std::move(load_from_json("../positions.json"));
    } catch (std::exception e) {
      QMessageBox er;
      er.setText(e.what());
      er.exec();
    }

    positions_list_view = new QListView(this);
    positions_list_view->setGeometry(0, 0, 400, 416);
    std::vector<std::string> position_names;
    for (auto& kv : positions) {
      position_names.push_back(kv.first);
    }

    std::sort(position_names.begin(), position_names.end());
    for (auto& p : position_names) {
      positions_list_view_qsl << p.c_str();
    }
    positions_list_model.setStringList(positions_list_view_qsl);
    positions_list_view->setModel(&positions_list_model);

    attach = new QPushButton(this);
    attach->setGeometry(0, 416, 100, 24);
    attach->setText("Connect to Nioh");

    connect(attach, &QPushButton::clicked, [this] () {
      std::thread([this] () {
        attach->setText("Attaching...");
        std::this_thread::sleep_for(std::chrono::milliseconds(2000));

        auto params = find_process("nioh.exe");
        process.attach(std::get<0>(params), std::get<1>(params), std::get<2>(params));

        if (process.is_attached()) {
          attach->setText("Connected");
        } else {
          attach->setText("Failed! Try again");
          std::this_thread::sleep_for(std::chrono::milliseconds(2000));
          attach->setText("Connect to Nioh");
        }
      });
    });

    load_pos = new QPushButton(this);
    load_pos->setGeometry(100, 416, 100, 24);
    load_pos->setText("Load Position");

    connect(load_pos, &QPushButton::clicked, [this] () {
      QMessageBox qmb;
      auto i = positions_list_view->currentIndex();
      auto s = positions_list_model.data(i).toString().toStdString();
      auto kv = positions.find(s);
      auto& p = (*kv).second;
      qmb.setText(tfm::format("%s %f %f %f", (const char*)s.data(), p.x, p.y, p.z).c_str());
      qmb.exec();
    });

    pos_x = new QTextEdit(this);
    pos_y = new QTextEdit(this);
    pos_z = new QTextEdit(this);
    pos_x->setGeometry(200, 416, 66, 24);
    pos_y->setGeometry(267, 416, 66, 24);
    pos_z->setGeometry(334, 416, 66, 24);
    pos_x->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    pos_y->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);
    pos_z->setTextInteractionFlags(Qt::TextSelectableByMouse | Qt::TextSelectableByKeyboard);

    position_updater = std::thread([this] () {
      while (true) {
        if (!lock && process.is_attached()) {
          try {
            lock = true;
            auto t = process.read_position();
            pos_x->setText(tfm::format("%f", t.x).c_str());
            pos_y->setText(tfm::format("%f", t.y).c_str());
            pos_z->setText(tfm::format("%f", t.z).c_str());
            lock = false;
          } catch (memory_exception e) {
            lock = false;
          }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(33));
      }
    });
  }

  void Window::keyup (DWORD vk_code) {
    int r;
    switch (vk_code) {
      case VK_HOME:
        load_pos->animateClick();
        break;
      case VK_F11:
        r = positions_list_view->currentIndex().row();
        positions_list_view->setCurrentIndex(positions_list_model.index(r + 1, 0));
        printf("%d\n", r);
        break;
    }
  }
}