<div align="center">
    <img width="128" src="https://github.com/atom0s/craft_extract/raw/main/repo/daochook.png" alt="daochook">
    <br/>
</div>

<div align="center">
    <a href="https://discord.gg/mkfP3XkZZ2"><img src="https://img.shields.io/discord/1022736642257211393.svg?style=for-the-badge" alt="Discord server" /></a>
    <a href="LICENSE.md"><img src="https://img.shields.io/badge/License-AGPL_v3-blue?style=for-the-badge" alt="license" /></a>
    <br/>
</div>

**craft_extract** is a tool used to parse and extract the Dark Age of Camelot crafting file information.

This tool can parse crafting file information for file versions: **v66**, **v67**

When extracting, there are options to save the parsed crafting recipes as: **csv**, **json**, **sqlite**, or **plain-text**

## Donations & Sponsorships

**craft_extract** is released completely free of charge. You can say thanks by donating or via sponsorships.

  * **GitHub Sponsor:** https://github.com/sponsors/atom0s
  * **Patreon Sponsor:** https://patreon.com/atom0s
  * **PayPal Donation:** https://www.paypal.me/atom0s

## Usage

**craft_extract** is a command line utility that takes several arguments in order to parse and extract the given input files craft information. _This tool expects you have already extracted the craft file (generally `tdl.crf`) from the main craft information mpk (generally `ifd.mpk`)._

```
Dark Age of Camelot Craft Information Extractor.
(c) 2022 atom0s [atom0s@live.com]

Contact  : https://atom0s.com/
Contact  : https://twitter.com/atom0s
Contact  : https://discord.gg/UmXNvjq - atom0s#0001
Donations: https://www.paypal.me/atom0s
Donations: https://github.com/sponsors/atom0s
Donations: https://patreon.com/atom0s

Extracts binary serialized craft information for DAoC.
Usage:
  craft_extract [options...]

  -f, --file arg  The input file to extract craft information from. (ie.
                  tdl.crf)
  -o, --out arg   The output file to save the extracted craft information
                  to.
  -m, --mode arg  The output file saving mode. (default: 0)

Modes:
  0 - none; will cause help info to display.
  1 - csv     - Information saved into a comma-separated value file.
  2 - json    - Information saved into a JSON formatted file.
  3 - sqlite  - Information saved into an SQLite database file.
  4 - text    - Information saved into a plain-text file.
```

Examples of using this tool are:

```
craft_extract.exe --file tdl.crf --out crafts.csv --mode 1
craft_extract.exe --file tdl.crf --out crafts.json --mode 2
craft_extract.exe --file tdl.crf --out crafts.sqlite --mode 3
craft_extract.exe --file tdl.crf --out crafts.text --mode 4
```

## For Developers

**craft_extract** is developed using `Visual Studio Code`. In order to compile the source code, you will need the following:

  * **Visual Studio Code (Insiders)**: https://code.visualstudio.com/insiders/
    * **Extension:** C/C++
    * **Extension:** CMake
    * **Extension:** CMake Tools
  * **CMake**: https://cmake.org/ _(v3.22.0 or newer!)_

## Legal

**craft_extract** does not claim ownership of any material(s) related to DAoC.

```
Dark Age of Camelot is copyright © 2021 Electronic Arts Inc.
All Rights Reserved.
```

The reverse engineering done by this repository and its contributors is entirely 'clean room'. We **DO NOT** have or use any leaked source code or other unpublished material. Any contributions made to **craft_extract** **MUST** adhere to the following requirements:

  * You are not employeed by EA, or have been previously, in any capacity.
  * You do not and have never had any leaked material from EA related to DAoC in any manner.
  * You do not and have never referenced any leaked or otherwise unreleased material from EA related to DAoC in any manner.

We **DO NOT** claim ownership of any material or information gathered through the means of reverse engineering the client and its files for this purpose.

## License

**craft_extract** is licensed under GNU AGPL v3

Please be sure you understand the license before making use of **craft_extract**. This is AGPL, not standard GPL.
