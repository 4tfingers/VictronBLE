/**
 * VictronBLE Zephyr discovery sample.
 *
 * Turns on the library's watch mode and does nothing else. No advertisement
 * keys, no device list: every Victron Instant Readout advert in range is
 * logged with its address, RSSI, device type and key-check byte.
 *
 * Run this first to find out what you have and what its MAC address is, then
 * put the addresses and keys into samples/observer.
 *
 * Copyright (c) 2026 Scott Penrose
 * License: MIT
 */

#include <zephyr/kernel.h>
#include <zephyr/bluetooth/bluetooth.h>
#include <zephyr/logging/log.h>

#include "victronble_zephyr.h"

LOG_MODULE_REGISTER(scan, LOG_LEVEL_INF);

#define STATS_INTERVAL K_SECONDS(30)

int main(void)
{
	struct victronble_stats stats;
	int err;

	LOG_INF("VictronBLE discovery — logging every Victron advert in range");

	/* The application owns the Bluetooth stack: victronble_start() needs
	 * it up already. */
	err = bt_enable(NULL);
	if (err != 0) {
		LOG_ERR("bt_enable failed (%d)", err);
		return 0;
	}

	/* All output comes from the library's own log module (victronble).
	 * Unregistered devices are not nonce-deduplicated, so expect roughly
	 * one line per device per second. */
	victronble_watch_set(true);

	err = victronble_start();
	if (err != 0) {
		LOG_ERR("victronble_start failed (%d)", err);
		return 0;
	}

	while (1) {
		k_sleep(STATS_INTERVAL);

		victronble_get_stats(&stats);
		LOG_INF("stats: adverts %u queued %u dropped %u", stats.adverts,
			stats.queued, stats.dropped);

		if (stats.adverts == 0) {
			LOG_WRN("nothing heard yet — check the device has "
				"'Instant readout via Bluetooth' enabled in "
				"VictronConnect");
		}
	}

	return 0;
}
