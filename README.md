# manet-viz
manet vizualizer

## Import a trace
The following example is given with the Crowdad roma/taxi dataset available [here][1].
The traces are of the following format:
```
DRIVER_ID;TIMESTAMP;POSITION
```
The regular expression associated with it is:
```
(\d+);(\d{4}\-\d{2}\-\d{2}\s\d{2}\:\d{2}\:\d{2})[^;]+;POINT\(([^\s;]+)\s([^\s;]+)\)
```
The time format is:
```
yyyy-MM-dd HH:mm:ss
```

The input projection of the coordinates in the dataset is WGS84 ([EPSG 4326][2]):
```
+proj=longlat +ellps=WGS84 +datum=WGS84 +no_defs
```

Since we want to work with distances and spatial operations, we use a projected coordinate system that fits with the coordinate extent of the dataset, namely UTM zone 33N ([EPSG 3065][3]):
```
+proj=utm +zone=33 +ellps=WGS84 +towgs84=0,0,0,0,0,0,0 +units=m +no_defs
```

*Note.*  You can use [this website][4] to transform WGS 84 lat/lon coordinates into UTM x/y coordinates. The website also gives the UTM zone associated with the lat/lon coordinates.

## Add a plugin

To add a new plugin under QT:
Right click on Plugin directory > Add new Subproject > Library > C++ Library > Fill the name > Requires `QtWidgets`,`QtGui`,`QtCore` > Create the files you need to implement the decorator
Add in the `*.pro` file:
```
QT       += widgets concurrent
...
INCLUDEPATH    += ../../manetviz
DESTDIR         = ../../manetviz/plugins
```

In the `<plugin_name>.h` file, add:
```c++
#include <QObject>
#include "iplugin.h"
```

Make the plugin inherit from the interface (eg. `IPlugin`):

```c++
class <plugin_name>: public QObject, public IPlugin
{
  Q_OBJECT
  Q_PLUGIN_METADATA(IID "org.manet-viz.IPlugin" FILE "<plugin_name>.json")
  Q_INTERFACES(IPlugin)

public:
  <plugin_name>(); // constructor
  // implement the abstract method
  // inherited from IPlugin
  QObject * getQObject() { return this; }
};
```

Create a new file in the plugin directory of the name: `<plugin_name>.json` that includes:
```
{
"name" : "<plugin_name>",
"version" : "0.0.1",
"dependencies" : [],
"active" : "true"
}
```

#### Add a control Widget to a plugin
On the plugin folder, right click > Add new > Qt > Qt Designer Form > Widget > Name `control.ui` > Finish

Add the elements you want into the designer view. **Do not forget to set a minimum width/height for the QWidget to see it.**

In the `<plugin_name>.h`, add the following:
```c++
namespace Ui {
    class Control;
}
```
Add a private variable `ui`:
```c++
Ui::Control *ui;
```

In the `<plugin_name>.cpp`, add the following:
```c++
#include "ui_control.h"
```

Set the UI attribute for the plugin in the constructor in the `cpp` file:
```
<plugin_name>::<plugin_name>():
  ui(new Ui::Control) { ... }
```

Implement the `QWidget * createControlWidget() const` method:
```c++
QWidget * control = new QWidget();
ui->setupUi(control);
... // init the widgets and connections
return control;
```

Clean all and run `qmake`

[1]:	http://crawdad.cs.dartmouth.edu/roma/taxi/ "Crawdad roma/taxi dataset"
[2]: http://spatialreference.org/ref/epsg/wgs-84/ "ESPG:4326"
[3]: http://spatialreference.org/ref/epsg/3065/ "EPSG:3065"
[4]: http://www.movable-type.co.uk/scripts/latlong-utm-mgrs.html "WGS84 to UTM coordinates"
