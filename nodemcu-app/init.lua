
function ntp_callback(sec, usec, server, info)
  print("settime "..sec)
end

NTP_SERVERS = { '0.debian.pool.ntp.org', '1.debian.pool.ntp.org', '2.debian.pool.ntp.org', '3.debian.pool.ntp.org',
  '0.opensuse.pool.ntp.org', '1.opensuse.pool.ntp.org', '2.opensuse.pool.ntp.org', '3.opensuse.pool.ntp.org',
  '0.pool.ntp.org', '1.pool.ntp.org', '2.pool.ntp.org', '3.pool.ntp.org', 'time.windows.com', 'pool.ntp.org',
  '0.ru.pool.ntp.org', '1.ru.pool.ntp.org', '2.ru.pool.ntp.org', '3.ru.pool.ntp.org' }
function ntp_sync()
  sntp.sync(NTP_SERVERS, ntp_callback, nil, false)
end

function ap_connected(i)
  print("connected_to "..i.SSID..")")
end

function ap_got_ip(i)
  -- Internet connectivity can be determined with net.dns.resolve().
  print("got_ip "..i.IP)
  ntp_sync()
end

function ap_disconnected(i)
  --[[if T.reason == wifi.eventmon.reason.ASSOC_LEAVE then
    --the station has disassociated from a previously connected AP
    return
  end
]]
  print("diconnected")

  --There are many possible disconnect reasons, the following iterates through
  --the list and returns the string corresponding to the disconnect reason.
  --[[for key,val in pairs(wifi.eventmon.reason) do
    if val == T.reason then
      print("Disconnect reason: "..val.."("..key..")")
      break
    end
  end]]
end


function sta_connected(i)
  print("station connected " .. i.MAC)
end

function sta_disconnected(i)
  print("station disconnected " .. i.MAC)
end

function network_init()
  -- TODO: pass config as argument
  wifi.setmode(wifi.STATIONAP)

  wifi.ap.config({ssid="slon-ds-player", auth = wifi.WPA2_PSK, pwd="adminadmin", staconnected_cb=sta_connected, stadisconnected_cb=sta_disconnected, save=false })
  wifi.ap.setip({ip="10.10.0.1", netmask="255.255.255.0", gateway="10.10.0.1"})

  wifi.sta.config({ssid="flhome", pwd="password_changed", save=false, connected_cb=ap_connected, disconnected_cb=ap_disconnected, got_ip_cb=ap_got_ip})
end

function startup()
  if file.open("init.lua") == nil then
    print("init.lua deleted or renamed")
  else
    file.close("init.lua")
    dofile("app.lua")
  end
end

network_init()
tmr.create():alarm(5000, tmr.ALARM_SINGLE, startup)
