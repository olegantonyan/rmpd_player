print("in app.lua")

if not tmr.create():alarm(5000, tmr.ALARM_SINGLE, function()
  print("hey there")
  sk = net.createConnection(net.TCP, 0)
  sk:dns("www.nodemcu.com", function(conn, ip) print(ip) end)
  sk = nil

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
