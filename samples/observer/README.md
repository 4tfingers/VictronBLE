# observer — decode records from known devices

Monitors a list of Victron devices, decrypts their Instant Readout
advertisements and logs every field. This is the reference for using the
Zephyr API.

Don't know your devices' addresses yet? Build [`../scan`](../scan/) first.

## Configure your devices

Edit the `known_devices` table at the top of [`src/main.c`](src/main.c):

```c
static const struct { ... } known_devices[] = {
	{
		.name = "Rainbow48V",
		.addr = "E4:05:42:34:14:F3",
		.addr_type = "random",
		.key = "0ec3adf7433dd61793ff2f3b8ad32ed8",
	},
};
```

The key is in VictronConnect: device → gear icon → Product info → *Instant
readout via Bluetooth* → show/copy the encryption key (32 hex characters).

**The address type must be `random`.** Victron devices use random static
Bluetooth addresses, and the registry lookup compares the type as well as the
bytes. Get it wrong and the symptom is quiet and confusing: `adverts` climbs
in the stats line but `decoded` stays at zero, because the adverts arrive and
never match a registered device.

Add more than four devices and you also need to raise
`CONFIG_VICTRONBLE_MAX_DEVICES` in `prj.conf`.

## Build and run

```sh
west build -p -b nrf52840dk/nrf52840 -d /tmp/vb_obs samples/observer \
    -- -DZEPHYR_EXTRA_MODULES=$PWD
west flash -d /tmp/vb_obs
```

Console (115200 baud):

```
<inf> observer: VictronBLE observer starting
<inf> observer: monitoring Rainbow48V (E4:05:42:34:14:F3)
<inf> victronble: observing (interval 2048 window 18)
<inf> observer: E4:05:42:34:14:F3 (random)  solar charger  rssi -67  model 0xa060  nonce 4660
<inf> observer:   state bulk  error 0
<inf> observer:   battery 13.24 V 5.4 A   pv 340 W   yield today 1200 Wh
<inf> observer:   load n/a
<inf> observer: stats: adverts 61 queued 30 dropped 0 decoded 28 dup 2 err 0
```

## Reading the stats line

Printed every 30 s, and the fastest way to diagnose a quiet console:

| Counter | Meaning if it misbehaves |
|---|---|
| `adverts` | Victron adverts seen from **any** device. Zero → nothing in range, or Instant Readout is off on the device. |
| `queued` | adverts from your registered devices. Zero while `adverts` climbs → wrong address or wrong address type. |
| `dropped` | queue was full. Raise `CONFIG_VICTRONBLE_QUEUE_DEPTH`. |
| `decoded` | records delivered to the callback. |
| `dup` | repeats suppressed by nonce dedup. A steady trickle is normal — each advert is broadcast on three channels. |
| `err` | decode failures. A wrong key shows up here, and in the `decode failed: key mismatch` warning. |

## What the code demonstrates

- `bt_enable()` **before** `victronble_start()` — the application owns the
  Bluetooth stack, the library only scans.
- `bt_addr_le_from_str()` + `victronble_parse_key()` to turn human-readable
  config into what `victronble_device_add()` wants.
- `victronble_cb_register()` with both `record` and `decode_error` set.
  Callbacks run on the library's decode thread, not the Bluetooth RX thread,
  so logging in them is safe.
- Formatting a record: `isnan()` guards on every float, and
  `remaining_minutes == 0xFFFF` for time-to-go. Absent fields are normal —
  an MPPT with no load output always reports `load n/a`.
- `victronble_get_stats()` for the periodic health line.

Printing floats needs `CONFIG_CBPRINTF_FP_SUPPORT=y`; without it the numbers
come out empty.

To decode a captured advert on your PC instead — no board involved — see
[`examples/NativeDecode`](../../examples/NativeDecode/).
