# ARP-Assignment-2
## Advanced Robot Programming - Transfer data from a Producer to a Consumer - Robotics Engineering
### Author: Simone Contorno

<br>

### Introduction
An overview of this program function.<br>
[Go to Introduction](#intro)

### How it works
A rapid description of how the program works.<br>
[Go to How it works](#how)

### Installation and Execution
How install and run this program in Linux.<br>
[Go to Installation and Execution](#installation)

### Conclusion
Conclusion.<br>
[Go to Conclusion](#con)

<a name="intro"></a>
### Introduction

This program allow to the user to send a random message with a maximum size of 100 MB, using four possible IPC (Inter Process Communication):
<ul>
  <li>Unnamed pipe</li>
  <li>Named pipe</li>
  <li>Socket</li>
  <li>Shared memory with circular buffer</li>
</ul>
The aim is to test the transmission velocities of these four different methods.

<a name="how"></a>
### How it works

The program asks to the user to type the dimension of the random message (with maximum size of 100 MB) and the IPC to use. <br>
In case of socket, an host address and a port will be ask too. <br;
Afterwards, the program will execute the corresponding process to transmit the message showing a progression bar and, in the end, reporting the total amount of time required.

<a name="installation"></a>
### Installation and Execution

Download the repository:

<pre><code>git clone https://github.com/simone-contorno/ARP-Assignment-2</code></pre>

After, go into the directory and make executable all .sh files:

<pre><code>chmod +x install.sh run.sh help.sh</code></pre>

To compile the source files, type:

<pre><code>./install.sh</code></pre>

Now, to exec the program, type:

<pre><code>./run.sh </code></pre>

If you want read some specific description about a process, type:

<pre><code>./help.sh < number ></code></pre>

where < number > is the corresponding process text file description.

<a name="con"></a>
### Conclusion

Thanks to have read this file, i hope it was clear and interesting.<br>
<br>
Collaborator: Gabriele Russo - @GabrieleRusso11

