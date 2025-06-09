function ProcessTest(compiler)
   local callResult = Util.WriteSimpleImmediateOrRegister(compiler)
   if callResult ~= nil then
       return callResult
   end
   -- Write the opcode for the AND operation
   compiler:WriteUnsignedNumber(9, 5)
end