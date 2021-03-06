#pragma once

#include <QRegularExpression>
#include <QSyntaxHighlighter>

class Highlighter : public QSyntaxHighlighter {
  Q_OBJECT

 public:
  Highlighter(QTextDocument* parent = nullptr);

 protected:
  void highlightBlock(const QString& text) override;

 private:
  void setupFormats();
  void setupPatterns();

  struct Rule {
    QRegularExpression pattern;
    QTextCharFormat format;
  };
  QVector<Rule> rules;

  QTextCharFormat argumentAliasFormat;
  QTextCharFormat assertedStateChangeFormat;
  QTextCharFormat commentFormat;
  QTextCharFormat completeJumpTableFormat;
  QTextCharFormat entryPointFormat;
  QTextCharFormat labelFormat;
  QTextCharFormat localLabelFormat;
  QTextCharFormat opcodeFormat;
  QTextCharFormat partialJumpTableFormat;
  QTextCharFormat unknownStateChangeFormat;
};
