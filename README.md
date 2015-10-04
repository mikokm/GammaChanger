##GammaChanger
GammaChanger is a tool that changes Windows gamma ramp and AMD display driver saturation settings. This is useful for example with games that benefit from higher contrast and brightness settings.

##Usage:

**GammaChanger.exe load \<filename\>**

Loads the file containing the saved gamma ramp and applies it. Sets the saturation setting to 100.

**GammaChanger.exe save \<filename\>**

Writes the current gamma ramp into the file.

**GammaChanger.exe set \<gamma\>**

Calculates a new gamma ramp for the given gamma value and applies it. The gamma value has to be between 0.1 and 3.0. Also sets the saturation setting to 200.
