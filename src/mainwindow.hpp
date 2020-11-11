#pragma once

#include <QMainWindow>

class QTextEdit;
class ROM;

class MainWindow : public QMainWindow {
  Q_OBJECT

 public:
  MainWindow(QWidget* parent = nullptr);

 public slots:
  void openROM(const QString& path = QString());

 private:
  void setupEditor();
  void setupFileMenu();

  QTextEdit* editor;
  ROM* rom;
};