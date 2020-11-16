#pragma once

#include <boost/container_hash/hash.hpp>
#include <optional>
#include <string>
#include <unordered_set>

#include "opcodes.hpp"
#include "state.hpp"
#include "types.hpp"

class Analysis;
class Subroutine;

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
  Instruction(Analysis* analysis,
              u24 pc,
              u24 subroutine,
              u8 opcode,
              u24 argument,
              State state);
  // Test constructor.
  Instruction(u8 opcode);

  std::string name() const;         // Name of the instruction's operation.
  Op operation() const;             // Instruction's operation.
  AddressMode addressMode() const;  // Instruction'a address mode.
  InstructionType type() const;     // Category of the instruction.
  bool isControl() const;           // Whether this is a control instruction.
  size_t size() const;              // Instruction size.
  size_t argumentSize() const;      // Instruction's argument size.
  // Instruction's argument, if any.
  std::optional<u24> argument() const;
  // Instruction's argument as an absolute value, if possible.
  std::optional<u24> absoluteArgument() const;
  std::string argumentString() const;  // Instruction's argument as a string.
  std::string toString() const;        // Disassemble the instruction.

  // Hash table utils.
  bool operator==(const Instruction& other) const;
  friend std::size_t hash_value(const Instruction& instruction);

  u24 pc;          // Instruction's address.
  u24 subroutine;  // Subroutine to which the instruction belongs.
  // Instruction's label, if any.
  std::optional<std::string> label = std::nullopt;

 private:
  Analysis* analysis;  // Pointer to the analysis.
  u8 opcode;           // Opcode byte.
  u24 _argument;       // Argument (if any).
  State state;         // CPU state in which the instruction is executed.
};
// Set of Instructions.
typedef std::unordered_set<Instruction, boost::hash<Instruction>>
    InstructionSet;
