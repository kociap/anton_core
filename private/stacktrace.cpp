#include <anton/stacktrace.hpp>

#include <anton/memory.hpp>
#include <anton/optional.hpp>
#include <anton/owning_ptr.hpp>

// We need SymTagEnum and this define allows us to access it without including cvconst.h (???)
#define _NO_CVCONST_H
#include <Windows.h>
// Windows.h must be included before DbgHelp.h. Otherwise we get undefined identifiers in DbgHelp.h
#include <DbgHelp.h>
#include <debugapi.h>

// TODO: add locking to ensure thread safety because all the functions are singlethreaded

namespace anton::stacktrace {
    static HANDLE process_handle = nullptr;

    static void ensure_initialize_symbol_handler() {
        static bool initialized = false;
        if(!initialized) {
            process_handle = GetCurrentProcess();
            SymInitialize(process_handle, nullptr, true);
            DWORD current_options = SymGetOptions();
            SymSetOptions(current_options | SYMOPT_LOAD_LINES | SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
            initialized = true;
        }
    }

    Array<Frame_Record*> capture_call_stack() {
        ensure_initialize_symbol_handler();

        Array<Frame_Record*> records{reserve, 1024};
        i64 const captured_frames = RtlCaptureStackBackTrace(1, 1024, (void**)records.data(), nullptr);
        records.force_size(captured_frames);
        return records;
    }

    [[nodiscard]] static String get_type_as_string(HANDLE process, ULONG64 module_base, ULONG64 index);

    [[nodiscard]] static String process_base_type(HANDLE const process, ULONG64 const module_base, ULONG64 const index) {
        ULONG64 length = 0;
        if(!SymGetTypeInfo(process, module_base, index, TI_GET_LENGTH, &length)) {
            return u8"unknown_type"_s;
        }

        DWORD base_type = 0;
        if(!SymGetTypeInfo(process, module_base, index, TI_GET_BASETYPE, &base_type)) {
            return u8"unknown_type"_s;
        }

        // Specifies symbol's basic type.
        // Copied from cvconst.h
        //
        // https://docs.microsoft.com/en-us/visualstudio/debugger/debug-interface-access/basictype
        //
        enum BasicType {
            btNoType = 0,
            btVoid = 1,
            btChar = 2,
            btWChar = 3,
            btInt = 6,
            btUInt = 7,
            btFloat = 8,
            btBCD = 9,
            btBool = 10,
            btLong = 13,
            btULong = 14,
            btCurrency = 25,
            btDate = 26,
            btVariant = 27,
            btComplex = 28,
            btBit = 29,
            btBSTR = 30,
            btHresult = 31,
            btChar16 = 32, // char16_t
            btChar32 = 33, // char32_t
            btChar8 = 34,  // char8_t
        };

        switch(base_type) {
            case btVoid:
                return u8"void"_s;
            case btBool:
                return u8"bool"_s;
            case btChar:
                return u8"char"_s;
            case btWChar:
                return u8"wchar_t"_s;
            case btChar8:
                return u8"char8_t"_s;
            case btChar16:
                return u8"char16_t"_s;
            case btChar32:
                return u8"char32_t"_s;
            case btInt:
                if(length == 1) {
                    return u8"char"_s;
                } else if(length == 2) {
                    return u8"_short"_s;
                } else if(length == 8) {
                    return u8"long long"_s;
                } else {
                    return u8"int"_s;
                }
            case btUInt:
                if(length == 1) {
                    return u8"unsigned char"_s;
                } else if(length == 2) {
                    return u8"unsigned short"_s;
                } else if(length == 8) {
                    return u8"unsigned long long"_s;
                } else {
                    return u8"unsigned int"_s;
                }
            case btLong:
                return u8"long"_s;
            case btULong:
                return u8"unsigned long"_s;
            case btFloat:
                if(length == 0 || length == 4) {
                    return u8"float"_s;
                } else {
                    return u8"double"_s;
                }

            default:
                return u8"unknown_type"_s;
        }
    }

    [[nodiscard]] static String process_typedef(HANDLE const process, ULONG64 const module_base, ULONG64 const index) {
        if(WCHAR* typedef_name = nullptr; SymGetTypeInfo(process, module_base, index, TI_GET_SYMNAME, &typedef_name)) {
            String name = String::from_utf16(reinterpret_cast<char16_t*>(typedef_name));
            LocalFree(typedef_name);
            return name;
        } else {
            return u8"unknown_typedef"_s;
        }
    }

    [[nodiscard]] static String process_pointer_type(HANDLE const process, ULONG64 const module_base, ULONG64 const index) {
        bool is_reference = false;
        // For some bizzare reason `process` changes after the call to SymGetTypeInfo,
        // so we make a copy of the valid value and use it instead.
        HANDLE const prcs = process;
        if(!SymGetTypeInfo(process, module_base, index, TI_GET_IS_REFERENCE, &is_reference)) {
            return u8"unknown_type"_s;
        }

        if(DWORD type_index = 0; SymGetTypeInfo(prcs, module_base, index, TI_GET_TYPEID, &type_index)) {
            String subtype = get_type_as_string(prcs, module_base, type_index);
            if(is_reference) {
                subtype += U'&';
            } else {
                subtype += U'*';
            }
            return subtype;
        } else {
            if(is_reference) {
                return u8"unknown_type&"_s;
            } else {
                return u8"unknown_type*"_s;
            }
        }
    }

    [[nodiscard]] static String process_function_type(HANDLE const process, ULONG64 const module_base, ULONG64 const index, String_View name) {
        String type_string;
        // Return type
        if(DWORD return_type_index = 0; SymGetTypeInfo(process, module_base, index, TI_GET_TYPEID, &return_type_index)) {
            String const return_type = get_type_as_string(process, module_base, return_type_index);
            type_string += return_type;
        } else {
            type_string += u8"unknown_type"_sv;
        }

        type_string += u8" "_sv;
        type_string += name;

        // Parameters
        DWORD parameters_count = 0;
        if(!SymGetTypeInfo(process, module_base, index, TI_GET_CHILDRENCOUNT, &parameters_count)) {
            type_string += u8"()"_sv;
            return type_string;
        }

        u64 const alloc_size = sizeof(TI_FINDCHILDREN_PARAMS) + sizeof(ULONG) * parameters_count;
        Owning_Ptr<void> children_params_memory = allocate(alloc_size, alignof(TI_FINDCHILDREN_PARAMS));
        TI_FINDCHILDREN_PARAMS& children_params = *reinterpret_cast<TI_FINDCHILDREN_PARAMS*>(children_params_memory.get());
        memset(&children_params, 0, alloc_size);
        children_params.Count = parameters_count;
        if(!SymGetTypeInfo(process, module_base, index, TI_FINDCHILDREN, &children_params)) {
            type_string += u8"()"_sv;
            return type_string;
        }

        type_string += U'(';
        for(u64 i = children_params.Start; i < children_params.Count; ++i) {
            String const argument = get_type_as_string(process, module_base, children_params.ChildId[i]);
            type_string += argument;
            if(i + 1 < children_params.Count) {
                type_string += u8", "_sv;
            }
        }
        type_string += U')';

        return type_string;
    }

    [[nodiscard]] static String process_function(HANDLE const process, ULONG64 const module_base, ULONG64 const index) {
        DWORD type_index = 0;
        if(!SymGetTypeInfo(process, module_base, index, TI_GET_TYPEID, &type_index)) {
            return u8"unknown_function"_s;
        }

        if(WCHAR* fn_name_wchar = nullptr; SymGetTypeInfo(process, module_base, index, TI_GET_SYMNAME, &fn_name_wchar)) {
            String function_name = String::from_utf16(reinterpret_cast<char16_t*>(fn_name_wchar));
            LocalFree(fn_name_wchar);
            return process_function_type(process, module_base, type_index, function_name);
        } else {
            return process_function_type(process, module_base, type_index, u8"unknown_name"_sv);
        }
    }

    [[nodiscard]] static String process_function_argument(HANDLE const process, ULONG64 const module_base, ULONG64 const index) {
        if(DWORD type_index = 0; SymGetTypeInfo(process, module_base, index, TI_GET_TYPEID, &type_index)) {
            return get_type_as_string(process, module_base, type_index);
        } else {
            return u8"unknown_type"_s;
        }
    }

    [[nodiscard]] static String process_class(HANDLE const process, ULONG64 const module_base, ULONG64 const index) {
        if(WCHAR* name = nullptr; SymGetTypeInfo(process, module_base, index, TI_GET_SYMNAME, &name)) {
            String class_name = String::from_utf16(reinterpret_cast<char16_t*>(name));
            LocalFree(name);
            return class_name;
        } else {
            return u8"unknown_name"_s;
        }
    }

    [[nodiscard]] static String process_udt(HANDLE const process, ULONG64 const module_base, ULONG64 const index) {
        // Describes the variety of User-Defined Types (UDT)
        // Copied from cvconst.h
        //
        // https://docs.microsoft.com/en-us/visualstudio/debugger/debug-interface-access/udtkind?view=vs-2019
        //
        enum UdtKind {
            UdtStruct,
            UdtClass,
            UdtUnion,
            UdtInterface // unused
        };

        if(DWORD udt_kind = ~0; SymGetTypeInfo(process, module_base, index, TI_GET_UDTKIND, &udt_kind)) {
            switch(udt_kind) {
                case UdtStruct:
                case UdtClass:
                case UdtUnion:
                    // Since we are only getting the name of the UDT, all 3 cases are the same.
                    return process_class(process, module_base, index);
                default:
                    return u8"unknown_type"_s;
            }
        } else {
            return u8"unknown_type"_s;
        }
    }

    static String get_type_as_string(HANDLE const process, ULONG64 const module_base, ULONG64 const index) {
        DWORD type_tag = 0;
        if(!SymGetTypeInfo(process, module_base, index, TI_GET_SYMTAG, &type_tag)) {
            return u8"unknown_type"_s;
        }

        switch(type_tag) {
            case SymTagBaseType:
                return process_base_type(process, module_base, index);

            case SymTagTypedef:
                return process_typedef(process, module_base, index);

            case SymTagPointerType:
                return process_pointer_type(process, module_base, index);

            case SymTagFunctionType:
                return process_function_type(process, module_base, index, u8""_sv);

            case SymTagFunction:
                return process_function(process, module_base, index);

            case SymTagFunctionArgType:
                return process_function_argument(process, module_base, index);

            case SymTagUDT:
                return process_udt(process, module_base, index);

            default:
                return u8"unknown_type"_s;
        }
    }

    static String get_record_function_name(Frame_Record* record) {
        constexpr int max_symbol_length = 512;
        char symbol_info_buffer[sizeof(SYMBOL_INFO) + sizeof(CHAR) * max_symbol_length] = {};
        SYMBOL_INFO& symbol_info = *reinterpret_cast<SYMBOL_INFO*>(symbol_info_buffer);
        symbol_info.SizeOfStruct = sizeof(SYMBOL_INFO);
        symbol_info.MaxNameLen = max_symbol_length + 1;

        if(!SymFromAddr(process_handle, DWORD64(record), nullptr, &symbol_info)) {
            return to_string(record);
        }

        DWORD type_tag = 0;
        if(!SymGetTypeInfo(process_handle, symbol_info.ModBase, symbol_info.Index, TI_GET_SYMTAG, &type_tag)) {
            return to_string(record);
        }

        if(type_tag != SymTagFunction) {
            return to_string(record);
        }

        return process_function(process_handle, symbol_info.ModBase, symbol_info.Index);
    }

    static i64 get_record_line(Frame_Record* record) {
        IMAGEHLP_LINE64 image_line;
        image_line.SizeOfStruct = sizeof(IMAGEHLP_LINE64);
        if(DWORD displacement = 0; SymGetLineFromAddr64(process_handle, DWORD64(record), &displacement, &image_line)) {
            return (i64)image_line.LineNumber;
        } else {
            return -1;
        }
    }

    Stack_Frame resolve_frame_record(Frame_Record* record) {
        Stack_Frame stack_frame;

        Owning_Ptr<Frame_Record> record_ptr;
        String function = get_record_function_name(record);
        i64 line = get_record_line(record);
        return Stack_Frame{ANTON_MOV(function), line};
    }
} // namespace anton::stacktrace
