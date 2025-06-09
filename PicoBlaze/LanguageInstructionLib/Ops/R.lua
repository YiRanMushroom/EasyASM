function ProcessReturn(compiler)
    local tokenStream = compiler:GetTokenStream()
    if tokenStream:IsNewLine() then
        compiler:WriteUnsignedNumber(172032, 18)
        return
    end

    local thisToken = Lib.ToLowerCase(tokenStream:ParseCurrent())
    if thisToken == "c" then
        -- Write the opcode for the return with carry
        compiler:WriteUnsignedNumber(178176, 18)
   elseif thisToken == "nc" then
        -- Write the opcode for the return without carry
        compiler:WriteUnsignedNumber(179200, 18)
    elseif thisToken == "z" then
        -- Write the opcode for the return with zero flag
        compiler:WriteUnsignedNumber(176128, 18)
    elseif thisToken == "nz" then
        -- Write the opcode for the return without zero flag
        compiler:WriteUnsignedNumber(177152, 18)
    else
        error("Invalid token for return operation: " .. thisToken)
    end
end

function ProcessReturnI(compiler)
    local tokenStream =  compiler:GetTokenStream()
    local thisToken = Lib.ToLowerCase(tokenStream:ParseCurrent())
    if thisToken == "disable" then
        -- Write the opcode for the disable return
        compiler:WriteUnsignedNumber(229376, 18)
    elseif thisToken == "enable" then
        -- Write the opcode for the enable return
        compiler:WriteUnsignedNumber(229377, 18)
    else
        error("Invalid token for returni operation, expected 'disable' or 'enable', got: " .. thisToken)
    end
end

function ProcessRL(compiler)
    compiler:WriteUnsignedNumber(2, 8)
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

function ProcessRR(compiler)
    compiler:WriteUnsignedNumber(12, 8)
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