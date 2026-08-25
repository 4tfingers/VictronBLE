# scan — find your Victron devices

Logs every Victron "Instant Readout" advertisement in range. **No
advertisement keys and no device list required**, so this is the first thing
to run: it tells you what you have and what its Bluetooth address is.

The whole sample is `victronble_watch_set(true)` plus `victronble_start()` —
all the output comes from the library's own log module.

## Build and run

```sh
west build -p -b nrf52840dk/nrf52840 -d /tmp/vb_scan samples/scan \
    -- -DZEPHYR_EXTRA_MODULES=$PWD
west flash -d /tmp/vb_scan
```

Console (115200 baud):

```
<inf> scan: VictronBLE discovery — logging every Victron advert in range
<inf> victronble: observing (interval 96 window 48)
<inf> victronble: watch: E4:05:42:34:14:F3 (random) rssi -67 type 0x01 (solar charger) len 31 keycheck 0x0d
<inf> victronble: watch: E6:45:59:78:3C:FB (random) rssi -82 type 0x02 (battery monitor) len 31 keycheck 0x3f
<inf> scan: stats: adverts 61 queued 61 dropped 0
```

Each line gives you everything you need for `samples/observer`:

| Field | Use |
|---|---|
| address + `(random)` | Victron uses **random** static addresses — that address type matters |
| `type` | which device family it is, before any decryption |
| `keycheck` | first byte of the advertisement key; confirms you copied the right key |
| `rssi` | how well you can hear it — useful for placing the node |

Nothing here is decrypted. Watch mode reads only the plaintext header, which
is why it works without keys.

## Next step

Copy the addresses into the `known_devices` table in
[`../observer/src/main.c`](../observer/src/main.c) along with each device's key
from VictronConnect (device → gear icon → Product info → *Instant readout via
Bluetooth*), then build `observer`.

## Notes

- Unregistered devices are **not** deduplicated by nonce, so expect roughly
  one line per device per second. That is the point — it shows liveness.
- `prj.conf` raises the scan duty cycle to 60 ms / 30 ms
  (`BT_GAP_SCAN_FAST_*`) instead of the library default 1.28 s / 11.25 ms.
  Discovery should be quick; `observer` uses the low-power default.
- `CONFIG_VICTRONBLE_QUEUE_DEPTH=16` because watch mode queues every advert,
  not just the ones from known devices. If `dropped` climbs on a busy site,
  raise it further.
- `adverts 0` after a minute means either nothing is in range or the device
  has *Instant readout via Bluetooth* switched off in VictronConnect.
