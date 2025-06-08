module Core.Parser;

import <cassert>;

namespace Core {
    const static std::unordered_set<char> whitespace = {' ', '\t', '\n', '\r'};
    const static std::unordered_set<char> delimiters = {'(', ')', '{', '}', '[', ']', ',', ';', ':', '\"', '\''};

    std::optional<std::string> TokenStream::ParseCurrent() {
        assert(current != source.end() && !whitespace.contains(*current) && "Current character should not be whitespace");

        if (current == source.end()) {
            return std::nullopt; // no more tokens
        }
        std::string buffer; // small string optimization should be enough for most tokens

        if (*current == '\"') {
            return ParseString();
        }

        while (current != source.end() && !whitespace.contains(*current) && !delimiters.contains(*current)) {
            buffer += *current;
            ++current;
        }

        if (buffer.empty()) {
            return std::nullopt; // no token found
        }

        SkipToNextToken();

        return buffer; // return the parsed token
    }

    std::optional<std::string> TokenStream::PeekCurrent() {
        auto savedCurrent = current; // save the current position
        auto savedNumberOfLines = numberOfLines; // save the current line count
        auto token = ParseCurrent(); // parse the current token
        numberOfLines = savedNumberOfLines; // restore the line count
        current = savedCurrent; // restore the current position
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
            return std::format("At line {}, near token '{}'", numberOfLines, *next);
        } else {
            return std::format("At line {}, end of source", numberOfLines);
        }
    }

    void TokenStream::SkipWhitespace() {
        while (current != source.end() && whitespace.contains(*current)) {
            if (*current == '\n') {
                ++numberOfLines; // increment line count on newline
            }
            ++current;
        }
    }

    void TokenStream::SkipToNextToken() {
        SkipWhitespace();
        // detect if a ; is presented, if so, skip to the next line
        while (current != source.end()) {
            assert(!whitespace.contains(*current) && "Whitespace should not be present here");

            if (current == source.end() || *current != ';') {
                return;
            }

            while (current != source.end() && *current != '\n') {
                ++current;
            }

            // this should now be at the end of a line
            SkipWhitespace();
        }
    }

    std::optional<std::string> TokenStream::ParseString() {
        assert(*current == '\"' && "Current character should be a quote for string parsing");
        std::string buffer = "\""; // start with the opening quote
        ++current; // move past the opening quote
        // parsed string should contain the qoutes
        while (current != source.end() && *current != '\"') {
            if (*current == '\\') {
                // handle escape sequences
                ++current; // move past the backslash
                if (current == source.end()) {
                    return std::nullopt; // unterminated string
                }
                switch (*current) {
                    case 'n': buffer += '\n'; break;
                    case 't': buffer += '\t'; break;
                    case '\"': buffer += '\"'; break;
                    case '\\': buffer += '\\'; break;
                    default: buffer += *current; break; // just add the character
                }
            } else {
                buffer += *current; // add the character to the buffer
            }

            ++current; // move to the next character
        }

        assert(current != source.end() && *current == '\"' && "Current character should be a closing quote for string parsing");
        ++current; // move past the closing quote

        SkipToNextToken(); // skip to the next token after the string

        return buffer; // return the parsed string
    }
}
