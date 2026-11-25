# SimpBMS (BMWPhevBMS) review: X5 45e SP44, 12 CSC × 8 cells

Reviewed: `BMS/SimpBMS_X5&CurrentFix/BMWPhevBMS` against upstream `Tom-evnut/BMWPhevBMS` (latest commit 91a7b12).
Your changes vs upstream: module loop `nextmes == 12`, cell arrays 16→8, ISA 0x521–0x523 byte order, one print commented out.
Patched copy: `BMS/SimpBMS_X5_Claude_Review/BMWPhevBMS/`. Every change is tagged `CLMod`. Nothing in your original folder was touched.
Not compiled here (no Teensyduino in this workspace); the module .cpp files pass a syntax check. Compile in Arduino IDE before flashing.

## Bugs

| # | Severity | Where | Problem | Fix in patched copy |
|---|---|---|---|---|
| 1 | High | `.ino` loop, `setBalIgnore` | Upstream bug: `balancetimer` re-arms every ~2 min whether or not balancing, and cell readings are ignored for the last `balanceDuty+5` s of each window. With duty 60 s, cell voltages freeze about 65 s of every ~2 min, including while driving and charging. UV/OV protection and the 0x373 values sent to ZombieVerter run on stale data for that time. | Ignore readings only while bleed is on, plus a 3 s settle. |
| 2 | Medium | `BMSModule::decodecan` | Your 16→8 array change left the decoder writing `cellVolt[8..15]` (frames 0x14x–0x17x). That writes past the array into `lowestCellVolt[]`. Undefined behaviour; today it only corrupts min/max tracking, but it can change with any rebuild. | Storage back to 16; all loops/limits use `CELLS_PER_MODULE = 8`. |
| 3 | Medium | Charge state | Balancing during charge freezes readings each bleed phase, so the 4.1 V charge stop and OV trip act on old values. | Balance only at rest (Ready). `BAL_DURING_CHARGE` in CONFIG.h to re-enable. |
| 4 | Medium | Drive state | `balancecells` kept its last value, so bleeding could continue while driving. | Forced to 0 in Drive, Precharge, Error. |
| 5 | Low | `getModuleVoltage()` | `for (int I; ...)` — `I` uninitialised (upstream). Works by luck. | `int I = 0`. |
| 6 | Low | `getCellVoltage()` etc. | Bound `cell > 8` allows index 8 (off by one). | `>= CELLS_PER_MODULE`. |
| 7 | Low | `canread()` / `decodecan` | Any ID 0x0A0–0x17F with low nibble ≥ 0xC created a phantom module (cell-count error). | Accept only 0x100–0x17F, CSC 1–12. |
| 8 | Low, not changed | `checkstatus()` | `getError() & 0x2000 >= 0` evaluates as `& 1` (precedence). Sends 0x0B0 "reset balance debug" whenever status bit 0 is set. Intent unclear, so left as is. |
| 9 | Info, not changed | Series-cell fault | Print is commented out, so a dropped module goes to Error silently. Consider restoring it rate-limited. |

## ISA shunt byte-order fix

Your change reads 0x521–0x523 bytes 2–5 little-endian. Upstream reads them big-endian, which is the IVT-S factory format.
Your version is right **only if the shunt has been reconfigured to little-endian** — most likely by ZombieVerter's ISA initialisation (from memory, not verified here).
Risk: if the shunt is ever factory-reset, or ZombieVerter stops configuring it, SimpBMS current and SOC will be garbage.
Check: read the shunt config, or confirm SimpBMS current matches ZombieVerter `idc` after a cold power-up with ZombieVerter off.

## Balancing: what was actually missing

The balance command already exists. Tom added it in July 2020 (commit 4f63e7c, "Implemented Balancing commands"). The empty `balancing()` stub is dead code.

Command frame 0x080+n (n = CSC 0–11), 8 bytes, as sent:

| Byte | Not balancing | Balancing |
|---|---|---|
| 0–1 | 0x10C7 (4295 mV, above any cell) | lowest cell mV + 5, little-endian: CSC bleeds cells above this |
| 2–3 | 0x00 | 0x00 |
| 4 | 0x40 | 0x48 (bit 3 = balance enable) |
| 5 | 0x01 | 0x01 |
| 6 | rolling counter, high nibble | same |
| 7 | CRC8 with per-ID final XOR (table has 12 entries, so CSCs 9–12 are covered) | same |

Feedback: status frame 0x100+n, bytes 4–5 = `balstat` (shown as `Bal: 0x....` in the `d` debug print). Assumed to be a per-cell bleed bitmap; not confirmed for SP44 CSCs.

What was wrong was the control around it: bug 1 above, no stop hysteresis (it stopped at a 40 mV spread, so the pack never got better than ~40 mV), and balancing allowed during charge and drive.

New control:
- Start: top cell > `balanceVoltage` and spread > `balanceHyst` (your sheet: 3.90 V, 40 mV).
- Stop: spread < 10 mV (`BAL_STOP_DELTA`), or top cell < `balanceVoltage − balanceHyst`.
- Duty cycle: bleed `balanceDuty` s, rest 10 s (`BAL_REST_MS`) with bleed off; readings trusted 3 s after bleed stops.
- Only in Ready (key off, not charging). After a charge to 4.1 V the pack goes to Ready and balances there.

## Not yet known (needs a bench test)

1. Whether SP44 CSCs honour this command. Your CSCs accept the PHEV-style frame for measurements, which is encouraging but not proof.
2. Bleed current per cell. Unknown. It sets how long balancing takes; measure it.
3. Whether SimpBMS stays powered after charging. If it loses 12 V when charging ends, it never balances. Check your wiring.

## Bench test

1. Flash the patched build. Serial menu: set balance voltage below current cell voltage (e.g. 3600 mV) and hysteresis 10 mV, temporarily. Pack at rest, key off, no charger.
2. `d` debug view. Expect `Balancing Active BLEED` / `REST`, and `Bal: 0x....` non-zero on modules with cells above lowest + 5 mV.
3. Confirm it's real: high cells should drop a few mV over 10–20 min relative to the lowest; balance resistors warm (thermal camera or finger on CSC board area); module 12 V/CSC supply current rises slightly during BLEED.
4. If `Bal:` stays 0 and nothing drops: the enable bit or byte layout differs on SP44. Next step is a CAN capture of an original BMW SME driving these CSCs (forum/community capture) — don't guess bits on live cells.
5. Restore balance voltage 3900 mV and hysteresis 40 mV.

This touches cell protection on a 400 V pack. Worth verifying properly: watch the first real charge + balance cycle with the debug view open.
