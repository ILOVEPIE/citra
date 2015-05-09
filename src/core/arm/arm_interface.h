// Copyright 2014 Citra Emulator Project
// Licensed under GPLv2 or any later version
// Refer to the license.txt file included.

#pragma once

#include "common/common_types.h"
#include "core/arm/skyeye_common/arm_regformat.h"

namespace Core {
    struct ThreadContext;
}

/// Generic ARM11 CPU interface
class ARM_Interface : NonCopyable {
public:
    ARM_Interface() {
        num_instructions = 0;
    }

    virtual ~ARM_Interface() {
    }

    /**
     * Runs the CPU for the given number of instructions
     * @param num_instructions Number of instructions to run
     */
    void Run(int num_instructions) {
        ExecuteInstructions(num_instructions);
        this->num_instructions += num_instructions;
    }

    /// Step CPU by one instruction
    void Step() {
        Run(1);
    }

    /**
     * Set the Program Counter to an address
     * @param addr Address to set PC to
     */
    virtual void SetPC(u32 addr) = 0;

    /*
     * Get the current Program Counter
     * @return Returns current PC
     */
    virtual u32 GetPC() const = 0;

    /**
     * Get an ARM register
     * @param index Register index (0-15)
     * @return Returns the value in the register
     */
    virtual u32 GetReg(int index) const = 0;

    /**
     * Set an ARM register
     * @param index Register index (0-15)
     * @param value Value to set register to
     */
    virtual void SetReg(int index, u32 value) = 0;

    /**
     * Get the current CPSR register
     * @return Returns the value of the CPSR register
     */
    virtual u32 GetCPSR() const = 0;

    /**
     * Set the current CPSR register
     * @param cpsr Value to set CPSR to
     */
    virtual void SetCPSR(u32 cpsr) = 0;

    /**
     * Gets the value stored in a CP15 register.
     * @param reg The CP15 register to retrieve the value from.
     * @return the value stored in the given CP15 register.
     */
    virtual u32 GetCP15Register(CP15Register reg) = 0;

    /**
     * Stores the given value into the indicated CP15 register.
     * @param reg   The CP15 register to store the value into.
     * @param value The value to store into the CP15 register.
     */
    virtual void SetCP15Register(CP15Register reg, u32 value) = 0;

    /**
     * Advance the CPU core by the specified number of ticks (e.g. to simulate CPU execution time)
     * @param ticks Number of ticks to advance the CPU core
     */
    virtual void AddTicks(u64 ticks) = 0;

    /**
     * Initializes a CPU context for use on this CPU
     * @param context Thread context to reset
     * @param stack_top Pointer to the top of the stack
     * @param entry_point Entry point for execution
     * @param arg User argument for thread
     */
    virtual void ResetContext(Core::ThreadContext& context, u32 stack_top, u32 entry_point, u32 arg) = 0;

    /**
     * Saves the current CPU context
     * @param ctx Thread context to save
     */
    virtual void SaveContext(Core::ThreadContext& ctx) = 0;

    /**
     * Loads a CPU context
     * @param ctx Thread context to load
     */
    virtual void LoadContext(const Core::ThreadContext& ctx) = 0;

    /// Prepare core for thread reschedule (if needed to correctly handle state)
    virtual void PrepareReschedule() = 0;

    /// Getter for num_instructions
    u64 GetNumInstructions() {
        return num_instructions;
    }

    s64 down_count; ///< A decreasing counter of remaining cycles before the next event, decreased by the cpu run loop

protected:

    /**
     * Executes the given number of instructions
     * @param num_instructions Number of instructions to executes
     */
    virtual void ExecuteInstructions(int num_instructions) = 0;

private:

    u64 num_instructions; ///< Number of instructions executed

};
