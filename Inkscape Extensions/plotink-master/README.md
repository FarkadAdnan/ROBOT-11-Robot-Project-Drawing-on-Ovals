# plotink
Helper routines for Inkscape extensions to drive EggBot, WaterColorBot, AxiDraw, and similar plotter-type machines.

These library files are intended to provide a single place to edit routines that are common to both the [EggBot extensions for Inkscape](https://github.com/evil-mad/EggBot/), the [WaterColorBot extensions for Inkscape](https://github.com/evil-mad/wcb-ink/), the [AxiDraw extensions for Inkscape](https://github.com/evil-mad/axidraw/), and derivative machines. Hosting these in a single place means less duplication of code, and (more importantly) reduces the number of cases where identical code changes need to be made in multiple places.


The three files are:

* ebb_serial.py - Routines for communicating with the EiBotBoard by USB serial (PySerial 2.7 + required).
* ebb_motion.py - Common commands for interacting with the robot
* plot_utils.py - Additional helper functions for managing the plot.

ebb_serial.py - has been modified to remove the check for an EBB board so that it will work with the emulated firmware EggDuino

https://github.com/plex3r/EggDuino