module Core.Parser;

import <cassert>;

namespace Core {
    const static std::unordered_set<char> whitespace = {' ', '\t', '\n', '\r'};
    const static std::unordered_set<char> delimiters = {'(', ')', '{', '}', '[', ']', ',', ';', ':', '\"', '\''};

    std::optional<std::string> TokenStream::ParseCurrent() {
        if (m_Current == m_Source.end()) {
            return std::nullopt; // no more tokens
        }

        assert(!whitespace.contains(*m_Current) && "Current character should not be whitespace");

        m_IsNewLine = false; // reset newline state

        if (*m_Current == '\"') {
            return ParseString();
        }

        if (delimiters.contains(*m_Current)) {
            std::string delimiter(1, *m_Current); // create a string from the single character
            ++m_Current; // move past the delimiter
            SkipToNextToken(); // skip to the next token
            return delimiter; // return the parsed delimiter
        }

        std::string buffer; // small string optimization should be enough for most tokens

        while (m_Current != m_Source.end() && !whitespace.contains(*m_Current) && !delimiters.contains(*m_Current)) {
            buffer += *m_Current;
            ++m_Current;
        }

        assert(!buffer.empty() && "Buffer should not be empty after parsing a token");

        SkipToNextToken();

        return buffer; // return the parsed token
    }

    std::optional<std::string> TokenStream::PeekCurrent() {
        auto savedCurrent = m_Current; // save the current position
        auto savedNumberOfLines = m_NumberOfLines; // save the current line count
        auto savedIsNewLine = m_IsNewLine; // save the current newline state
        auto token = ParseCurrent(); // parse the current token
        m_IsNewLine = savedIsNewLine; // restore the newline state
        m_NumberOfLines = savedNumberOfLines; // restore the line count
        m_Current = savedCurrent; // restore the current position
        return token; // return the parsed token
    }

    void TokenStream::SkipCurrent() {
        // Could find a better way.
        (void) ParseCurrent(); // simply call ParseCurrent to skip the current token
    }

    std::string TokenStream::GetApproxCurrentLocation() {
        // Approximate location is the number of lines and the current position in the source
        auto next = PeekCurrent();
        if (next) {
            return std::format("At line {}, near token '{}'", m_NumberOfLines, *next);
        } else {
            return std::format("At line {}, end of source", m_NumberOfLines);
        }
    }

    std::optional<Exceptions::WrappedGenericException> TokenStream::AssertIsNewLine() {
        if (!m_IsNewLine) {
            return Exceptions::MakeCompilerImplementationError(
                std::format("Expected a newline at line {}, but found '{}'",
                            m_NumberOfLines, *m_Current));
        }

        return std::nullopt; // no exception, we are at a newline
    }

    void TokenStream::SetNewLine(bool isNewLine) {
        m_IsNewLine = isNewLine;
    }

    void TokenStream::SkipWhitespace() {
        while (m_Current != m_Source.end() && whitespace.contains(*m_Current)) {
            if (*m_Current == '\n') {
                ++m_NumberOfLines; // increment line count on newline
                m_IsNewLine = true; // set newline state
            }
            ++m_Current;
        }
    }

    void TokenStream::SkipToNextToken() {
        SkipWhitespace();
        // detect if a ; is presented, if so, skip to the next line
        while (m_Current != m_Source.end()) {
            assert(!whitespace.contains(*m_Current) && "Whitespace should not be present here");

            if (m_Current == m_Source.end() || *m_Current != ';') {
                return;
            }

            while (m_Current != m_Source.end() && *m_Current != '\n') {
                ++m_Current;
            }

            // this should now be at the end of a line
            SkipWhitespace();
        }
    }

    std::optional<std::string> TokenStream::ParseString() {
        assert(*m_Current == '\"' && "Current character should be a quote for string parsing");
        std::string buffer = "\""; // start with the opening quote
        ++m_Current; // move past the opening quote
        // parsed string should contain the qoutes
        while (m_Current != m_Source.end() && *m_Current != '\"') {
            if (*m_Current == '\\') {
                // handle escape sequences
                ++m_Current; // move past the backslash
                if (m_Current == m_Source.end()) {
                    return std::nullopt; // unterminated string
                }
                switch (*m_Current) {
                    case 'n': buffer += '\n'; break;
                    case 't': buffer += '\t'; break;
                    case '\"': buffer += '\"'; break;
                    case '\\': buffer += '\\'; break;
                    default: buffer += *m_Current; break; // just add the character
                }
            } else {
                buffer += *m_Current; // add the character to the buffer
            }

            ++m_Current; // move to the next character
        }

        assert(m_Current != m_Source.end() && *m_Current == '\"' && "Current character should be a closing quote for string parsing");
        ++m_Current; // move past the closing quote

        SkipToNextToken(); // skip to the next token after the string

        return buffer; // return the parsed string
    }
}
