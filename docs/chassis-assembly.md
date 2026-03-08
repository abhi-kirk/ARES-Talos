# Chassis Assembly — Talos

Reference for physical build decisions, component placement, and phased assembly plan.

---

## Hardware

- **Chassis**: Yahboom SBR self-balancing 2WD kit
- **Compute**: STM32 NUCLEO-F446RE (Nucleo-64, MB1136 Rev C)
- **Motor driver**: Sabertooth 2x12 RC
- **IMU**: BNO055 breakout (~25×25mm)
- **Battery**: 3S LiPo 2200mAh (too wide for bay — see below)
- **High-level compute (Phase 4+)**: Jetson Orin Nano Super

---

## Chassis Structure

The chassis is a rectangular aluminum box body with two wheels on the left/right faces:

```
Top view (robot standing upright):

        [front face]
  ┌──────────────────────┐
  │  chassis top face    │  ← thin removable panel, 4 screw positions at corners
  │  (second floor here) │    (battery width prevents full screw-down on one side)
  ├──────────────────────┤
  │                      │
  │  battery bay         │  ← battery slides in, accessed via removable cover
  │                      │
  └──────────────────────┘
        [rear face]

Wheels on left and right sides (not shown).
```

The **chassis top face plate** is a separate thin metal panel that normally screws onto the chassis body. The 2200mAh battery is slightly too wide, preventing full screw-down on one side — the plate gets partial structural support from the screws that do catch.

---

## Battery Issue

The 3S 2200mAh LiPo is too wide for the battery bay — it protrudes slightly, preventing the top face plate from fully seating. Options evaluated:

| Option | Notes |
|---|---|
| Replace with 3S 1000–1300mAh (smaller pack) | Cleanest fix; fits bay; lighter; better dynamics. Recommended for Phase 4+. |
| Mount 2200mAh externally (velcro to rear face) | Works; shifts CoM backward (compensate with PITCH_SETPOINT offset in Phase 3 PID) |
| **Use 2200mAh in bay, no cover, stabilize plate with tape + strap** | **Phase 3 approach — chosen** |

For Phase 3 the battery sits in the bay without the cover. The top face plate is stabilized with VHB tape on contact edges and a velcro strap around the chassis body.

---

## Nucleo Mounting Holes

The Nucleo-64 has **3 mounting holes** (not 4 corners):

```
┌──────────────────────────────┐
│ ●  (top-left, ST-Link area)  │
│                              │
│                              │
│ ●                          ● │
│ (bottom-left)    (bottom-right)
└──────────────────────────────┘
```

These 3 holes form a triangle. All are M3 clearance (3.2mm diameter). Used as standoff attachment points for both Nucleo mounting and (Phase 4+) the second floor plate.

---

## Phase 3 Assembly — No Drilling, Full Tape

Goal: get the robot balanced on the chassis with STM32 only. Reversible, tool-free, open access to Nucleo pins for wiring and debugging.

### Stack (bottom to top)

```
[ BNO055 ]                     ← foam tape on Nucleo surface
[ Nucleo PCB ]                 ← adhesive push-in standoffs on chassis plate
[ chassis top face plate ]     ← VHB tape + velcro strap stabilization
[ chassis body ]               ← battery in bay (no cover)
```

### Component Placement

**Battery**
- In bay, no cover
- Retention: velcro strap wrapped around chassis body + plate

**Chassis top face plate**
- Screws tightened where they catch (partial fit)
- VHB tape strips applied to contact edges between plate and chassis body
- Velcro strap around full chassis body as backup clamping force

**Nucleo**
- Flat on chassis top face plate, component side up
- Mounted with peel-and-stick adhesive PCB standoffs (0.31" / 8mm height) at the 3 mounting hole positions — expanding clip pin goes through Nucleo hole, locks without screws
- Additional VHB tape patches at 1–2 PCB edges for supplemental hold
- Fully accessible from above and sides — all pins, USB, and RESET exposed

**BNO055**
- Flat on Nucleo PCB surface, foam tape
- **Critical orientation**: BNO055 X-axis must be parallel to the wheel axle (left-right of robot)
- With PS0 edge facing upward, forward lean → pitch positive (verify after assembly)
- See `docs/bno055.md` for full axis mapping and mounting guidance

**Sabertooth**
- Mounted to chassis side face (front or rear panel) using VHB tape on its flat aluminum back
- Kept low and close to motors/battery — short power and signal leads
- For Phase 4+: replace tape with M3 screws through drilled holes in chassis panel

### Parts Used

| Item | Spec | Use |
|---|---|---|
| 3M VHB foam tape | 0.5" wide, 0.025" thick | Chassis plate edges, Nucleo supplemental hold, BNO055 to Nucleo, Sabertooth to side panel |
| Velcro strap (hook+loop roll) | 1" wide, cut to length | Battery retention, chassis plate clamping |
| Adhesive PCB standoffs | Push-in nylon, 0.31" (8mm) height | Nucleo mounting on chassis plate — 3 standoffs at Nucleo hole positions |
| Zip ties | 100mm | Wire management throughout |

---

## Phase 4 Assembly — Proper Mounting + Jetson

When adding the Jetson Orin Nano (Phase 4), the tape-only approach gets replaced with drilled and bolted mounting.

### Changes from Phase 3

1. **Drill 3 holes in chassis top face plate** — positions matching Nucleo mounting holes
2. **M3 screws from below chassis plate** → through plate → through Nucleo holes → M3 nut on top of Nucleo PCB (clamps plate + Nucleo rigidly)
3. **M3 hex standoffs** screwed onto those nuts, extending upward from the Nucleo surface
4. **Custom second floor plate** (cut acrylic or aluminum, ~120×100mm) with 3 holes matching Nucleo hole pattern — sits on top of the 3 standoffs
5. **Jetson Orin Nano** mounted on second floor plate

### Second Floor Stack

```
[ Jetson Orin Nano ]           ← M3 screws to second floor plate
[ second floor plate ]         ← custom cut, 3 holes at Nucleo hole positions
  |           |           |
  M3 standoffs (25–30mm tall)  ← must clear Nucleo USB connector (~13mm) + margin
  |           |           |
[ Nucleo PCB ]                 ← M3 screw/nut through chassis plate
[ chassis top face plate ]     ← drilled, screwed down properly
[ chassis body ]
```

Standoff height: **25–30mm** above Nucleo PCB surface. The Nucleo's tallest component is the mini-USB connector (~13mm). 25mm gives ~12mm of clearance for wires above the Nucleo.

### Sabertooth Phase 4

Replace VHB tape with M3 screws through drilled holes in chassis side panel. Sabertooth mounting hole pattern: 1.5" × 2" (four corners), M3 compatible.

### Parts Needed for Phase 4

| Item | Spec | Qty |
|---|---|---|
| M3 nylon hex standoffs (male-female) | 25–30mm | 3 |
| M3 screws | 8mm | 6 |
| M3 hex nuts | standard | 3 |
| Custom second floor plate | ~120×100mm, 2–3mm acrylic or aluminum | 1 |
| M3 screws | 12mm | 4 |
| Drill bit | 3.2mm | 1 |
| Digital calipers | any | 1 (if not already owned) |

The M3 nylon standoff kit (580 pcs assortment) already ordered covers the standoffs, screws, and nuts for this.

---

## Center of Mass Notes

- Battery low (in chassis body) = low CoM = good for stability
- Jetson high (second floor) = high CoM = slower pendulum frequency = easier to control
- Sabertooth on side face = minimal CoM impact if centered front-to-back
- Nucleo at bottom of stack = minimal CoM impact

**Before Phase 3 first spin-up**: hold the assembled robot and find the front-back balance point by feel. If it naturally leans forward or backward at rest, the `PITCH_SETPOINT` in the Phase 3 PID absorbs this offset — no need to physically re-center.

---

## IMU Orientation After Assembly

After mounting on chassis, verify before running any balance control:

1. Robot upright on floor, held near vertical
2. Watch `pitch` in PlotJuggler
3. Tilt robot forward (top falls forward) → confirm `pitch` goes **positive**
4. If negative: set `PITCH_SIGN = -1` in Phase 3 firmware (do not modify driver)
5. Note the `pitch` reading when robot is held at approximate balance point → this becomes `PITCH_SETPOINT`

See `docs/bno055.md` — Mounting Guidance section.
