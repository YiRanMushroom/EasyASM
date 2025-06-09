function ProcessXor(compiler)
   local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
   if callResult ~= nil then
       return callResult
   end
   -- Write the opcode for the AND operation
   compiler:WriteUnsignedNumber(7, 5)
end