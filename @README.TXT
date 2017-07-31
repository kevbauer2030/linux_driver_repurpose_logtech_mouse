October 1, 2016
OVERVIEW AND CURRENT STATUS: 
1. This project contains source code for a driver that will overide the default Logitech driver on a recent Linux system.
2. Possible uses for this would be:
   a) Modern example of how to write and install a driver.  There is one commonly referenced example on how to do this from the Linux Journal, but it is a few years old, and the particulars of doing this have changed.
  b) If you wanted to use the mouse for some other kind of function, aside from pointing.  Since it is an optical mouse, the device could potentially be used for measuring some types of motion (for example the motion of a tire assuming it is not too fast).  The buttons could be used for measuring anything that a button can measure, for example, a door opening.
4. This USB Driver will talk to the Logitech mouse and report the mouse directions with dmesg's below.
5. It is necessary to setup the URB correctly for an interrupt transfer, add a command to re-submit the URB in the callback function.  This is done in the source code.
6. Example DMEGS's printed out from the driver are below.
-------------
[ 1041.484059] SKELETON: Initializing function
[ 1041.484105] USB-SKEL: Probe activated
[ 1041.484108] Device Number = 4
[ 1041.484110] Device Path=1
[ 1041.484112] Bus mA=500
[ 1041.484114] Speed is 1
[ 1041.484116] Can submit URBs 1
[ 1041.484118] Product String=USB Optical Mouse
[ 1041.484120] Manufacturer=Logitech
[ 1041.484121] Serial=NULL
[ 1041.484123] Found endpoint 0, type is 3
[ 1041.484125] Packetsize is 4
[ 1041.484127] Endpoint address is 129
[ 1041.484129] This is my interrupt endpoint, setting up URB to receive
information
[ 1041.484132] Allocated buffer of size 4 address ffff880036b95fa0
[ 1041.484142] Return value from usb_submit was 0
[ 1041.484251] USB Skeleton device now attached to USBSkel-0
[ 1041.484298] usbcore: registered new interface driver skeleton
[ 1044.347073] MOUSE moved UP LEFT ....4 bytes in. Vertical=253,
Horizontal=255
[ 1044.355085] MOUSE moved UP RIGHT ....4 bytes in. Vertical=252,
Horizontal=1
[ 1044.363096] MOUSE moved UP RIGHT ....4 bytes in. Vertical=255,
Horizontal=3
[ 1044.371101] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=2
[ 1044.379052] MOUSE moved UP RIGHT ....4 bytes in. Vertical=255,
Horizontal=4
[ 1044.387008] MOUSE moved UP RIGHT ....4 bytes in. Vertical=255,
Horizontal=2
[ 1044.395128] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=2
[ 1044.403137] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=3
[ 1044.411088] MOUSE moved UP RIGHT ....4 bytes in. Vertical=254,
Horizontal=4
[ 1044.419154] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=1
[ 1044.427165] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=5
[ 1044.435175] MOUSE moved UP RIGHT ....4 bytes in. Vertical=255,
Horizontal=5
[ 1044.443066] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=5
[ 1044.451079] MOUSE moved UP RIGHT ....4 bytes in. Vertical=255,
Horizontal=8
[ 1044.459101] MOUSE moved UP RIGHT ....4 bytes in. Vertical=255,
Horizontal=5
[ 1044.467210] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=6
[ 1044.475219] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=6
[ 1044.483232] MOUSE moved UP RIGHT ....4 bytes in. Vertical=254,
Horizontal=5
[ 1044.491239] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=4
[ 1044.499130] MOUSE moved UP RIGHT ....4 bytes in. Vertical=255,
Horizontal=2
[ 1044.507138] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=2
[ 1044.515158] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=1
[ 1044.523285] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=3
[ 1044.531294] MOUSE moved UP RIGHT ....4 bytes in. Vertical=255,
Horizontal=2
[ 1044.539304] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=1
[ 1044.547307] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=2
[ 1044.555320] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=1
[ 1049.456885] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=255
[ 1049.464951] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=249
[ 1049.472965] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=248
[ 1049.480973] MOUSE moved UP LEFT ....4 bytes in. Vertical=255,
Horizontal=249
[ 1049.488923] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=249
[ 1049.496988] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=249
[ 1049.504999] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=251
[ 1049.513006] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=251
[ 1049.521021] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=252
[ 1049.529026] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=252
[ 1049.536979] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=254
[ 1049.545047] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=253
[ 1049.552997] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=254
[ 1049.561066] MOUSE moved UP LEFT ....4 bytes in. Vertical=254,
Horizontal=253
[ 1049.569075] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=255
[ 1049.577081] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=255
[ 1049.585093] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=253
[ 1049.593099] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=255
[ 1049.601052] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=253
[ 1049.609059] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=254
[ 1049.617034] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=253
[ 1049.625138] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=254
[ 1049.633088] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=255
[ 1049.641157] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=253
[ 1049.649166] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=254
[ 1049.657174] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=254
[ 1049.665187] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=255
[ 1049.673196] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=255
[ 1049.681205] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=255
[ 1053.453479] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=3
[ 1053.461545] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=4
[ 1053.469554] MOUSE moved RIGHT ....4 bytes in. Vertical=0,
Horizontal=1
[ 1053.477565] MOUSE moved UP RIGHT ....4 bytes in. Vertical=255,
Horizontal=1
[ 1053.685806] MOUSE moved UP RIGHT ....4 bytes in. Vertical=252,
Horizontal=1
[ 1053.693818] MOUSE moved UP ....4 bytes in. Vertical=254, Horizontal=0
[ 1053.701825] MOUSE moved UP ....4 bytes in. Vertical=253, Horizontal=0
[ 1053.709834] MOUSE moved UP LEFT ....4 bytes in. Vertical=251,
Horizontal=255
[ 1053.717844] MOUSE moved UP LEFT ....4 bytes in. Vertical=253,
Horizontal=254
[ 1053.725851] MOUSE moved UP ....4 bytes in. Vertical=254, Horizontal=0
[ 1053.733861] MOUSE moved UP LEFT ....4 bytes in. Vertical=252,
Horizontal=255
[ 1053.741871] MOUSE moved UP ....4 bytes in. Vertical=254, Horizontal=0
[ 1053.749876] MOUSE moved UP LEFT ....4 bytes in. Vertical=254,
Horizontal=255
[ 1053.757888] MOUSE moved UP ....4 bytes in. Vertical=254, Horizontal=0
[ 1053.765897] MOUSE moved UP LEFT ....4 bytes in. Vertical=252,
Horizontal=255
[ 1053.773848] MOUSE moved UP ....4 bytes in. Vertical=254, Horizontal=0
[ 1053.781919] MOUSE moved UP ....4 bytes in. Vertical=252, Horizontal=0
[ 1053.789924] MOUSE moved UP ....4 bytes in. Vertical=252, Horizontal=0
[ 1053.797934] MOUSE moved UP LEFT ....4 bytes in. Vertical=252,
Horizontal=253
[ 1053.805885] MOUSE moved UP LEFT ....4 bytes in. Vertical=253,
Horizontal=255
[ 1053.813953] MOUSE moved UP ....4 bytes in. Vertical=252, Horizontal=0
[ 1053.821962] MOUSE moved UP LEFT ....4 bytes in. Vertical=254,
Horizontal=255
[ 1053.829972] MOUSE moved UP LEFT ....4 bytes in. Vertical=253,
Horizontal=255
[ 1053.837980] MOUSE moved UP LEFT ....4 bytes in. Vertical=255,
Horizontal=255
[ 1053.853999] MOUSE moved UP LEFT ....4 bytes in. Vertical=255,
Horizontal=255
[ 1053.862009] MOUSE moved UP LEFT ....4 bytes in. Vertical=255,
Horizontal=255
[ 1053.870018] MOUSE moved UP LEFT ....4 bytes in. Vertical=254,
Horizontal=252
[ 1053.878030] MOUSE moved UP LEFT ....4 bytes in. Vertical=254,
Horizontal=253
[ 1053.885972] MOUSE moved UP LEFT ....4 bytes in. Vertical=255,
Horizontal=254
[ 1053.894044] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=253
[ 1058.915814] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=255
[ 1058.923737] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=254
[ 1058.931772] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=252
[ 1058.939843] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=252
[ 1058.947793] MOUSE moved DOWN LEFT ....4 bytes in. Vertical=1,
Horizontal=254
[ 1058.955802] MOUSE moved DOWN LEFT ....4 bytes in. Vertical=2,
Horizontal=251
[ 1058.963773] MOUSE moved DOWN LEFT ....4 bytes in. Vertical=1,
Horizontal=254
[ 1058.971879] MOUSE moved DOWN LEFT ....4 bytes in. Vertical=1,
Horizontal=254
[ 1058.979738] MOUSE moved DOWN LEFT ....4 bytes in. Vertical=1,
Horizontal=254
[ 1058.987897] MOUSE moved LEFT ....4 bytes in. Vertical=0,
Horizontal=253
[ 1058.995848] MOUSE moved DOWN LEFT ....4 bytes in. Vertical=1,
Horizontal=255
[ 1059.100026] MOUSE moved DOWN ....4 bytes in. Vertical=2, Horizontal=0
[ 1059.107918] MOUSE moved DOWN ....4 bytes in. Vertical=3, Horizontal=0
[ 1059.116040] MOUSE moved DOWN ....4 bytes in. Vertical=5, Horizontal=0
[ 1059.124052] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=5,
Horizontal=2
[ 1059.132062] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=6,
Horizontal=1
[ 1059.140071] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=4,
Horizontal=1
[ 1059.148077] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=5,
Horizontal=3
[ 1059.156030] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=5,
Horizontal=2
[ 1059.164099] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=6,
Horizontal=3
[ 1059.172048] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=5,
Horizontal=1
[ 1059.180057] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=4,
Horizontal=2
[ 1059.188125] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=6,
Horizontal=3
[ 1059.196138] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=5,
Horizontal=2
[ 1059.204142] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=7,
Horizontal=3
[ 1059.212155] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=10,
Horizontal=4
[ 1059.220162] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=8,
Horizontal=3
[ 1059.228055] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=7,
Horizontal=2
[ 1059.236182] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=8,
Horizontal=2
[ 1059.244192] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=7,
Horizontal=1
[ 1059.252197] MOUSE moved DOWN RIGHT ....4 bytes in. Vertical=6,
Horizontal=2
[ 1059.260106] MOUSE moved DOWN ....4 bytes in. Vertical=3, Horizontal=0
[ 1059.268219] MOUSE moved DOWN ....4 bytes in. Vertical=3, Horizontal=0
[ 1059.276228] MOUSE moved DOWN LEFT ....4 bytes in. Vertical=2,
Horizontal=255
[ 1059.284237] MOUSE moved DOWN LEFT ....4 bytes in. Vertical=1,
Horizontal=255

