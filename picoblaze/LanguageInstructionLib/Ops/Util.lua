Util = {}
-- Util functions for PicoBlaze assembly language

function Util.WriteSimpleImmediateOrRegister(compiler)
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

    local regSecond = Lib.ParseRegister(compiler, thisToken)

    if regSecond == nil then
        -- an immediate value
        local immValue = Lib.ParseUnsigned(compiler, thisToken)
        if type(immValue) ~= "number" then
            return immValue
        end
        compiler:WriteUnsignedNumber(immValue, 8)
        compiler:WriteUnsignedNumber(regFirst, 4)
        compiler:WriteUnsignedNumber(0, 1)
        return nil
    end

    if type(regSecond) ~= "number" then
        compiler:WriteUnsignedNumber(0, 4)
        compiler:WriteUnsignedNumber(regSecond, 4)
        compiler:WriteUnsignedNumber(regFirst, 4)
        compiler:WriteUnsignedNumber(1, 1)
        return nil
    end

--     print("Util: Error writing immediate or register for s" .. regFirst .. " with second operand '" .. thisToken .. "'")

    return Exception.MakeCompileErrorWithLocation(
        tokenStream,
        "Invalid second operand: '" .. thisToken .. "'. Expected a register or an immediate value."
    )
end

function Util.WriteDummyAddress(compiler, label)
    local currentStart = compiler:GetBitBufferSize()
    compiler:WriteUnsignedNumber(1023, 10)
    local linkerContext = compiler:GetLinkerContext()
    linkerContext.LinkAddressRequestArray[#linkerContext.LinkAddressRequestArray + 1] = {
        Label = label,
        Start = currentStart,
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