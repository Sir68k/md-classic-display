# md-classic-display

This project is a modern replacement for classic LCD displays in Sony MiniDisc portables. It uses an ATTiny1614 controller with a TPS61046 boost circuit for driving an SSD1306 OLED.

The protocol is documented here: https://github.com/MiniDisc-wiki/md-firmware/wiki/LCD-SPI-protocol-(NetMD-devices)

Confirmed to work with: MZ-N510, MZ-N520, MZ-N505. Should work on all Sony NetMD devices (non-himd), and most likely will work on older devices (the MZ-R700 uses the exact samedisplay protocol, I only haven't tested it).

## How to use

Connect the OLED board to the SPI lines on the MD display board. VCC on the display board isn't stable enough, you should connect to a stable voltage line on the MD controller board (such as VCC for the remote, VCO1 on the MZ-N510).

## What needs to be improved

- I haven't implemented custom glyph support yet, so my tetris / raycaster demos will not work with this display
- There can be minor lag between two display updates, which sometimes is visible when text is scrolling. Can be fixed by updating the display in a slightly smarter way.
- Themes would be cool

## Photos 

<img src="images/test_a.jpg" width="300px">
<img src="images/a.jpg" width="300px">
<img src="images/b.jpg" width="300px">
<img src="images/install.jpg" width="300px">
<img src="board/pcb.png" width="300px">