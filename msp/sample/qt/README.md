
(1) Function description:
qt folder below the code, is the SDK package provides an example of reference code, to facilitate the rapid use of QT customers.

(2) The default resolution of the GUI drawn by QT in this example is 1920x1080 output through HDMI0.
    A. Run the command as follows:
	   export QT_QPA_PLATFORM_PLUGIN_PATH=/opt/lib/qt-5.15.7/plugins
	   export QT_QPA_PLATFORM=linuxfb:size=1920x1080
	   export XDG_RUNTIME_DIR=/usr/lib/
	   export LD_LIBRARY_PATH='/usr/local/lib:/usr/lib:/opt/lib:/soc/lib:/opt/lib/qt-5.15.7/lib'
	   sample_qt