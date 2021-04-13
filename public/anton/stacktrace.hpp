#pragma once

#include <anton/array.hpp>
#include <anton/intrinsics.hpp>
#include <anton/string.hpp>
#include <anton/types.hpp>

namespace anton::stacktrace {
    struct Frame_Record;

    struct Stack_Frame {
        String function;
        i64 line;
    };

    // capture_call_stack
    // Capture the current call stack.
    // This function may not be used across multiple processes.
    // Using it from a different process than the one it was first
    // called from will result in undefined behaviour.
    //
    // Returns:
    // Array containing call stack frame records.
    // The first element is the bottom frame, the last element is the top frame.
    //
    ANTON_NOINLINE Array<Frame_Record*> capture_call_stack();

    // resolve_frame_record
    // Obtains information about the symbol stored in record.
    // This function may not be used across multiple processes.
    // Using it from a different process than the one it was first
    // called from will result in undefined behaviour.
    //
    // Parameters:
    // record - the frame record to resolve.
    //
    // Returns:
    // Stack_Frame containing the symbol information about the stack frame stored in record.
    //
    Stack_Frame resolve_frame_record(Frame_Record* record);
} // namespace anton::stacktrace
