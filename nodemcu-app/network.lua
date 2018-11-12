
function ntp_sync()
  sntp.sync("time.windows.com",
  function(sec, usec, server, info)
    print('sync', sec, usec, server)
  end,
  function()
   print('failed!')
  end
)
end

function ap_connected(i)
  print("connected_to: "..i.SSID..")")
end

function ap_got_ip(i)
  -- Internet connectivity can be determined with net.dns.resolve().
  print("got_ip: "..i.IP)
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
  print("station connected  " .. i.MAC)
end

function sta_disconnected(i)
  print("station disconnected  " .. i.MAC)
end

function network_init()
  -- TODO: pass config as argument
  wifi.setmode(wifi.STATIONAP)

  wifi.ap.config({ssid="slon-ds-player", auth = wifi.WPA2_PSK, pwd="adminadmin", staconnected_cb=sta_connected, stadisconnected_cb=sta_disconnected, save=false })
  wifi.ap.setip({ip="10.10.0.1", netmask="255.255.255.0", gateway="10.10.0.1"})

  wifi.sta.config({ssid="flhome", pwd="password_changed", save=false, connected_cb=ap_connected, disconnected_cb=ap_disconnected, got_ip_cb=ap_got_ip})
end