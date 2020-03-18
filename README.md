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

1. Run a windows form, asking for user inputs such as:
	1. Test parameters information (Name of test, ROI dimensions, frame rate etc).
	2. File I/O parameters (where are input frames located, where do you want the outputs saved).
	3. Processing parameters (what image segmentation methods do you want to employ, what drop parameters to analyse).
2. Run computer vision analysis on selected parameters.
3. Provide the following output files:
	1. Excel Spreadsheet, which tracks the parameters selected for each frame in the input directory.
	2. Debug Images, a folder that contains debug images for visual inspection of the workings of the algorithms. 

## Need for Validation
The algorithms used need to be validated through scientific means.

