![](./favicon-64x64.png) Settings in Aerofly's `main.mcf`
=========================================================

This tool modifies the following settings in Aerofly's `main.mcf`:

```
<[tm_time_utc][time_utc][]      # Time is set in UTC
    <[int32][time_year][2016]>  # Year as four digit integer, can not be set in simulation
    <[int32][time_month][5]>    # Month as 1-2 digit integer, can not be set in simulation
    <[int32][time_day][20]>     # Day of month as 1-2 digit integer, can not be set in simulation
    <[float64][time_hours][0]>  # UTC time; minutes as fractions of 100/60
>
<[float64][visibility][0]>      # Percent, 0..1
<[tmsettings_wind][wind][]
    <[float64][strength][0]>             # Percent, 0..1, but can be set bigger than 1
    <[float64][direction_in_degree][0]>  # 0..359
    <[float64][turbulence][0]>           # Percent, 0..1
    <[float64][thermal_activity][0]>     # Percent, 0..1
>
<[tmsettings_clouds][clouds][]
    <[float64][cumulus_density][0]>            # Percent, 0..1
    <[float64][cumulus_height][0]>             # Percent, 0..1, but can be set bigger than 1
    <[float64][cumulus_mediocris_density][0]>  # see above, can not be set in simulation
    <[float64][cumulus_mediocris_height][0]>   # see above, can not be set in simulation
    <[float64][cirrus_height][0]>              # see above
    <[float64][cirrus_density][0]>             # see above
>
```

Visibility
----------

Visibility tests set maximum visibility in Aerofly FS 2 to 20,000 meters.

Wind
----

Setting the **wind strength** to `1` yields a wind speed of ~16 knots; setting it to `2` yields ~48 knots, and `4` yields ~160 knots. This implies an exponential function, which can be approximated by `f(x) = 8 * (x + x^2)`, with `x` being the value set and `f(x)` returning the resulting wind speed in knots.

**Turbulence** causes the wind to change its direction of attack in Aerofly FS2, as well as change its speed. The direct impact has not been measured yet.

The effect of **thermal activity** has not been measured yet.

Cloud height
------------

Setting the cloud height to `1` yields a cloud height of 40.000 feet AGL.

---

Return to [table of contents](README.md).
