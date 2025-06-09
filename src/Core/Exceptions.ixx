export module Core.Exceptions;

import std;
import Vendor.sol;

namespace Core::Exceptions {
    export class EasyASMException : public std::exception {
        std::string message;

    public:
        EasyASMException(const std::string &msg) : message(msg) {}

        const char *what() const noexcept override {
            return message.c_str();
        }
    };

    export class CompilerImplementationError : public EasyASMException {
    public:
        CompilerImplementationError(const std::string &msg)
            : EasyASMException(msg) {}
    };

    export class CompileError : public EasyASMException {
    public:
        CompileError(const std::string &msg)
            : EasyASMException(msg) {}
    };

    export class LinkError : public EasyASMException {
    public:
        LinkError(const std::string &msg)
            : EasyASMException(msg) {}
    };

    export class WrappedGenericException : public std::exception {
    private:
        struct WrappedGenericExceptionImpl {
            WrappedGenericExceptionImpl(std::unique_ptr<std::exception> e,
                                        std::function<void(std::unique_ptr<std::exception>)> rethrowFunction)
                : WrappedException(std::move(e)),
                  RethrowFunction(std::move(rethrowFunction)) {}

            std::unique_ptr<std::exception> WrappedException;
            std::function<void(std::unique_ptr<std::exception>)> RethrowFunction;

            ~WrappedGenericExceptionImpl() noexcept {
                if (WrappedException) {
                    std::cerr << std::format(
                        "Unhandled exception detected, this is generally a bug in the compiler. Please report this issue to your compiler vendor.\n"
                        "The Exception is: {}\n",
                        WrappedException->what());
                    std::exit(-1);
                }
            }
        };

    public:
        template<typename T, typename... Args>
        static WrappedGenericException MakeWrappedGenericException(
            Args &&... args) {
            return WrappedGenericException{
                 std::make_shared<WrappedGenericExceptionImpl>(
                    std::make_unique<T>(std::forward<Args>(args)...),
                    [](std::unique_ptr<std::exception> e) {
                        std::exception *ptr = e.release();
                        T *promoted = static_cast<T *>(ptr);
                        throw std::move(*promoted);
                    })
            };
        };

        WrappedGenericException(std::shared_ptr<WrappedGenericExceptionImpl> wrappedException)
            : m_WrappedException(std::move(wrappedException)) {
        }

        std::unique_ptr<std::exception> GetWrappedException() {
            return std::move(m_WrappedException->WrappedException);
        }

        void ThrowIfNotNull() {
            if (IsNull()) {
                return;
            }
            m_WrappedException->RethrowFunction(std::move(m_WrappedException->WrappedException));
        }

        [[nodiscard]] std::exception *GetPointer() const {
            if (m_WrappedException && m_WrappedException->WrappedException) {
                return m_WrappedException->WrappedException.get();
            }
            return nullptr;
        }

        [[nodiscard]] bool IsNull() const {
            return !m_WrappedException || !m_WrappedException->WrappedException;
        }

        [[nodiscard]] const char *what() const override {
            if (m_WrappedException && m_WrappedException->WrappedException) {
                return m_WrappedException->WrappedException->what();
            }
            throw std::runtime_error("Wrapped exception is null");
        }

        std::exception &operator*() const {
            if (m_WrappedException && m_WrappedException->WrappedException) {
                return *m_WrappedException->WrappedException;
            }
            throw std::runtime_error("Wrapped exception is null");
        }

        std::exception *operator->() const {
            if (m_WrappedException && m_WrappedException->WrappedException) {
                return m_WrappedException->WrappedException.get();
            }
            throw std::runtime_error("Wrapped exception is null");
        }

    private:
        std::shared_ptr<WrappedGenericExceptionImpl> m_WrappedException;
    };

    export WrappedGenericException MakeCompilerImplementationError(
        const std::string &msg) {
        return WrappedGenericException::MakeWrappedGenericException<
            CompilerImplementationError>(msg);
    }

    export WrappedGenericException MakeCompileError(
        const std::string &msg) {
        return WrappedGenericException::MakeWrappedGenericException<
            CompileError>(msg);
    }

    export WrappedGenericException MakeLinkError(
        const std::string &msg) {
        return WrappedGenericException::MakeWrappedGenericException<
            LinkError>(msg);
    }

    export void AddLibToState(sol::state &state) {
        auto Exception = state.create_named_table("Exception");
        Exception.set_function("MakeCompilerImplementationError",
                               &MakeCompilerImplementationError);
        Exception.set_function("MakeCompileError",
                               &MakeCompileError);
        Exception.set_function("MakeLinkError",
                               &MakeLinkError);
    }
}
