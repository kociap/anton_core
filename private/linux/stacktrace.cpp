#include <anton/stacktrace.hpp>

namespace anton::stacktrace {
    Array<Frame_Record*> capture_call_stack() {
        return Array<Frame_Record*>{};
    }

    Stack_Frame resolve_frame_record(Frame_Record* record) {
        return Stack_Frame{};
    }
}
