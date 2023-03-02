# This is the individual coursework of Operating System Lab in KUL coached by Professors Jeroen Van Aken, Yuri Cauwets, Ludo Bruynseels, and Bert Lagaisse.

# reference of code from external sources is cited in my code

## Sensor Monitoring System
The sensor monitoring system consists of client-side sensor nodes measuring the room temperature, and a central server-side sensor-gateway that acquires all sensor data from the sensor nodes. A sensor node uses a TCP connection to transfer the sensor data to the sensor gateway. The full system is depicted below.

<img width="433" alt="image" src="https://user-images.githubusercontent.com/100540403/222374835-a3176c0d-f7a1-48f7-bdbf-c62a747b9e56.png">

The sensor gateway may not assume a maximum amount of sensors at start up. In fact, the number of sensors connecting to the sensor gateway is not constant and may change over time.

Working with real embedded sensor nodes is not an option for this assignment. Therefore, sensor nodes will be simulated in software using a client-side sensor-node (see sensor_node.c from plab3, which you can use to test your server implementation).

## Sensor Gateway
A more detailed design of the sensor gateway is depicted below. 

<img width="543" alt="image" src="https://user-images.githubusercontent.com/100540403/222375089-df535bc0-0ca3-4338-a7a8-5e4f5ec2fef0.png">

## note
Every branch contains a `.gitignore` file that instructs git to automatically ignore `.a`, `.o`, `.so` files. Most of these are compiled files and don't belong in a repository. Besides these, `.zip` files probably include files that are already in your repo, so these are also ignored.
