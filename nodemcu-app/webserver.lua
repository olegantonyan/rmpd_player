local function response(status, data, content_type)
  local resp = "HTTP/1.1 "..status.."\r\n"
  resp = resp.."Content-Type: "..content_type.."\r\n"
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

      print("method", method)
      print("path", path)

      filename = string.sub(path, 2)
      f = file.open(filename)
      if f then
        local type = ""
        if string.find(filename, "\.html$") then
          type = "text/html"
        elseif string.find(filename, "\.css$") then
          type = "text/css"
        elseif string.find(filename, "\.ico$") then
          type = "image/x-icon"
        elseif string.find(filename, "\.js$") then
          type = "text/javascript"
        end
        c:send(response("200 OK", f:read(), type))
        f:close()
      elseif path == "/" then
        file.open("index.html")
        c:send(response("200 OK", file.read(), "text/html; charset=UTF-8"))
        file.close()
      else
        c:send(response("404 Not Found", "Error: 404 Not Found", "text/plain; charset=UTF-8"))
      end
    end)

    connection:on("sent", function(c) c:close() end)
  end)
end

server()
