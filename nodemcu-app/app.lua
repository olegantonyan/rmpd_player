print("in app.lua")

buffer = ""

uart.on("data", 1,
  function(data)
    buffer = buffer..data
    if(data == "\n") then
      print(buffer)
      buffer = ""
    end
end, 0)
