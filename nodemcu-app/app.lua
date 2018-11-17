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

--[[if file.open("test.txt") then
  print(file.read())
  file.close()
end

file.open("test.txt","w")
file.writeline("hello")
file.close()
]]

local homepage = [[
  <!DOCTYPE HTML>
  <html>
    <head>
      <meta content="text/html; charset=utf-8">
      <title>ESP8266</title>
      <style type="text/css">
        html, body {
          min-height: 100%;
        }
        body {
          font-family: monospace;
          background: url(http://i.imgur.com/rqJrop4.gif) no-repeat 0 0 #5656fa;
          background-size: cover;
          margin: 0;
          padding: 10px;
          text-align: center;
          color: #56f2ff;
        }
      </style>
    </head>
    <body>
      NodeMCU Server
    </body>
  </html>
]]

local function server()
  local s = net.createServer(net.TCP)

  s:listen(80, function(connection)
    connection:on("receive", function(c, request)
      print(request)
      file.open("index.html")
      c:send(file.read())
      file.close()
    end)

    connection:on("sent", function(c) c:close() end)
  end)
end

server()
