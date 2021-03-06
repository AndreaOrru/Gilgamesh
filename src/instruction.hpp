#pragma once

#include <boost/container_hash/hash.hpp>
#include <optional>
#include <string>
#include <unordered_set>
#include <utility>

#include "assertion.hpp"
#include "label.hpp"
#include "opcodes.hpp"
#include "state.hpp"
#include "types.hpp"

class Analysis;
class JumpTable;
class Subroutine;

// Instruction coordinates (PC, subroutine's PC).
typedef std::pair<InstructionPC, SubroutinePC> PCPair;

// Categories of instructions.
enum class InstructionType {
  Branch,
  Call,
  Interrupt,
  Other,
  Jump,
  Pop,
  Push,
  Return,
  SepRep,
};

// Structure representing an instruction.
class Instruction {
 public:
  // Constructor.
  Instruction(InstructionPC pc,
              SubroutinePC subroutinePC,
              u8 opcode,
              u24 argument,
              State state,
              Analysis* analysis = nullptr);
  // Test constructor.
  Instruction(u8 opcode);

  std::string name() const;         // Name of the instruction's operation.
  Op operation() const;             // Instruction's operation.
  AddressMode addressMode() const;  // Instruction'a address mode.
  InstructionType type() const;     // Category of the instruction.
  bool changesA() const;            // Whether the instruction modifies A.
  bool changesX() const;            // Whether the instruction modifies X.
  // Whether the instruction modifies the stack pointer.
  bool changesStackPointer() const;
  bool isControl() const;       // Whether this is a control instruction.
  bool isSepRep() const;        // Whether this is a SEP/REP instruction.
  size_t size() const;          // Instruction size.
  size_t argumentSize() const;  // Instruction's argument size.
  // Instruction's argument, if any.
  std::optional<u24> argument() const;
  // Instruction's argument as an absolute value, if possible.
  std::optional<u24> absoluteArgument() const;
  // Instruction argument as a label, if any.
  std::optional<Label> argumentLabel() const;
  // Instruction's argument as a string.
  std::string argumentString(bool aliased = true) const;
  // Return the state change caused by this instruction, if any.
  std::optional<StateChange> stateChange() const;

  // Get an assertion for the instruction, if any.
  std::optional<Assertion> assertion() const;
  // Get the jumptable associated with the instruction, if any.
  const JumpTable* jumpTable() const;
  // Get the instruction's coordinates (PC, subroutine's PC).
  PCPair pcPair() const;
  // Pointer to the subroutine to which the instruction belongs.
  Subroutine* subroutine() const;

  std::string comment() const;           // Return the instruction's comment.
  void setComment(std::string comment);  // Set the instruction's comment.

  // Hash table utils.
  bool operator==(const Instruction& other) const;
  friend std::size_t hash_value(const Instruction& instruction);

  Analysis* analysis;          // Pointer to the analysis.
  InstructionPC pc;            // Instruction's address.
  SubroutinePC subroutinePC;   // Subroutine to which the instruction belongs.
  u8 opcode;                   // Opcode byte.
  State state;                 // State in which the instruction is executed.
  std::optional<Label> label;  // Instruction's label, if any.

 private:
  u24 _argument;  // Argument (if any).
};
// Set of Instructions.
typedef std::unordered_set<Instruction, boost::hash<Instruction>>
    InstructionSet;
