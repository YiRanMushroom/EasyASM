function ProcessSL0(compiler)
    compiler:WriteUnsignedNumber(6, 8)
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local regFirst = Lib.ParseRegister(compiler, thisToken)
    if type(regFirst) ~= "number" then
        if regFirst == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Invalid register name: '" .. thisToken .. "'. Expected 's0' to 's15' or a valid register name."
            )
        else
            return regFirst
        end
    end
    compiler:WriteUnsignedNumber(regFirst, 4)
    compiler:WriteUnsignedNumber(32, 6)
end

function ProcessSL1(compiler)
    compiler:WriteUnsignedNumber(7, 8)
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local regFirst = Lib.ParseRegister(compiler, thisToken)
    if type(regFirst) ~= "number" then
        if regFirst == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Invalid register name: '" .. thisToken .. "'. Expected 's0' to 's15' or a valid register name."
            )
        else
            return regFirst
        end
    end
    compiler:WriteUnsignedNumber(regFirst, 4)
    compiler:WriteUnsignedNumber(32, 6)
end

function ProcessSLA(compiler)
    compiler:WriteUnsignedNumber(0, 8)
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local regFirst = Lib.ParseRegister(compiler, thisToken)
    if type(regFirst) ~= "number" then
        if regFirst == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Invalid register name: '" .. thisToken .. "'. Expected 's0' to 's15' or a valid register name."
            )
        else
            return regFirst
        end
    end
    compiler:WriteUnsignedNumber(regFirst, 4)
    compiler:WriteUnsignedNumber(32, 6)
end

function ProcessSLX(compiler)
    compiler:WriteUnsignedNumber(4, 8)
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local regFirst = Lib.ParseRegister(compiler, thisToken)
    if type(regFirst) ~= "number" then
        if regFirst == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Invalid register name: '" .. thisToken .. "'. Expected 's0' to 's15' or a valid register name."
            )
        else
            return regFirst
        end
    end
    compiler:WriteUnsignedNumber(regFirst, 4)
    compiler:WriteUnsignedNumber(32, 6)
end

function ProcessSR0(compiler)
    compiler:WriteUnsignedNumber(14, 8)
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local regFirst = Lib.ParseRegister(compiler, thisToken)
    if type(regFirst) ~= "number" then
        if regFirst == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Invalid register name: '" .. thisToken .. "'. Expected 's0' to 's15' or a valid register name."
            )
        else
            return regFirst
        end
    end
    compiler:WriteUnsignedNumber(regFirst, 4)
    compiler:WriteUnsignedNumber(32, 6)
end

function ProcessSR1(compiler)
    compiler:WriteUnsignedNumber(15, 8)
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local regFirst = Lib.ParseRegister(compiler, thisToken)
    if type(regFirst) ~= "number" then
        if regFirst == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Invalid register name: '" .. thisToken .. "'. Expected 's0' to 's15' or a valid register name."
            )
        else
            return regFirst
        end
    end
    compiler:WriteUnsignedNumber(regFirst, 4)
    compiler:WriteUnsignedNumber(32, 6)
end

function ProcessSRA(compiler)
    compiler:WriteUnsignedNumber(8, 8)
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local regFirst = Lib.ParseRegister(compiler, thisToken)
    if type(regFirst) ~= "number" then
        if regFirst == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Invalid register name: '" .. thisToken .. "'. Expected 's0' to 's15' or a valid register name."
            )
        else
            return regFirst
        end
    end
    compiler:WriteUnsignedNumber(regFirst, 4)
    compiler:WriteUnsignedNumber(32, 6)
end

function ProcessSRX(compiler)
    compiler:WriteUnsignedNumber(10, 8)
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local regFirst = Lib.ParseRegister(compiler, thisToken)
    if type(regFirst) ~= "number" then
        if regFirst == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Invalid register name: '" .. thisToken .. "'. Expected 's0' to 's15' or a valid register name."
            )
        else
            return regFirst
        end
    end
    compiler:WriteUnsignedNumber(regFirst, 4)
    compiler:WriteUnsignedNumber(32, 6)
end

function ProcessStore(compiler)
    local callResult = Util.WriteAddressImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the AND operation
    compiler:WriteUnsignedNumber(23, 5)
end

function ProcessSub(compiler)
    local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the AND operation
    compiler:WriteUnsignedNumber(14, 5)
end

function ProcessSubCarry(compiler)
    local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
    if callResult ~= nil then
        return callResult
    end
    -- Write the opcode for the AND operation
    compiler:WriteUnsignedNumber(15, 5)
end