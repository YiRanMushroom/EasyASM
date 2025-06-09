function Lib.ParseSimpleRegister(tokenStream, str)
    local lower = Lib.ToLowerCase(str)
    if (#lower ~= 2 or lower:sub(1,1) ~= 's') then
        return nil
    end

    local second = lower:sub(2,2)
    if not((second >= '0' and second <='9') or(second >= 'a' and second <= 'f')) then
        return nil
    end

    return tonumber(second, 16)
end

function Lib.ParseRegister(compiler, str)
    local tokenStream = compiler:GetTokenStream()
    local RegNameArray = compiler:GetCompilerContext().RegNameArray
    result = Lib.ParseSimpleRegister(tokenStream, str)
    if result ~= nil then
        if RegNameArray[result] ~= nil then
            return Exception.MakeCompileErrorWithLocation(
                tokenStream,
                "Register '" .. str .. "' is already defined as '" .. RegNameArray[result] .. "'."
            )
        end

        return result
    else
        for i = 0,  15 do
            if RegNameArray[i] == str then
                return i
            end
        end

        return nil
    end
end

function Lib.ParseSimpleUnsigned(tokenStream, str) -- 8 bit unsigned immediate value
    if str:sub(1,2) == "0x" or str:sub(1,2) == "0X" then
        print("Warning: Immediate value should not include '0x' prefix: " .. str)
        str = str:sub(3)
    end

    return tonumber(str, 16)
end

function Lib.ParseUnsigned(compiler, str)
    local result = Lib.ParseSimpleUnsigned(compiler:GetTokenStream(),  str)
    if result == nil or result < 0 or result > 255 then
        return Exception.MakeCompileErrorWithLocation(
            compiler:GetTokenStream(),
            "Invalid unsigned immediate value: '" .. str .. "'. Expected a value between 0 and 255."
        )
    end
    return result
end

function Exception.MakeCompileErrorWithLocation(tokenStream, message)
    local position = tokenStream:GetApproxCurrentLocation()
    return Exception.MakeCompileError(
        "Compile Error " .. position .. ": " .. message
    )
end

function Exception.MakeLinkErrorWithLocation(tokenStream, message)
    local position = tokenStream:GetApproxCurrentLocation()
    return Exception.MakeLinkError(
        "Link Error " .. position .. ": " .. message
    )
end

function Exception.MakeCompilerImplementationErrorWithLocation(tokenStream, message)
    local position = tokenStream:GetApproxCurrentLocation()
    return Exception.MakeCompilerImplementationError(
        "Compiler Implementation Error " .. position .. ": " .. message
    )
end