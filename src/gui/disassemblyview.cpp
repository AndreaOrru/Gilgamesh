#include "gui/disassemblyview.hpp"

#include "analysis.hpp"
#include "utils.hpp"

DisassemblyView::DisassemblyView(QWidget* parent) : QTextEdit(parent) {
  setFontFamily("monospace");
  setReadOnly(true);
}

void DisassemblyView::setAnalysis(const Analysis* analysis) {
  clear();

  for (auto& [pc, subroutine] : analysis->subroutines) {
    renderSubroutine(subroutine);
  }
}

void DisassemblyView::renderSubroutine(const Subroutine& subroutine) {
  auto label = qformat("%s:", subroutine.label.c_str());
  append(label);

  for (auto& [pc, instruction] : subroutine.instructions) {
    renderInstruction(instruction);
  }
  append("");
}

void DisassemblyView::renderInstruction(const Instruction* instruction) {
  auto code = qformat("  %-30s; $%06X | %s", instruction->toString().c_str(),
                      instruction->pc, "");
  append(code);
}