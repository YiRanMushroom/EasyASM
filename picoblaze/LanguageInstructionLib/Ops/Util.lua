Util = {}
-- Util functions for PicoBlaze assembly language

function Util.WriteSimpleImmediateOrRegister(compiler)
    local tokenStream = compiler:GetTokenStream()

    local thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    local regFirst = Lib.ParseRegister(tokenStream, thisToken)
    if type(regFirst) ~= "number" then
        return regNum
    end

    thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    if thisToken ~= "," then
        return Exception.MakeCompileErrorWithLocation(
            tokenStream,
            "Expected ',' after first register, but found '" .. thisToken .. "' instead."
        )
    end

    thisToken = tokenStream:ParseCurrent()
    if thisToken == nil then
        return Exception.MakeCompileErrorWithLocation(tokenStream, "No token found in the token stream.")
    end
    if thisToken:sub(1, 1) == 's' then
        -- also a register
        local regSecond = Lib.ParseRegister(tokenStream, thisToken)
        if type(regSecond) ~= "number" then
            return regSecond
        end
        compiler:WriteUnsignedNumber(0, 4)
        compiler:WriteUnsignedNumber(regSecond, 4)
        compiler:WriteUnsignedNumber(regFirst, 4)
        compiler:WriteUnsignedNumber(1, 1)
    else
        -- an immediate value
        local immValue = Lib.ParseUnsigned(tokenStream, thisToken)
        if type(immValue) ~= "number" then
            return immValue
        end
        compiler:WriteUnsignedNumber(immValue, 8)
        compiler:WriteUnsignedNumber(regFirst, 4)
        compiler:WriteUnsignedNumber(0, 1)
    end
end

function Util.WriteDummyAddress(compiler, label)
    local currentStart = compiler:GetBitBufferSize()
    compiler:WriteUnsignedNumber(1023, 10) -- 1023 is the maximum address in PicoBlaze
    local linkerContext = compiler:GetLinkerContext()
--     print("Util: Writing dummy address for label '" .. label .. "' at position " .. currentStart)
    linkerContext.LinkRequestArray[#linkerContext.LinkRequestArray + 1] = {
        Label = label,
        Start = currentStart
    }
end

function Util.GetPossibleCondition(tokenStream, thisToken)
    local returnCondition = 0

    if Lib.ToLowerCase(thisToken) == "c" then
        returnCondition = 6;
        thisToken = tokenStream:ParseCurrent()
        if thisToken == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Expected condition after 'C', but found nothing."
            )
        elseif thisToken ~= "," then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Expected ',' after 'C', but found '" .. thisToken .. "' instead."
            )
        end
        thisToken = tokenStream:ParseCurrent()

    elseif Lib.ToLowerCase(thisToken) == "nc" then
        returnCondition = 7;
        thisToken = tokenStream:ParseCurrent()
        if thisToken == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Expected condition after 'NC', but found nothing."
            )
        elseif thisToken ~= "," then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Expected ',' after 'NC', but found '" .. thisToken .. "' instead."
            )
        end
        thisToken = tokenStream:ParseCurrent()

    elseif Lib.ToLowerCase(thisToken) == "z" then
        returnCondition = 4;
        thisToken = tokenStream:ParseCurrent()
        if thisToken == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Expected condition after 'Z', but found nothing."
            )
        elseif thisToken ~= "," then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Expected ',' after 'Z', but found '" .. thisToken .. "' instead."
            )
        end
        thisToken = tokenStream:ParseCurrent()

    elseif Lib.ToLowerCase(thisToken) == "nz" then
        returnCondition = 5;
        thisToken = tokenStream:ParseCurrent()

        if thisToken == nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Expected condition after 'NZ', but found nothing."
            )
        elseif thisToken ~= "," then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Expected ',' after 'NZ', but found '" .. thisToken .. "' instead."
            )
        end

        thisToken = tokenStream:ParseCurrent()
    else
        returnCondition = 0; -- no condition
    end

    return thisToken, returnCondition
end