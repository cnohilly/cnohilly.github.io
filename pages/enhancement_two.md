# Enhancement Two
## Algorithms and Data Structures 

### Explanation
For this category, the artifact I chose to enhance was the final project for the course CS330 - Computer Graphics and Visualization - which I took in the summer of 2020. In this project, we had to create a 3D lit and textured object with a toggle orthographic view using OpenGL and C++. An example of how this project looked when submitted can be seen [here](https://www.youtube.com/watch?v=XyaaWhLJi_w). I chose to include this artifact because at the time of the project I had written several hundred lines of repetitive code, roughly 350 lines, of plotting the 3D coordinates, normals and texture coordinates for each vertex of the chair. I wished to use this enhancement to demonstrate capability with algorithms and data structures to simply code like this and be more efficient and clean. 

### Enhancement Plan
<img src="../images/WoodChair.jpg" width="200" height="250" align="right">
The enhancement plan briefly mentioned above is to create functions and better use algorithms and data structures to reduce the amount of lines that it takes to plot the vertices of my object. In the case of my project, I had created a chair that was a simple wooden chair where I used the refernce image shown below, and this required about 350 vertices which I did each one manually in the original rendition of the project. In the enhancement, I created a single function that takes in a vector arguent and 6 GLfloat values which represent 2 x-coordinate values, 2 y-coordinate values, and 2 z-coordinate values. With these 6 values, I am able to create a 3D piece of my chair with a single call of the function rather than 36 lines of manually placing coordinates for 6 vertices for each of the 6 sides. 


### Outcomes
I chose this artifact because I felt it was a chance to show proficiency in a different language, show problem solving skills, and show an ability to learn new things when necessary. Going into the enhancement, I had some prior knowledge using SQL, but had to do further research to get a database using MySQL set up and learn to use JDBC to interface between the program and the database. I believe with the enhancements I've made that I have achieved the goals and objectives that I was meant to. I did not find that there were a large number of challenges to accomplish this enhancement and it primarily required me to become reacquainted with both Java and SQL and learn a few new things in Java to create the user interface that I wanted. 

### Enhancement One Files
[Github containing C++ and Java Versions](https://github.com/cnohilly/cnohilly.github.io/tree/main/Enhancement_One_Files)

### Navigation
- [Professional Self-Assessment](../index.md)
- [Enhancement One](./enhancement_one.md)
