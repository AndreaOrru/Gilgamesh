#pragma once

#include <map>
#include <string>

#include "state.hpp"
#include "types.hpp"

class Instruction;

class Subroutine {
 public:
  // Constructor.
  Subroutine(SubroutinePC pc, std::string label);

  // Add an instruction.
  void addInstruction(Instruction* instruction);

  // Add a state change.
  void addStateChange(InstructionPC pc, StateChange stateChange);

  SubroutinePC pc;    // Program Counter.
  std::string label;  // Label.
  // Map from PC to instructions.
  std::map<InstructionPC, Instruction*> instructions;

  // Known state changes.
  StateChangeMap knownStateChanges;

  // Unknown state changes.
  StateChangeMap unknownStateChanges;
};
