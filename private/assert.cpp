#include <anton/assert.hpp>

#include <anton/format.hpp>
#include <anton/stacktrace.hpp>
#include <anton/string.hpp>

#include <Windows.h>
#include <stdio.h>

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

        dialog_text += u8"\nPress 'Retry' to break into debug mode."_sv;
        int clicked_button = MessageBoxA(nullptr, dialog_text.data(), "Assertion Failed", MB_ABORTRETRYIGNORE | MB_TASKMODAL);
        if(clicked_button == IDRETRY) {
            DebugBreak();
        } else {
            HANDLE current_process = GetCurrentProcess();
            // TODO: Exit code (what does 900 mean?)
            TerminateProcess(current_process, 900);
        }

        // If the above methods fail (although they shouldn't), we enter an infinite loop to never return.
        while(true) {}
    }
} // namespace anton
