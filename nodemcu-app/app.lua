print("in app.lua")

uart.on("data", 1,
  function(data)
    print("receive from uart:", data)
    if data=="quit" then
      uart.on("data") -- unregister callback function
    end
end, 0)
