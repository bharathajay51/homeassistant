# homeassistant
## Overview
   My personal [Home Assistant](https://www.home-assistant.io/) config. Currently running on a windows server (although not recommended :0). Using emqttd as mqtt broker (also running on the same server). Homebridge has also been installed for ios counterparts. 
   
  esp8266 development boards have been extensively used all around the build to make custom wifi enabled smart switches, lights, sensors etc.They are programmed using Arduino IDE and an FTDI programmer. **.ino** files for all the modules is provided. 

   All configs working with Home Assistant Version: 0.78.3 

 ## Installing
 ### Installing Home Assistant
 Home Assistant can be installed in several different ways. Choose what suits your tastes.
 
 Home Assistant currently reccomends insatallation using Hass.io
 
 - [Hass.io](https://www.home-assistant.io/getting-started/) - the easiest way (Also the reccomended way).
 - [Hassbian](https://www.home-assistant.io/docs/installation/hassbian/installation/) - Hassbian Installation.
 - [Manually](https://www.home-assistant.io/docs/installation/virtualenv/) - setting up Python environment and doing a Manual Install
 
 For windows use the following commands (**Make Sure that Python3 is installed**):
 ```
 py -m pip install homeassistant
 ```
 And for starting the server type:
 ```
 py -m homeassistant --open-ui
 ```

### Installing emqttd
The emttd broker does not have any limitations on number of clients. Also it has a good web-gui. 
Follow the instructions on the [emqttd](http://emqtt.io/) page to install the broker on your device.

**After installing emqttd cd into the folder and then type the following command to start or stop the server: (administrative privileges are required)**
```
.\bin\emqttd start | stop
```
The web interface will run on http://localhost:18083 (**username: admin || password: public**)

### Installing HomeBridge (For native ios support through [HomeKit](https://developer.apple.com/homekit/))
**Home Assistant natively supports HomKit since version 0.64.**

For older versions install [homebridge-homeassistant](https://github.com/home-assistant/homebridge-homeassistant).

### PushBullet
Configure [PushBullet](https://www.pushbullet.com/) for push notifications (you will need api key to perform push notifications).

### Configure OwnTracks
Configure [OwnTracks](https://owntracks.org/) with Home Assistant. OwnTracks allows you to track your devices in real time and thus assists in various automations like turning on lights when you are near your home.

## Acknowledgements
- **HomeAssistant** by far the most awesome home automation server there is.
- Home Assistant Community - again one of the best. Credits for all custom-ui components and themes.
- [mertenats/Open-Home-Automation](https://github.com/mertenats/Open-Home-Automation) - credits for the aweseome esp8266-Pir, rgb-led code.
