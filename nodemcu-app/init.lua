-- load credentials, 'SSID' and 'PASSWORD' declared and initialize in there
-- dofile("credentials.lua")

dofile("network.lua")

network_init()

function startup()
  if file.open("init.lua") == nil then
    print("init.lua deleted or renamed")
  else
    file.close("init.lua")
    dofile("app.lua")
  end
end

tmr.create():alarm(5000, tmr.ALARM_SINGLE, startup)
