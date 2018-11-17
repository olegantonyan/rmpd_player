local function response(status, data)
  local resp = "HTTP/1.1 "..status.."\r\n"
  resp = resp.."Content-Type: text/html; charset=UTF-8\r\n"
  resp = resp.."Content-Length: "..string.len(data).."\r\n"
  resp = resp.."Server: rmpd_player\r\n"
  resp = resp.."Connection: close\r\n"
  resp = resp.."\r\n"..data
  return resp
end

local function route(method, path)
end

local function server()
  local s = net.createServer(net.TCP)

  s:listen(80, function(connection)
    connection:on("receive", function(c, request)
      local _, _, method, path, vars = string.find(request, "([A-Z]+) (.+)?(.+) HTTP")
      if (method == nil) then
        _, _, method, path = string.find(request, "([A-Z]+) (.+) HTTP")
      end

      print(request)
      file.open("index.html")
      c:send(response("200 OK", file.read()))
      file.close()
    end)

    connection:on("sent", function(c) c:close() end)
  end)
end

server()
