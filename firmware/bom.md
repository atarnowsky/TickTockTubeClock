# Bill of materials

All costs are approximate (in €) basedon Digikey prices as of 2022. (Including bulk discount, values are for reference only.)


## Upper PCB

| Comment | Description | Designator | Footprint | Amount | Price (sum.) | Link | Remarks |
| ------- | ----------- | ---------- | --------- | ------ | ------------ | ---- | ------- |
| Old stock | Hitachi CD81 | NX1, NX2, NX3, NX4 | Custom | 4 |  n/a | n/a | Might be salvaged from old Canon calculators. (Do not get fooled by Ebay prices!) |
| ESR10EZPJ163 | 16k Resistor, 0.4 W | R1, R32, R57, R58 | 0805 | 4 | 0.12 € | [Digikey](https://www.digikey.de/de/products/detail/rohm-semiconductor/ESR10EZPJ163/4051315) | Be careful wrt. voltage ratings. |
| RCS08052K49FKEA | 2.4k Resistor, 0.4 W | R55, R56 | 0805 | 2 | 0.39 € | [Digikey](https://www.digikey.de/de/products/detail/vishay-dale/RCS08052K49FKEA/5869310) | Be careful wrt. voltage ratings. |
| ESR03EZPJ103 | 10k Resistor, 0.125 W | R33-R54 | 0603 | 22 | 0.62 € | [Digikey](https://www.digikey.de/de/products/detail/rohm-semiconductor/ESR03EZPJ103/1762722) |  |
| MMBTA42,215 | NPN HV-Transistor | Q1-Q11, Q16-Q26 | SOT-23-3 | 22 | 2.18 € | [Digikey](https://www.digikey.de/de/products/detail/nexperia-usa-inc/MMBTA42-215/1156601) | |


## Lower PCB

| Comment | Description | Designator | Footprint | Amount | Price (sum.) | Link | Remarks |
| ------- | ----------- | ---------- | --------- | ------ | ------------ | ---- | ------- |
| CMT-5023S-SMT-TR | Buzzer 5x5mm | BUZZER | Custom | 1 |  2.55 € | [Digikey](https://www.digikey.de/de/products/detail/cui-devices/CMT-5023S-SMT-TR/6012414) | Cheaper alternative from [LCSC](https://lcsc.com/product-detail/Buzzers_KELIKING-KLJ-5018_C201045.html) |
| OPT3001DNPR | Ambient light sensor | U3 | 6-UDFN | 1 | 2,77 € | [Digikey](https://www.digikey.de/de/products/detail/texas-instruments/OPT3001DNPR/5039362) | Hard to solder by hand | 
| SMMBTA92LT1G | PNP HV-Transistor | Q12, Q14 | SOT-23-3 | 2 | 0.58 € | [Digikey](https://www.digikey.de/de/products/detail/onsemi/SMMBTA92LT1G/3062778) ||
| MMBTA42,215 | NPN HV-Transistor | Q13, Q28 | SOT-23-3 | 2 | 0.20 € | [Digikey](https://www.digikey.de/de/products/detail/nexperia-usa-inc/MMBTA42-215/1156601) | |
| MFU0805FF00500P100 | Fuse 500mA | F1 | 0805 | 1 | 0,49 € | [Digikey](https://www.digikey.de/de/products/detail/vishay-beyschlag-draloric-bc-components/MFU0805FF00500P100/1206486) | |
| STS321050B331 | TVS Diode 5.5V | ? | SOD-323 | 1 | 0.25 € | [Digikey](https://www.digikey.de/de/products/detail/eaton-electronics-division/STS321050B331/13280450) ||
| SN74HC595DR | Shift register 8bit | U7, U8, U9 | SOIC-16 | 3 | 1.34 € | [Digikey](https://www.digikey.de/de/products/detail/texas-instruments/SN74HC595DR/562919) ||
| LM3940IMPX-3.3/NOPB | 3.3V Linear regulator | U4 | SOT-223-4 | 1 | 1.46 € | [Digikey](https://www.digikey.de/de/products/detail/texas-instruments/LM3940IMPX-3-3-NOPB/367097) ||
| C3216X5R1C336M160AB | Capacitor 33µF | C3 | 1206 | 1 | 0.70 € | [Digikey](https://www.digikey.de/de/products/detail/tdk-corporation/C3216X5R1C336M160AB/2443480) || 
| CL21B474KAFNNNE | Capacitor 470nF | C4 | 0805 | 1 | 0.03 € | [Digikey](https://www.digikey.de/de/products/detail/samsung-electro-mechanics/CL21B474KAFNNNE/3886732) ||
| CL10B104KB8WPNC | Capacitor 100nF | C2, C5-C10 | 0603 | 7 | 0.13 € | [Digikey](https://www.digikey.de/de/products/detail/samsung-electro-mechanics/CL10B104KB8WPNC/5961338) ||
| ERJ-P06J151V | Resistor 150 | R2, R9-R14, R24 | 0805 | 8 | 0.29 € | [Digikey](https://www.digikey.de/de/products/detail/panasonic-electronic-components/ERJ-P06J151V/525217) ||
| RMCF0805JG10K0 | 10k Resistor, 0.125 W | R3, R5, R8, R25, R26, R28, R29 | 0805 | 7 | 0.09 € | [Digikey](https://www.digikey.de/de/products/detail/stackpole-electronics-inc/RMCF0805JG10K0/1757769) |  | 
| RMCF0805JT1M00 | 1M Resistor, 0.125 W | R27, R30 | 0805 | 2 | 0.02 € | [Digikey](https://www.digikey.de/de/products/detail/stackpole-electronics-inc/RMCF0805JT1M00/1757878) || 
| UJC-HP-3-SMT-TR | USB-C Terminal | USB1 | | 1 | 0.88 € | [Digikey](https://www.digikey.de/de/products/detail/cui-devices/UJC-HP-3-SMT-TR/12173295) || 
| DS3232MZ+TRL | RTC integrated Osc. | U2 | SOIC-8 | 1 | 7,91 € | [Digikey](https://www.digikey.de/de/products/detail/analog-devices-inc-maxim-integrated/DS3232MZ-TRL/3087654) || 
| RB168MM-30TR | Diode (Schottky) | D1 | SOD-123 | 1 | 0.31 € | [Digikey](https://www.digikey.de/de/products/detail/rohm-semiconductor/RB168MM-30TR/10670912) | Use low drop out AND very low leakage current diode (standard or schottky) to prolong buffer time | 
| DMN2041L-7 | MOSFET | Q15, Q27 | SOT-23-3 | 2 | 0.36 € | [Digikey](https://www.digikey.de/de/products/detail/diodes-incorporated/DMN2041L-7/2182568) | Use any FET with low Vgs(th), 3.3V compatible |