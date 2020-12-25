# RideHailingDataAnalyzer
🚖Analysis and Visualization of Online Ride-Hailing Order Data Project for PPPS

## Build Requirements

```shell
    Qt 5.15.2 MinGW 64 Release
    QtCharts
```
Open the .pro file in Qt and build the project

It may takes some time to load the map data from the OSM or Amap at the first time to run. (Make sure you have access to Internet)

## Load Data from .csv Files

Set the path to the direction of the dataset.  
Click OK and wait.  
After a while, you will see the loading work done successfully.  
![successfully loaded](image/img1.gif)

## Easy to Tune the Parameters

Time Span: support both calendar edit and slider.  
Fields: support sliders.
![setting fields and time span](image/img2.gif)

## Diverse Statistic Graphs
Three kinds of graphs:  
Number of Orders: get a quick view at the number of orders over time in a line chart.   
Orders percentage: display the percentage of orders leaving/entering/inside the area selected.  
Fee-Time scatter chat: diplay the distribution of both fee and time.
![statistic graph](image/img3.gif)

## Heat Map

Display the order in the map to show the distribution of the orders.  

heat map v1: scatter all the points to the map (running slow but contains more details).

heat map v2: count the number of points in every pixel of an image and transform it to colored heat map.(running faster than v1 to show the changes over time)

![heat map](image/img4.gif)

![heat map v2](image/img5.gif)

##  Travel Time Prediction

Predict the travel time from the given start point to the given end point.

Two methods are used to predict the travel time:

1. Find all the spatially and temporally related orders in the dataset, and get the average time of them as prediction.
2. Train deep neural network on the dataset and use the network to predict. Information about the network:
   1. Input:[departure_time,  weekday, orig_lon, orig_lat, orig_group, dest_lon, dest_lat, dest_group]
   2. Network: Eight Linear Layer with ReLU Layer in them.
   3. Output: [time]   
   4. Accuracy: 0.72 The probability that the error is within five minutes is 0.72   

![Prediction](image/img6.gif)

if the dataset don't have the similar orders, the Deep Neural Network will try to predict the travel time like the example below.

![predict with no similar order](image/img7.png)

## Related Files

[report](https://jbox.sjtu.edu.cn/l/b1kNbM)

[video](https://jbox.sjtu.edu.cn/l/QJGUff )

## Extern Library Used

[MapGraphics](https://github.com/raptorswing/MapGraphics) 