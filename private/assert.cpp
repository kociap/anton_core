#include <anton/assert.hpp>

#include <anton/format.hpp>
#include <anton/intrinsics.hpp>
#include <anton/stacktrace.hpp>
#include <anton/string.hpp>

namespace anton {
    void anton_assert(char8 const* message, char8 const* file, u64 line) {
        String dialog_text = format(u8"Assertion failed:\n{}\nin file {} on line {}\n\nStack trace:\n"_sv, message, file, line);
        Array<stacktrace::Frame_Record*> frame_records = stacktrace::capture_call_stack();
        for(stacktrace::Frame_Record* record: frame_records) {
            stacktrace::Stack_Frame frame = stacktrace::resolve_frame_record(record);
            dialog_text += frame.function;
            dialog_text += u8" line "_sv;
            dialog_text += to_string(frame.line);
            dialog_text += u8"\n"_sv;
        }

        // Print to error stream of the console
        fprintf(stderr, "%s", dialog_text.data());
        fflush(stderr);

        ANTON_DEBUG_BREAK();

        // If the above methods fail (although they shouldn't), we enter an infinite loop to never return.
        while(true) {}
    }
} // namespace anton
