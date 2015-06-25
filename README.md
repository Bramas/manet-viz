# manet-viz
manet vizualizer

# Import a trace
The following example is given with the Crowdad roma/taxi dataset available [here][1].
The traces are of the following format: 
	DRIVER_ID;TIMESTAMP;POSITION
The regular expression associated with it is:
	(\d+);(\d{4}\-\d{2}\-\d{2}\s\d{2}\:\d{2}\:\d{2})[^;]+;POINT\(([^\s;]+)\s([^\s;]+)\)
The time format is:
	yyyy-MM-dd HH:mm:ss

[1]:	http://crawdad.cs.dartmouth.edu/roma/taxi/ "Crawdad roma/taxi dataset"