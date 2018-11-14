
function write(text)
  uart.write(0, text)
end

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
