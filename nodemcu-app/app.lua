print("in app.lua")

if not tmr.create():alarm(5000, tmr.ALARM_SINGLE, function()
  print("hey there")
end)
then
  print("whoopsie")
end
--[[]
buffer = ""

uart.on("data", 1,
  function(data)
    buffer = buffer..data
    if(data == "\n") then
      print(buffer)
      buffer = ""
    end
end, 0)
]]
