# Zephyr samples

Zephyr applications live here; `examples/` holds the Arduino/PlatformIO ones.

| Sample | What it does |
|---|---|
| [`scan/`](scan/) | Lists every Victron device advertising nearby. No keys needed. **Start here.** |
| [`observer/`](observer/) | Monitors known devices and decodes their records. |

Both need `CONFIG_VICTRONBLE=y`, which depends on `CONFIG_BT_OBSERVER=y`. See
the Zephyr section of the top-level [README](../README.md) for how to add this
library to a west workspace.

## Building

The library is a Zephyr module. If it is already in your `west.yml`, the
samples build with no extra flags:

```sh
west build -p -b nrf52840dk/nrf52840 samples/observer
```

For local development against a checkout that is *not* in the manifest, point
Zephyr at it directly:

```sh
west build -p -b nrf52840dk/nrf52840 -d /tmp/vb_obs \
    /path/to/VictronBLE/samples/observer \
    -- -DZEPHYR_EXTRA_MODULES=/path/to/VictronBLE
```

Then `west flash`, and watch the console at 115200 baud.

Tested on `nrf52840dk/nrf52840` and `rak4631/nrf52840` with Zephyr v4.4.0. Any
board with a Bluetooth controller and the observer role should work — nothing
in the library is nRF-specific.

Build-test both samples without hardware. For a checkout outside the manifest,
twister needs the same module hint via the environment:

```sh
ZEPHYR_EXTRA_MODULES=$PWD \
    west twister -T samples -p nrf52840dk/nrf52840 -p rak4631/nrf52840
```
