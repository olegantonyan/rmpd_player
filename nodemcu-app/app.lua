function write(text)
  uart.write(0, text)
end
--[[
function on_recevie(text)
  write("gotcha! "..text)
end

buffer = ""
uart.on("data", 1,
  function(data)
    buffer = buffer..data
    if(data == "\n") then
      on_recevie(buffer)
      buffer = ""
    end
end, 0)
]]
--[[if file.open("test.txt") then
  print(file.read())
  file.close()
end

file.open("test.txt","w")
file.writeline("hello")
file.close()
]]
