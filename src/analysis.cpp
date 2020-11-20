#include "analysis.hpp"

#include "cpu.hpp"
#include "utils.hpp"

using namespace std;

/****************
 *  EntryPoint  *
 ****************/

// Hash table utils.
bool EntryPoint::operator==(const EntryPoint& other) const {
  return pc == other.pc;
}
size_t hash_value(const EntryPoint& entryPoint) {
  return entryPoint.pc;
}

/***************
 *  Reference  *
 ***************/

// Hash table utils.
bool Reference::operator==(const Reference& other) const {
  return target == other.target && subroutinePC == other.subroutinePC;
}
size_t hash_value(const Reference& reference) {
  size_t seed = 0;
  boost::hash_combine(seed, reference.target);
  boost::hash_combine(seed, reference.subroutinePC);
  return seed;
}

/**************
 *  Analysis  *
 **************/

// Construct an empty analysis.
Analysis::Analysis() {}

// Construct an analysis from a ROM object.
Analysis::Analysis(const ROM& rom) : rom{rom} {
  entryPoints = {
      {.label = "reset", .pc = rom.resetVector(), .state = State()},
      {.label = "nmi", .pc = rom.nmiVector(), .state = State()},
  };
}

// Construct an analysis from a ROM path.
Analysis::Analysis(const std::string& romPath) {
  Analysis(ROM(romPath));
}

// Clear the results of the analysis.
void Analysis::clear() {
  instructions.clear();
  subroutines.clear();
  references.clear();
}

// Analyze the ROM.
void Analysis::run() {
  clear();

  for (auto& e : entryPoints) {
    addSubroutine(e.pc, e.label);
    CPU cpu(this, e.pc, e.pc, e.state);
    cpu.run();
  }

  generateLocalLabels();
}

// Add an instruction to the analysis.
Instruction* Analysis::addInstruction(InstructionPC pc,
                                      SubroutinePC subroutinePC,
                                      u8 opcode,
                                      u24 argument,
                                      State state) {
  // Retrieve the set of instructions for the given PC, or create a new one.
  auto& instructionSet = instructions.try_emplace(pc).first->second;
  // Insert the given instruction into the set.
  auto [instructionIter, inserted] =
      instructionSet.emplace(this, pc, subroutinePC, opcode, argument, state);
  // If the instruction was already present, return NULL.
  if (!inserted) {
    return nullptr;
  }

  // Add the instruction to its subroutine.
  auto& subroutine = subroutines.at(subroutinePC);
  auto instructionPtr = (Instruction*)&(*instructionIter);
  subroutine.addInstruction(instructionPtr);
  // Return a pointer to the new instruction.
  return instructionPtr;
}

// Add a reference from an instruction to another.
void Analysis::addReference(InstructionPC source,
                            InstructionPC target,
                            SubroutinePC subroutinePC) {
  auto& referenceSet = references.try_emplace(source).first->second;
  referenceSet.insert({target, subroutinePC});
}

// Add a subroutine to the analysis.
void Analysis::addSubroutine(SubroutinePC pc, optional<string> label) {
  auto labelValue = label.value_or(format("sub_%06X", pc));
  subroutines.try_emplace(pc, pc, labelValue);
}

// Define a jump table: caller spans a jumptable going from x to y (included).
void Analysis::defineJumpTable(InstructionPC callerPC,
                               pair<u16, u16> range,
                               JumpTableStatus status) {
  auto& jumpTable = jumpTables.at(callerPC);
  auto caller = anyInstruction(callerPC);

  // TODO: support stepping by 3 for long addresses.
  for (int x = range.first; x <= range.second; x += 2) {
    auto offset = *caller->argument() + x;
    auto bank = caller->pc & 0xFF0000;
    auto target = bank | rom.readWord(bank | offset);
    jumpTable.targets.insert_or_assign(x, target);
  }
  jumpTable.status = status;
}

// Return any of the instructions at address PC.
const Instruction* Analysis::anyInstruction(InstructionPC pc) {
  auto search = instructions.find(pc);
  if (search == instructions.end()) {
    return nullptr;
  } else {
    return &(*search->second.begin());
  }
}

// Get an assertion for the current instruction, if any.
optional<Assertion> Analysis::getAssertion(InstructionPC pc,
                                           SubroutinePC subroutinePC) const {
  auto instructionAssertion = instructionAssertions.find(pc);
  if (instructionAssertion != instructionAssertions.end()) {
    return optional(
        Assertion{AssertionType::Instruction, instructionAssertion->second});
  }

  auto subroutineAssertion = subroutineAssertions.find({pc, subroutinePC});
  if (subroutineAssertion != subroutineAssertions.end()) {
    return optional(
        Assertion{AssertionType::Subroutine, subroutineAssertion->second});
  }

  return nullopt;
}

// Generate local label names.
void Analysis::generateLocalLabels() {
  for (auto& [source, referenceSet] : references) {
    for (auto& [target, subroutinePC] : referenceSet) {
      if (subroutines.count(target) == 0) {
        auto& subroutine = subroutines.at(subroutinePC);
        auto& instruction = subroutine.instructions.at(target);
        instruction->label = format("loc_%06X", target);
      }
    }
  }
}
