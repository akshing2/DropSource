# DropSource
This is a repository for a DOD droplet formation analysis app. This is part of an undergraduate research project in collaboration with the University of New South Wales and Inventia Life Science. 

## Aim of The Project
The purpose of this thesis project is to deliver an easy to use GUI application for researchers to analyse Drop-On-Demand Droplet formation from piezoelectric printerheads. Users input a series of images collected from a high speed camera.
The following parameters will be analysed:

1. Droplet Position.
2. Droplet Velocity.
3. Number of Satellites.
4. Length of Main Droplet Ligament.
5. Volume of Main Droplet.

## External Libraries Used
This project incorporates OpenCV 4.1.2 libraries to conduct image processing and subsequent droplet analysis. A free version of libxl, an excel spreadsheet read/write softwarewas also incorporated.

## Current Functionality
At this stage, the app is able to do the following:

1. Run the GUI, with input error checking.
2. Itterate through selected input directory.
3. Create a time vector based off FPS (Hz) input.
4. Determine main droplet position and errors associated with image processing.
5. Calculate main droplet velocities based off 4.
6. Determine number of satellites in each frame. Note: need to see if image quality is sufficient.
7. Calculate the length of the ligament.
8. Calculate the volume of the main droplet (excludes satellites).
9. Write all data to an excel spreadsheet.


## TODO
1. Output frames with time stamp and calculted droplet position/velocity as well as number of satellites. This is purely for the purpose of validation.

