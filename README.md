OSCeleton-Puppet
=========

This is a fork of OSCeleton that adds a number of hacks for a specific project.

Checkout [https://github.com/Sensebloom/OSCeleton](https://github.com/Sensebloom/OSCeleton) for more information

New Tags
--------
OSCeleton-Puppet spits out a number of new event tags, namely:

/pose_detected <user Id>
/calibration_started <user Id>
/calibration_failed <user Id>

New options
-----------
OSCeleton-Puppet adds a number of new command line options all starrting with -x.

### -xr
This option outputs the X,Y & Z data as "raw" kinect values

### -xt
This option outputs the joint rotation data.
    Address pattern: "/orient"
    Type tag: "sifffffffff"
    s: Joint name, check out the full list of joints below.
    i: The ID of the user.
    f f f: X axis orientation data
    f f f : Y axis orientation data
    f f f : Z axis orientation data
	
or if in Quartz Composer mode:

    Address pattern: "/joint/name/id"
    Type tag: "fffffffff"
    f f f: X axis orientation data
    f f f : Y axis orientation data
    f f f : Z axis orientation data
Example (torso of user 4):
    /orient/torso/4 0.938792 -0.0774589 0.335662 0.0649184 0.996714 0.0484401 -0.338311 -0.0236846 0.940736

### -xa
This option outputs the audio level of the default microphone in a range of [0..1]. 0 = no sound, 1 = max sound. This is only for WIN32 builds.
    Address pattern: "/audio_level"
    Type tag: "f"
    f : Audio level in the range [0..1]

Example:
    /audio_level 0.429688	
	
### -xd
Runs the program with a number of options enabled by default: -xr -xt -xa -p -w -r