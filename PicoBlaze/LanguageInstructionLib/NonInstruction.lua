function AddLabel(compiler, label)
    local linkerContext = compiler:GetLinkerContext()
    if linkerContext.LabelToAddressMap[label] ~= nil then
        return Exception.MakeCompileErrorWithLocation(
            compiler:GetTokenStream(),
            "Label '" .. label .. "' is already defined."
        )
    end

    linkerContext.LabelToAddressMap[label] = compiler:GetBitBufferSize() / 18
end

function CheckLabelValid(compiler, token)
    if token == nil then
        return Exception.MakeCompilerImplementationErrorWithLocation(
            compiler:GetTokenStream(),
            "Token is nil when checking label validity.")
    end

    if token == '/' then
        local tokenStream = compiler:GetTokenStream()
        local nextToken = tokenStream:PeekCurrent()
        if nextToken == nil then
            return Exception.MakeCompilerImplementationErrorWithLocation(
                tokenStream,
                "Unexpected Token '/'. No next token found in the token stream."
            )
        end
        if nextToken == '/' then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Unexpected Token '//'. Notice that you cannot use '//' for comments in PicoBlaze assembly. Use ';' instead."
            )
        else
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Unexpected Token '/'."
            )
        end
    end

    -- if it does not start with a letter, _ or a number, it is not a valid label
    if not (token:match("^[_%a]") or token:match("^[%d]")) then
        return Exception.MakeCompileErrorWithLocation(
            compiler:GetTokenStream(),
            "Label '" .. token .. "' must start with a letter, a number, or an underscore (_)."
        )
    end
end

function ProcessNonInstruction(compiler)
    local tokenStream = compiler:GetTokenStream()
    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompilerImplementationErrorWithLocation(
            tokenStream,
            "No token found in the token stream."
        )
    end

    local possibleError = CheckLabelValid(compiler, thisToken)
    if possibleError ~= nil then
        return possibleError
    end

    local nextToken = tokenStream:PeekCurrent()
    if nextToken == nil then
        return Exception.MakeCompilerImplementationErrorWithLocation(
            tokenStream,
            "No next token found in the token stream."
        )
    end

    if nextToken ~= ":" then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Expected ':' after '" .. thisToken .. "'. This may be a label missing a colon, or the token was mistakenly interpreted as an instruction or directive-possibly due to a spelling error or because the user assumed it exists in the instruction/directive set-but it is not recognized."
        )
    end

    tokenStream:SkipCurrent() -- ":"

    tokenStream:SetNewLine(true)

    return AddLabel(compiler, thisToken)
end