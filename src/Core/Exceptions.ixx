export module Core.Exceptions;

import std;
import Vendor.sol;

namespace Core::Exceptions {
    export class EasyASMException : public std::exception {
        std::string message;
    public:
        EasyASMException(const std::string& msg) : message(msg) {}
        const char* what() const noexcept override {
            return message.c_str();
        }
    };

    export class CompilerImplementationError : public EasyASMException {
    public:
        CompilerImplementationError(const std::string& msg)
            : EasyASMException(msg) {}
    };

    export class CompileError : public EasyASMException {
    public:
        CompileError(const std::string& msg)
            : EasyASMException(msg) {}
    };

    export class LinkError : public EasyASMException {
    public:
        LinkError(const std::string& msg)
            : EasyASMException(msg) {}
    };

    export void ThrowCompilerImplementationError(const std::string& msg) {
        throw CompilerImplementationError(msg);
    }

    export void ThrowCompileError(const std::string& msg) {
        throw CompileError(msg);
    }

    export void ThrowLinkError(const std::string& msg) {
        throw LinkError(msg);
    }

    export void AddLibToState(sol::state& state) {
        auto Exception = state.create_named_table("Exception");
        Exception.set_function("ThrowCompilerImplementationError",
            &ThrowCompilerImplementationError);
        Exception.set_function("ThrowCompileError",
            &ThrowCompileError);
        Exception.set_function("ThrowLinkError",
            &ThrowLinkError);
    }
}