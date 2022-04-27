#Dimension: Augmented Reality CAD Viewer

##Overview
Dimension is an augmented reality app that allows you to project your CAD models into the real world. The project primarily uses Google's ARCore library for lighting detection and object positioning, OpenCascade as the CAD kernel, and OpenGL for rendering objects.

##Video Demonstration
Here is a video demonstration of Dimension. The program will first detect horizontal surfaces where you can place your CAD model. Then, touching a highlighted section of surface will place down an object. There are also three transform tools: translation, rotation, and scale. Translation moves the object in X,Y,Z space, rotation will rotate the object, and scale allows you to enlarge or shrink the object.

##Motivation
Modern CAD packages like Autodesk, Solidworks, and FreeCAD have revolutionized the way engineers design parts and mechanical systems. However, while these softwares are a far step up from the old pen and paper, they are still at times limiting. Half your time could be spent wrestling with the virtual camera to get a good view of your part and working in a blank void provides no environmental context for your designs. What if you could design your parts at the place where they are going to be used? If you are  designing a chair for example, you would be able to design it in the very room it is going to be used. This is the goal of Dimension.

The inspiration for Dimension came from the holograms used by Tony Stark in movies like Iron Man where he uses them to design his suits. Using holograms is infeasable due to technical limitations, however, augmented reality technology is incredibly mature and can be easily deployed on modern phones. Thus, the first prototype of Dimension utilizes Google's open source ARCore library. 

##Installation
To install Dimension on your Android device, simply download the apk from releases onto your device and then install the apk.
